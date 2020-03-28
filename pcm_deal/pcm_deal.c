#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

struct my_fun_t{
    const char *desc; 
    int (*func)(void);
};

static int __printf_buf(void *buf,int len)
{
	int i = 0 ;
	unsigned char *tmp = (unsigned char*)buf;
	for(i=0;i<len;i++)
		printf("%x ",tmp[i]);
	printf("\n");
	return 0;
}

static int __file_len(FILE *f)
{
    int flen = 0;
    if(f){
        fseek(f, 0, SEEK_END);
        flen = ftell(f); 
        fseek(f, 0, SEEK_SET);
    }
    return flen;
}
static void __find_pcm(char *name)
{
    struct dirent *entry = NULL; 
    DIR *cur_dir = opendir(".");
    int name_len;
    if(cur_dir){
       while(entry = readdir(cur_dir)) { 
           name_len = strlen(entry->d_name);
            if(!strncmp(&entry->d_name[name_len-3],"pcm",3)||!strncmp(&entry->d_name[name_len-3],"PCM",3)) {
                printf("find %s %d\n",entry->d_name ,name_len);
                strcpy(name,entry->d_name);
                goto __end;
            }
       }
    }
    printf("can not find pcm file!! \n");
__end:
    closedir(cur_dir);
}

/*
 * pcm 数据处理:
 * 32 bit 数据转24bit 
 */
static int pcm_32_24(void)
{ 
    printf("%s\n",__FUNCTION__ );
    char name_path[64];
    char dst_name_path[128];
    char tmp_name[64];
    /* printf("Please Input file name(32bit):"); */
    /* scanf("%s",name_path); */
    __find_pcm(name_path);
    /* printf("%s\n",name_path); */
	FILE *f = fopen(name_path,"rb");
    if(!f){
        printf("找不到 %s \n",name_path );
        return -1;
    }
    strcpy(tmp_name,name_path);
    memset(&tmp_name[strlen(tmp_name)-4],0,4);
    sprintf(dst_name_path,"OutPut\\%s_24.pcm",tmp_name);
	FILE *f_out = fopen(dst_name_path,"wb");;
	unsigned char *src_buf = NULL;
	unsigned char *dst_buf = NULL;
    int i = 0;
    int j = 0;
    int flen = 0;
    int new_len = 0;
    if(f) {
        //读文件长度
        flen = __file_len(f); 
        src_buf = malloc(flen);
        //读文件内容
		fread(src_buf,flen,1,f);
        /* printf("flen=%d\n",flen ); */
        //计算32bit所需长度
        new_len = flen / 4 * 3;
        dst_buf = malloc(new_len);
        //转换
        for(i=0;i<new_len;i+=3){
            memcpy(&dst_buf[i],&src_buf[j],3);  
            j += 4;
        }

        fwrite(dst_buf,new_len,1,f_out);

        free(src_buf);
        free(dst_buf);
        fclose(f);
        remove(name_path); 
        fclose(f_out);
    }
    printf(">>>> ok >> %s\n",dst_name_path);
    return 0;
}
/*
 * pcm 数据处理:
 * 24 bit 数据转32bit 
 */
static int pcm_24_32(void)
{ 
    printf("%s\n",__FUNCTION__ );
    char name_path[64];
    char dst_name_path[128];
    char tmp_name[64];
    /* printf("Please Input file name(24bit):"); */
    /* scanf("%s",name_path); */
    __find_pcm(name_path);
    /* printf("%s\n",name_path); */
	FILE *f = fopen(name_path,"rb");
    if(!f){
        printf("找不到 %s \n",name_path );
        return -1;
    }
    strcpy(tmp_name,name_path);
    memset(&tmp_name[strlen(tmp_name)-4],0,4);
    sprintf(dst_name_path,"OutPut\\%s_32.pcm",tmp_name);
	FILE *f_out = fopen(dst_name_path,"wb");;
	unsigned char *src_buf = NULL;
	unsigned char *dst_buf = NULL;
    int err = 0;
    int i = 0;
    int j = 0;
    int flen = 0;
    int new_len = 0;
    if(f) {
        //读文件长度
        flen = __file_len(f); 
        src_buf = malloc(flen);
        //读文件内容
		fread(src_buf,flen,1,f);
        /* printf("flen=%d\n",flen ); */
        //计算24bit所需长度
        new_len = flen / 3 * 4;
        dst_buf = malloc(new_len);
        //转换
        for(i=0;i<new_len;i+=4){
            memcpy(&dst_buf[i],&src_buf[j],3);  
            j += 3;
        }

        fwrite(dst_buf,new_len,1,f_out);

        free(src_buf);
        free(dst_buf);
        fclose(f);
    printf("remove %s\n",name_path);
        err = remove(name_path); 
        printf(">>>>>err=%d\n",err);
        if(err){
            perror("remove");
        }
        fclose(f_out);
    }
    printf(">>>> ok >> %s\n",dst_name_path);
    return 0;
}


/*
 * pcm 数据处理:
 * pcm 数据分左右声道 
 */
static int pcm_split(void)
{ 
    printf("%s\n",__FUNCTION__ );
    int data_bit = 0;
    char name_path[64];
    char dst_name_path[128];
    char tmp_name[64];
    printf("Please Input (24/32):");
    scanf("%d",&data_bit);
    int each_bits = data_bit / 8;
    __find_pcm(name_path);
    /* printf("%s\n",name_path); */
	FILE *f = fopen(name_path,"rb");
    if(!f){
        printf("找不到 %s \n",name_path );
        return -1;
    }
	FILE *f_l_out = NULL;
	FILE *f_r_out = NULL;
	unsigned char *src_buf = NULL;
    int err = 0;
    int i = 0;
    int j = 0;
    int k = 0;
    int flen = 0;
    //读文件长度
    flen = __file_len(f); 
    src_buf = malloc(flen);
    //读文件内容
    fread(src_buf,flen,1,f);

    unsigned char *left_buf = malloc(flen/2);
    unsigned char *right_buf = malloc(flen/2);
    //分离
    for(i=0;i<flen;i+=(each_bits*2)) {
        //L
        memcpy(&left_buf[j],&src_buf[i],each_bits);
        j+=each_bits;
        //R
        memcpy(&right_buf[k],&src_buf[i+each_bits],each_bits);
        k+=each_bits;
    }
     
    strcpy(tmp_name,name_path);
    memset(&tmp_name[strlen(tmp_name)-4],0,4);
    sprintf(dst_name_path,"OutPut\\%s_l.pcm",tmp_name);
	f_l_out = fopen(dst_name_path,"wb");;
    fwrite(left_buf, flen/2,1,f_l_out);
    printf(">>>> ok >> %s\n",dst_name_path);
    fclose(f_l_out);

    strcpy(tmp_name,name_path);
    memset(&tmp_name[strlen(tmp_name)-4],0,4);
    sprintf(dst_name_path,"OutPut\\%s_r.pcm",tmp_name);
	f_r_out = fopen(dst_name_path,"wb");;
    fwrite(right_buf,flen/2,1,f_r_out);
    printf(">>>> ok >> %s\n",dst_name_path);
    fclose(f_r_out);

    free(src_buf);
    free(left_buf);
    free(right_buf);
    fclose(f);
    err = remove(name_path); 
    if(err){
        perror("remove");
    }
    return 0;
}


/*
 * pcm 数据处理:
 * pcm 32 bit数据小端转大端 (数据处理需要:如音量调节,计算分贝数) 
 */
static int pcm_little_big(void)
{ 
    printf("%s\n",__FUNCTION__ );
    char name_path[64];
    char dst_name_path[128];
    char tmp_name[64];
    __find_pcm(name_path);
    /* printf("%s\n",name_path); */
	FILE *f = fopen(name_path,"rb");
    if(!f){
        printf("找不到 %s \n",name_path );
        return -1;
    }
    strcpy(tmp_name,name_path);
    memset(&tmp_name[strlen(tmp_name)-4],0,4);
    sprintf(dst_name_path,"OutPut\\%s_big.pcm",tmp_name);
	FILE *f_out = fopen(dst_name_path,"wb");;
	unsigned char *buf = NULL;
    unsigned char tmp[4];
    int err = 0;
    int i = 0;
    int flen = 0;
    memset(tmp,0,sizeof(tmp));
    //读文件长度
    flen = __file_len(f); 
    buf = malloc(flen);
    //读文件内容
    fread(buf,flen,1,f);
    /* printf("flen=%d\n",flen ); */
    //计算
    for(i=0;i<flen;i+=4) {
        tmp[1] = buf[i];
        tmp[2] = buf[i+1];
        tmp[3] = buf[i+2];
        memcpy(&buf[i],tmp,4);
    }
    fwrite(buf,flen,1,f_out);

    free(buf);
    fclose(f);
    err = remove(name_path); 
    printf(">>>>>err=%d\n",err);
    if(err){
        perror("remove");
    }
    fclose(f_out);
    printf(">>>> ok >> %s\n",dst_name_path);

    return 0;
}

/********** pcm相关处理函数集 **********/
static struct my_fun_t my_func_test[] = {
    {"pcm 32 to 24bit Test",pcm_32_24},
    {"pcm 24 to 32bit Test",pcm_24_32},
    {"pcm split left&right",pcm_split},
    {"pcm 32 bit little to big",pcm_little_big},
}; 
int main(int argc,char **argv)
{
    int c;
    int i;
    setbuf(stdout,NULL);  //不带缓存printf,实时输出
    printf("\n=========================\n");
    printf("hello...\n");
    for(i=0;i<sizeof(my_func_test)/sizeof(my_func_test[0]);i++) {
        printf("%d. ",i+1);
        printf("%s \n",my_func_test[i].desc);
    }
    printf("=========================\n");
    printf("Please Input Number:");
    scanf("%d",&c);
    printf(">>%d\n",c);
    my_func_test[c-1].func();

    return 0;
}
