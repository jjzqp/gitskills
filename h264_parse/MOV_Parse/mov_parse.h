#ifndef __MOV_PARSE_H_
#define __MOV_PARSE_H_

#define _GNU_SOURCE
#define ntohl(x) (unsigned int)((((unsigned int)(x))>>24) | ((((unsigned int)(x))>>8)&0xff00) | (((unsigned int)(x))<<24) | ((((unsigned int)(x))&0xff00)<<8))
#define ntoh(x) (unsigned short)((((unsigned int)(x))>>8&0x00ff) | (((unsigned int)(x))<<8&0xff00))

#define htonl(A)  ((((unsigned int)(A) & 0xff000000) >> 24) | \
                  ((( unsigned int)(A) & 0x00ff0000) >> 8)  | \
                  ((( unsigned int)(A) & 0x0000ff00) << 8)  | \
                  ((( unsigned int)(A) & 0x000000ff) << 24))


#define  CHANGE_264_DATA   

#define READ_SIZE   512 

struct MY_MOV_CHUNK_T {
	int moov_len;
    int aud_size;       //音频帧大小
    int aud_frame_number; //音频帧数量
    int vid_fps;        //视频帧率
    int frame_number;   //视频帧数量
    int *vide_stco_buf; //视频索引buf
    int *soun_stco_buf; //音频索引buf
    int *frames_offset_buf; //全部帧索引buf(视频和音频排序后)
};


int change_buf_data(unsigned char *buf);
int get_frame_by_chunk(FILE *f,struct MY_MOV_CHUNK_T *chunk,int chunk_offset,unsigned char *buf);

int get_chunk_attr(FILE *f,struct MY_MOV_CHUNK_T *chunk);
int realse_chunk_attr(struct MY_MOV_CHUNK_T *chunk);

int sort_vide_soun_chunk(struct MY_MOV_CHUNK_T *chunk,int sec);
#endif

