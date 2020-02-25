
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int printf_buf(void *buf,int len)
{
	int i = 0 ;
	unsigned char *tmp = (unsigned char*)buf;
	for(i=0;i<len;i++)
		printf("%x ",tmp[i]);
	printf("\n");
	return 0;
}

int main(int argc,char **argv)
{
    printf("===== argc %d\n",argc);
    for(int i=1;i<argc;i++)
        printf("=====[%d] argv %s\n",i,argv[i]);
    printf("hello\n");
    int flen = 0;
    int new_len = 0;
	unsigned char *src_buf = NULL;
	unsigned char *dst_buf = NULL;
    int i = 0;
    int j = 0;
    /* if(strcmp(argv[2],"-o")) { */
        /* printf("err argv 1, must -o\n"); */
        /* return 0; */
    /* } */
	FILE *f = fopen(argv[1],"rb");
	FILE *f_out = fopen("output_24.pcm","wb");
	/* FILE *f_out = fopen(argv[3],"wb"); */
    if(f){
        fseek(f, 0, SEEK_END);
        flen = ftell(f); 
        fseek(f, 0, SEEK_SET);
        src_buf = malloc(flen);
		fread(src_buf,flen,1,f);
        /* printf_buf(src_buf,flen); */
        printf("flen=%d\n",flen );
        new_len = flen / 4 * 3;
        dst_buf = malloc(new_len);
        for(i=0;i<new_len;i+=3){
            memcpy(&dst_buf[i],&src_buf[j],3);  
            j += 4;
        }

        fwrite(dst_buf,new_len,1,f_out);
        

        fclose(f);
        fclose(f_out);


    }
	return 0;
}
