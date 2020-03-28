#include <stdio.h>
#include <stdlib.h> 
#include <string.h>

#include "mov_parse.h"


int main()
{	

    FILE *f = fopen("VID_001.MOV","rb");
	/* FILE *f = fopen("VID_002.MOV","rb"); */
    FILE *f_out = fopen("VIDEO.DAT","wb");
    int len;
    int i;
    unsigned char *buf = NULL;

    struct MY_MOV_CHUNK_T video_chunk = {0};

    int sec = 6;

    get_chunk_attr(f,&video_chunk);

    sort_vide_soun_chunk(&video_chunk,sec); 

    //准备帧buffer
    buf = malloc(1024*1024);
    for(i=0;i<((sec * video_chunk.vid_fps)+(sec * 2));i++) {
        if(video_chunk.aud_size && video_chunk.soun_stco_buf) {
            len = get_frame_by_chunk(f,&video_chunk,ntohl(video_chunk.frames_offset_buf[i]),buf);
        } else {
            len = get_frame_by_chunk(f,&video_chunk,ntohl(video_chunk.vide_stco_buf[i]),buf);
        }
        change_buf_data(buf);
        fwrite(buf,len,1,f_out);
    }
	
    if(buf){
        free(buf);
        buf = NULL;
    }

    realse_chunk_attr(&video_chunk);

	if(f){
		fclose(f);
	}
	if(f_out){
		fclose(f_out);
	}
	
	return 0;
}

