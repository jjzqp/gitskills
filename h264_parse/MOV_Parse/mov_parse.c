#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#define _GNU_SOURCE
#define ntohl(x) (unsigned int)((((unsigned int)(x))>>24) | ((((unsigned int)(x))>>8)&0xff00) | (((unsigned int)(x))<<24) | ((((unsigned int)(x))&0xff00)<<8))
#define ntoh(x) (unsigned short)((((unsigned int)(x))>>8&0x00ff) | (((unsigned int)(x))<<8&0xff00))

#define htonl(A)  ((((unsigned int)(A) & 0xff000000) >> 24) | \
                  ((( unsigned int)(A) & 0x00ff0000) >> 8)  | \
                  ((( unsigned int)(A) & 0x0000ff00) << 8)  | \
                  ((( unsigned int)(A) & 0x000000ff) << 24))
                  
/* #define  CHANGE_264_DATA    */

#define READ_SIZE   512 

struct MY_MOV_CHUNK_T {
	int moov_len;
    int aud_size;       //音频帧大小
    int aud_frame_number; //音频帧数量
    int frame_number;   //视频帧数量
    int *vide_stco_buf; //视频索引buf
    int *soun_stco_buf; //音频索引buf
    int *frames_offset_buf; //全部帧索引buf(视频和音频排序后)
};
static struct MY_MOV_CHUNK_T video_chunk = {0};

int printf_buf(void *buf,int len)
{
	int i = 0 ;
	unsigned char *tmp = (unsigned char*)buf;
	for(i=0;i<len;i++)
		printf("%x ",tmp[i]);
	printf("\n");
	return 0;
}

int change_buf_data(unsigned char *buf)
{

#ifdef  CHANGE_264_DATA   
    unsigned int change_data = 0x01000000;
    unsigned int tmp, tmp2;
    if (*((char *)(buf + 4)) == 0x67) {
        memcpy(&tmp, buf, 4);
        tmp = htonl(tmp);
        memcpy(buf, &change_data, 4);
        memcpy(&tmp2, buf + tmp + 4, 4);
        tmp2 = htonl(tmp2);
        memcpy(buf + tmp + 4, &change_data, 4);
        memcpy(buf + tmp + tmp2 + 8, &change_data, 4);
    } else {
        memcpy(buf, &change_data, 4);
    }
#endif
	return 0;
}
extern void *memmem(const void *__haystack, size_t __haystacklen,
                    const void *__needle, size_t __needlelen);

static int get_frame_by_chunk(FILE *f,int chunk_offset,unsigned char *buf)
{
    unsigned char cache[READ_SIZE];
    int sps_len = 0;
    int pps_len = 0;
    int offset = 0;
    int ret = 0;
    int cur_addr = chunk_offset;
    int flen;
    if(!f){
        printf("file is null!!!\n");
        return -1;
    }
    fseek(f,chunk_offset,SEEK_SET);
    fread(cache,READ_SIZE,1,f);
    cur_addr += READ_SIZE;
    fseek(f, cur_addr-READ_SIZE, SEEK_SET);
    cur_addr -= READ_SIZE;
    if ((cache[0] == 0) && (cache[4] == 0x41) && (cache[5] == 0x9a)) {
        memcpy(&offset, &cache[0], 4);
        offset = ntohl(offset);
        offset += 4;
        printf(" P frame   | %d \n",offset);
        ret = fread(buf,offset,1,f);
        if(ret == 0){
            printf("end\n");
            return -1;
        }
        cur_addr += offset;
        printf("cur_addr==%x\n",cur_addr);
        return offset;
    } else if ((cache[0] == 0) && (cache[4] == 0x67)) {
        //sps
        printf("sps\n");
        printf_buf(cache,32);
        memcpy(&sps_len, &cache[0], 4);
        sps_len = ntohl(sps_len);	
        if ((cache[sps_len+4] == 0) && (cache[(sps_len+4)+4] == 0x68)) {
            //pps
            printf("pps\n");
            memcpy(&pps_len, &cache[sps_len+4], 4);
            pps_len = ntohl(pps_len);
            if ((cache[(sps_len+4)+pps_len+4] == 0) && (cache[(sps_len+4)+pps_len+4+4] == 0x65)) {
                printf("III\n");   
                memcpy(&offset, &cache[(sps_len+4)+pps_len+4], 4);                   
                offset = ntohl(offset);                 
                flen = offset + sps_len + pps_len + 12;
                ret = fread(buf,flen,1,f);
                cur_addr += flen;
                printf("cur_addr==%x\n",cur_addr);
                if(ret == 0){
                    printf("end\n");
                }
                return flen;
            }
        }
    } else {
        printf(" aud frame    | %d \n",video_chunk.aud_size);
        offset = video_chunk.aud_size;
        ret = fread(buf,offset,1,f);
        if(ret == 0){
            printf("end\n");
            return -1;
        }
        cur_addr += offset;
        printf("cur_addr==%x\n",cur_addr);
        return offset;
    }

    return -1;
}
int main()
{	

	/* FILE *f = fopen("VID_001.MOV","rb"); */
	FILE *f = fopen("VID_002.MOV","rb");
    FILE *f_out = fopen("VIDEO.DAT","wb");
	unsigned char cache[READ_SIZE];
	unsigned char *p;
	int offset = 0;
	int moov_offset = 0;
    int len;
    int i;
    unsigned char *buf = NULL;

	//定位mdat size 
	fread(cache,READ_SIZE,1,f);
	//printf_buf(cache,READ_SIZE);
	p = memmem(cache,READ_SIZE,"wide",4);
	printf("p:%s\n",p);
    memcpy(&offset,p+4,4);
    offset = ntohl(offset);
    printf("offset:%x\n",offset);

    //获取音频帧大小
    memcpy(&video_chunk.aud_size,&cache[72],4);
    /* video_chunk.aud_size = ntohl(video_chunk.aud_size); */
    printf("video_chunk.aud_size:%x\n",video_chunk.aud_size);
    
    //偏移到moov 
    moov_offset = (p-cache)+4+offset; 
    fseek(f,moov_offset,SEEK_SET);
    fread(cache,READ_SIZE,1,f);
    memcpy(&video_chunk.moov_len,&cache[0],4);
    video_chunk.moov_len = ntohl(video_chunk.moov_len);
    printf("video_chunk.moov_len:%x\n",video_chunk.moov_len);
    fseek(f,moov_offset,SEEK_SET);//恢复到moov

    //读出整个moov索引表
    buf = malloc(video_chunk.moov_len);
    fread(buf,video_chunk.moov_len,1,f);
#if 0
    //判断帧类型
	p = memmem(buf,video_chunk.moov_len,"mhlr",4);
    memcpy(&video_chunk.chunk_type,p+4,4);
    video_chunk.chunk_type = ntohl(video_chunk.chunk_type);
    if(video_chunk.chunk_type == 0x76696465){
        printf("vide\n");
    } else {
        printf("soun\n");
    }
#endif
    //找到stco
	p = memmem(buf,video_chunk.moov_len,"stco",4);
    //找到帧数量
    memcpy(&video_chunk.frame_number,p+8,4);
    video_chunk.frame_number = ntohl(video_chunk.frame_number);
    printf("frame_number:%d\n",video_chunk.frame_number);
    //获取帧索引
    video_chunk.vide_stco_buf = (int *)malloc(video_chunk.frame_number * 4);
    //找到帧偏移量
    memcpy(video_chunk.vide_stco_buf,p+12,(video_chunk.frame_number * 4));
    printf("first_frame_chuck:%x\n",ntohl(video_chunk.vide_stco_buf[0]));
    

    if(video_chunk.aud_size) {
        //寻找音频stco
        unsigned char *tmp_p = (p+4);
        int left_size = video_chunk.moov_len - (tmp_p - buf);
        p = memmem(tmp_p,left_size,"stco",4);
        //找到帧数量
        memcpy(&video_chunk.aud_frame_number,p+8,4);
        video_chunk.aud_frame_number = ntohl(video_chunk.aud_frame_number);
        printf("aud_frame_number:%d\n",video_chunk.aud_frame_number);
        video_chunk.soun_stco_buf = (int *)malloc(video_chunk.aud_frame_number * 4);
        //获取帧索引
        memcpy(video_chunk.soun_stco_buf,p+12,(video_chunk.aud_frame_number * 4));
        printf("first_aud_frame_chuck:%x\n",ntohl(video_chunk.soun_stco_buf[0]));
        /* for(int k=0;k<video_chunk.aud_frame_number;k++){ */
            /* printf("[%d]aud_frame_chuck:%x\n",k,ntohl(video_chunk.soun_stco_buf[k])); */
        /* } */
    } else {
        //无音频
        video_chunk.aud_frame_number = 0;
        video_chunk.soun_stco_buf = NULL;
    }

    if(buf){
        free(buf);
        buf = NULL;
    }
    /**************重新排序视频帧和音频帧Offset**************/
    if(video_chunk.aud_size && video_chunk.soun_stco_buf) {
        int x = 0;
        int y = 0;
        int cnt;
        int total_frames_number = video_chunk.frame_number+video_chunk.aud_frame_number;
        video_chunk.frames_offset_buf = (int *)malloc(total_frames_number*4);
        for(cnt=0;cnt<total_frames_number;cnt++){
           if(ntohl(video_chunk.vide_stco_buf[x])<ntohl(video_chunk.soun_stco_buf[y])) { 
               video_chunk.frames_offset_buf[cnt] = video_chunk.vide_stco_buf[x];
               x++; 
               /* printf("V[%d]offs=0x%x\n",cnt,ntohl(video_chunk.frames_offset_buf[cnt])); */
           } else {
               video_chunk.frames_offset_buf[cnt] = video_chunk.soun_stco_buf[y];
               y++; 
               /* printf("A[%d]offs=0x%x\n",cnt,ntohl(video_chunk.frames_offset_buf[cnt])); */
           }
        }
    }

    //准备帧buffer
    buf = malloc(1024*1024);
    for(i=0;i<video_chunk.frame_number+video_chunk.aud_frame_number;i++) {
        len = get_frame_by_chunk(f,ntohl(video_chunk.frames_offset_buf[i]),buf);
        /* change_buf_data(buf); */
        fwrite(buf,len,1,f_out);
    }
	
    if(buf){
        free(buf);
        buf = NULL;
    }
    if(video_chunk.vide_stco_buf){
        free(video_chunk.vide_stco_buf);
    }
    if(video_chunk.soun_stco_buf){
        free(video_chunk.soun_stco_buf);
    }
    if(video_chunk.frames_offset_buf){
        free(video_chunk.frames_offset_buf);
    }
	if(f){
		fclose(f);
	}
	if(f_out){
		fclose(f_out);
	}
	
	return 0;
}
