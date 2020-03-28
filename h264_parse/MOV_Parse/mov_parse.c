#include <stdio.h>
#include <stdlib.h> 
#include <string.h>

#include "mov_parse.h"
                  
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

int get_frame_by_chunk(FILE *f,struct MY_MOV_CHUNK_T *chunk,int chunk_offset,unsigned char *buf)
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
        printf(" aud frame    | %d \n",chunk->aud_size);
        offset = chunk->aud_size;
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
static int __is_I_frame(FILE *f,int chunk_offset) 
{
    unsigned char cache[READ_SIZE];
    int sps_len = 0;
    int pps_len = 0;
    if(!f){
        printf("file is null!!!\n");
        return -1;
    }
    fseek(f,chunk_offset,SEEK_SET);
    fread(cache,READ_SIZE,1,f);
    fseek(f,chunk_offset,SEEK_SET);
    if((cache[0] == 0) && (cache[4] == 0x67)) {
        //sps
        printf("sps\n");
        memcpy(&sps_len, &cache[0], 4);
        sps_len = ntohl(sps_len);	
        if ((cache[sps_len+4] == 0) && (cache[(sps_len+4)+4] == 0x68)) {
            //pps
            printf("pps\n");
            memcpy(&pps_len, &cache[sps_len+4], 4);
            pps_len = ntohl(pps_len);
            if ((cache[(sps_len+4)+pps_len+4] == 0) && (cache[(sps_len+4)+pps_len+4+4] == 0x65)) {
                printf("III\n");   
                return 1;
            }
        }
    }
    return 0;
}

int realse_chunk_attr(struct MY_MOV_CHUNK_T *chunk)
{
    chunk->moov_len = 0;
    chunk->aud_size = 0;
    chunk->aud_frame_number = 0;
    chunk->frame_number = 0;
    chunk->vid_fps = 0;
    if(chunk->vide_stco_buf){
        free(chunk->vide_stco_buf);
    }
    if(chunk->soun_stco_buf){
        free(chunk->soun_stco_buf);
    }
    if(chunk->frames_offset_buf){
        free(chunk->frames_offset_buf);
    }
    return 0; 
}

int get_chunk_attr(FILE *f,struct MY_MOV_CHUNK_T *chunk)
{
	unsigned char cache[READ_SIZE];
	unsigned char *p;
	int offset = 0;
	int moov_offset = 0;
    unsigned char *buf = NULL;

	//定位mdat size 
	fread(cache,READ_SIZE,1,f);
	//printf_buf(cache,READ_SIZE);
	p = memmem(cache,READ_SIZE,"wide",4);
	printf("p:%s\n",p);
    memcpy(&offset,p+4,4);
    offset = ntohl(offset);
    printf("offset:%x\n",offset);

    //获取帧率
    memcpy(&chunk->vid_fps,&cache[48],4);
    printf("video_chunk.vid_fps:%d\n",chunk->vid_fps);

    //获取音频帧大小
    memcpy(&chunk->aud_size,&cache[72],4);
    printf("video_chunk.aud_size:%x\n",chunk->aud_size);

    //偏移到moov 
    moov_offset = (p-cache)+4+offset; 
    fseek(f,moov_offset,SEEK_SET);
    fread(cache,READ_SIZE,1,f);
    memcpy(&chunk->moov_len,&cache[0],4);
    chunk->moov_len = ntohl(chunk->moov_len);
    printf("video_chunk.moov_len:%x\n",chunk->moov_len);
    fseek(f,moov_offset,SEEK_SET);//恢复到moov

    //读出整个moov索引表
    buf = malloc(chunk->moov_len);
    fread(buf,chunk->moov_len,1,f);

    //找到stco
	p = memmem(buf,chunk->moov_len,"stco",4);
    //找到帧数量
    memcpy(&chunk->frame_number,p+8,4);
    chunk->frame_number = ntohl(chunk->frame_number);
    printf("frame_number:%d\n",chunk->frame_number);
    //获取帧索引
    chunk->vide_stco_buf = (int *)malloc(chunk->frame_number * 4);
    //找到帧偏移量
    memcpy(chunk->vide_stco_buf,p+12,(chunk->frame_number * 4));
    printf("first_frame_chuck:%x\n",ntohl(chunk->vide_stco_buf[0]));

    if(chunk->aud_size) {
        //寻找音频stco
        unsigned char *tmp_p = (p+4);
        int left_size = chunk->moov_len - (tmp_p - buf);
        p = memmem(tmp_p,left_size,"stco",4);
        //找到帧数量
        memcpy(&chunk->aud_frame_number,p+8,4);
        chunk->aud_frame_number = ntohl(chunk->aud_frame_number);
        printf("aud_frame_number:%d\n",chunk->aud_frame_number);
        chunk->soun_stco_buf = (int *)malloc(chunk->aud_frame_number * 4);
        //获取帧索引
        memcpy(chunk->soun_stco_buf,p+12,(chunk->aud_frame_number * 4));
        printf("first_aud_frame_chuck:%x\n",ntohl(chunk->soun_stco_buf[0]));
        /* for(int k=0;k<chunk->aud_frame_number;k++){ */
            /* printf("[%d]aud_frame_chuck:%x\n",k,ntohl(chunk->soun_stco_buf[k])); */
        /* } */
    } else {
        //无音频
        chunk->aud_frame_number = 0;
        chunk->soun_stco_buf = NULL;
    }
    
    if(buf){
        free(buf);
        buf = NULL;
    }

    return 0;
}
int sort_vide_soun_chunk(struct MY_MOV_CHUNK_T *chunk,int sec) 
{
    /**************重新排序视频帧和音频帧Offset**************/
    if(chunk->aud_size && chunk->soun_stco_buf) {
        int x = chunk->frame_number - (sec * chunk->vid_fps);
        int y = chunk->aud_frame_number - (sec * 2);
        int cnt;
        int total_frames_number = chunk->frame_number+chunk->aud_frame_number;
        chunk->frames_offset_buf = (int *)malloc(total_frames_number*4);
        for(cnt=0;cnt<total_frames_number;cnt++){
           if(ntohl(chunk->vide_stco_buf[x])<ntohl(chunk->soun_stco_buf[y])) { 
               chunk->frames_offset_buf[cnt] = chunk->vide_stco_buf[x];
               x++; 
               /* printf("V[%d]offs=0x%x\n",cnt,ntohl(chunk->frames_offset_buf[cnt])); */
           } else {
               chunk->frames_offset_buf[cnt] = chunk->soun_stco_buf[y];
               y++; 
               /* printf("A[%d]offs=0x%x\n",cnt,ntohl(chunk->frames_offset_buf[cnt])); */
           }
        }
    }

   return 0; 
}

