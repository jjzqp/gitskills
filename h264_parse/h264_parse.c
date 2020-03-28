#include <stdio.h>
#include <stdlib.h> 
#include <string.h>

#define ntohl(x) (unsigned int)((((unsigned int)(x))>>24) | ((((unsigned int)(x))>>8)&0xff00) | (((unsigned int)(x))<<24) | ((((unsigned int)(x))&0xff00)<<8))
#define ntoh(x) (unsigned short)((((unsigned int)(x))>>8&0x00ff) | (((unsigned int)(x))<<8&0xff00))

#define htonl(A)  ((((unsigned int)(A) & 0xff000000) >> 24) | \
                  ((( unsigned int)(A) & 0x00ff0000) >> 8)  | \
                  ((( unsigned int)(A) & 0x0000ff00) << 8)  | \
                  ((( unsigned int)(A) & 0x000000ff) << 24))
                  
//#define  CHANGE_264_DATA   

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
#ifdef CHANGE_264_DATA
	unsigned int change_data = 0x01000000;
 	unsigned int tmp, tmp2, tmp3;
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
int main()
{
	#define READ_SIZE    64
	printf("test...\n");
	int ret;
	unsigned char cache[READ_SIZE];
	int sps_len = 0;
	int pps_len = 0;
    int fnum = 0;
    int cur_addr = 0;
    int flen = 0;

	FILE *f = fopen("src.264","rb");
	FILE *f_out = fopen("out.264","wb");
	int offset = 0;
	int cnt = 0;
	unsigned char *buf = (unsigned char *)malloc(1024*1024);
	while(1) {
		ret = fread(cache,READ_SIZE,1,f);
		cur_addr += READ_SIZE;
		fseek(f, cur_addr-READ_SIZE, SEEK_SET);
		cur_addr -= READ_SIZE;
		//printf_buf(cache,READ_SIZE);
		cnt++;
		if(ret == 0){
			printf("eend\n");
			goto __END;
			
		}
	
        if ((cache[0] == 0) && (cache[4] == 0x41) && (cache[5] == 0x9a)) {
        	memcpy(&offset, &cache[0], 4);
        	offset = ntohl(offset);
        	offset += 4;
        	printf(" %d | P frame   | %d \n",fnum,offset);
        	//fseek(f, offset-READ_SIZE, SEEK_CUR);
        	ret = fread(buf,offset,1,f);
       		if(ret == 0){
				printf("end\n");
				goto __END;
				
			}
        	cur_addr += offset;
        	//printf("cur_addr==%x\n",cur_addr);
        	change_buf_data(buf);
        	fnum++;
        	fwrite(buf,offset,1,f_out);
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
                printf_buf(&pps_len,4);
                if ((cache[(sps_len+4)+pps_len+4] == 0) && (cache[(sps_len+4)+pps_len+4+4] == 0x65)) {
                    printf("III\n");   
                    memcpy(&offset, &cache[(sps_len+4)+pps_len+4], 4);                   
                    offset = ntohl(offset);                 
                    flen = offset + sps_len + pps_len + 12;
                    ret = fread(buf,flen,1,f);
                    cur_addr += flen;
                    //printf("cur_addr==%x\n",cur_addr);
                       if(ret == 0){
                        printf("end\n");
                        goto __END;
                        
                    }
                }
                change_buf_data(buf);
                
            }
            
            fwrite(buf,flen,1,f_out);
        } else if((cache[0] == 0x5A) && (cache[1] == 0x00)){
        	
         printf(" %d | sound     |  \n",fnum);
         ret = fread(buf,0x2000,1,f);
         cur_addr += 0x2000;

        }

        }
	
__END:
	if(f){
		fclose(f);
	}
	if(f_out){
		fclose(f_out);
	}
	if(buf){
		free(buf);
	}
	
	return 0;
}
