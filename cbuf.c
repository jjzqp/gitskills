#include "circular_buf.h"


#define  cb_memcpy memcpy

#if 0
#define CBUF_ENTER_CRITICAL() \
	spin_lock(&cbuffer->lock)


#define CBUF_EXIT_CRITICAL() \
	spin_unlock(&cbuffer->lock)

#else
#define CBUF_ENTER_CRITICAL()
#define CBUF_EXIT_CRITICAL() 

#endif

unsigned int cbuf_read(cbuffer_t *cbuffer, void *buf, unsigned int len)
{
    unsigned int flags;
    unsigned int r_len = len;
    unsigned int copy_len;
    unsigned char *read_ptr;
    unsigned int data_len = cbuffer->data_len;

    if (!cbuffer) {
        return 0;
    }

    if ((unsigned int)cbuffer->read_ptr >= (unsigned int)cbuffer->end) {
        cbuffer->read_ptr = (unsigned char *)cbuffer->begin;
    }

    if (data_len < len) {
        memset(buf, 0, len);
        return 0;
    }


    read_ptr = cbuffer->read_ptr;

    copy_len = (unsigned int)cbuffer->end - (unsigned int)read_ptr;
    if (copy_len > len) {
        copy_len = len;
    }
    len -= copy_len;


    cb_memcpy(buf, read_ptr, copy_len);

    if (len == 0) {
        read_ptr += copy_len;
    } else {
        cb_memcpy((unsigned char *)buf + copy_len, cbuffer->begin, len);
        read_ptr = cbuffer->begin + len;
    }

    CBUF_ENTER_CRITICAL();
    cbuffer->data_len -= r_len;
    cbuffer->tmp_len -= r_len;
    cbuffer->read_ptr = read_ptr;
    CBUF_EXIT_CRITICAL();

    return r_len;
}

/*********************************************************************************
  *Function: ?cbuf_prewrite
? *Description：cbuf预写函数
? *Param: 
? *Return:
? *Others: 写入数据不是立即有效,需调用cbuf_updata_prewrite 更新数据才有效
/*********************************************************************************/
unsigned int cbuf_prewrite(cbuffer_t *cbuffer, void *buf, unsigned int len)
{
    unsigned int flags;
    unsigned int length;
    unsigned int remain_len;
    unsigned char *tmp_ptr;
    unsigned int tmp_len = cbuffer->tmp_len;

    if (!cbuffer) {
        return 0;
    }

    if ((cbuffer->total_len - tmp_len) < len) {
        return 0;
    }

    tmp_ptr = cbuffer->tmp_ptr;
    length = (unsigned int)cbuffer->end - (unsigned int)tmp_ptr;
    if (length >= len) {
        cb_memcpy(tmp_ptr, buf, len);
        tmp_ptr += len;
    } else {
        remain_len = len - length;
        cb_memcpy(tmp_ptr, buf, length);
        cb_memcpy(cbuffer->begin, ((unsigned char *)buf) + length, remain_len);
        tmp_ptr = (unsigned char *)cbuffer->begin + remain_len;
    }

    CBUF_ENTER_CRITICAL();
    cbuffer->tmp_len += len;
    cbuffer->tmp_ptr = tmp_ptr;
    CBUF_EXIT_CRITICAL();

    return len;
}

/*********************************************************************************
  *Function: ?cbuf_updata_prewrite
? *Description：cbuf预写数据更新函数
? *Param: 
? *Return:
? *Others: 将cbuf_prewrite 预写的数据，更新生效
/*********************************************************************************/
void cbuf_updata_prewrite(cbuffer_t *cbuffer)
{
    unsigned int flags;

    CBUF_ENTER_CRITICAL();
    cbuffer->data_len = cbuffer->tmp_len;
    cbuffer->write_ptr = cbuffer->tmp_ptr;
    CBUF_EXIT_CRITICAL();

}

/*********************************************************************************
  *Function: ?cbuf_discard_prewrite
? *Description：cbuf预写数据取消函数
? *Param: 
? *Return:
? *Others: 将cbuf_prewrite 预写的数据取消
/*********************************************************************************/
void  cbuf_discard_prewrite(cbuffer_t *cbuffer)
{
    unsigned int flags;
    CBUF_ENTER_CRITICAL();
    cbuffer->tmp_len = cbuffer->data_len ;
    cbuffer->tmp_ptr = cbuffer->write_ptr ;
    CBUF_EXIT_CRITICAL();
}

/*********************************************************************************
  *Function: ?cbuf_write
? *Description：cbuf写函数
? *Param: 
? *Return:
? *Others: 写入数据将立即有效
/*********************************************************************************/
unsigned int cbuf_write(cbuffer_t *cbuffer, void *buf, unsigned int len)
{
    unsigned int flags;
    unsigned int length;
    unsigned int remain_len;

    if (!cbuffer) {
        return 0;
    }

    CBUF_ENTER_CRITICAL();

    if ((cbuffer->total_len - cbuffer->data_len) < len) {
        printf("cbuf write len no enough\n");
        CBUF_EXIT_CRITICAL();
        return 0;
    }

    length = (unsigned int)cbuffer->end - (unsigned int)cbuffer->write_ptr;
    if (length >= len) {
        cb_memcpy(cbuffer->write_ptr, buf, len);
        cbuffer->write_ptr += len;
    } else {
        remain_len = len - length;
        cb_memcpy(cbuffer->write_ptr, buf, length);
        cb_memcpy(cbuffer->begin, ((unsigned char *)buf) + length, remain_len);
        cbuffer->write_ptr = (unsigned char *)cbuffer->begin + remain_len;
    }

    cbuffer->data_len += len;
    cbuffer->tmp_len = cbuffer->data_len ;
    cbuffer->tmp_ptr = cbuffer->write_ptr ;
    CBUF_EXIT_CRITICAL();

    return len;
}

/*********************************************************************************
  *Function: cbuf_is_write_able?
? *Description：cbuf写函数
? *Param: 
? *Return: 可写的长度
? *Others: 判断是否能写入长度为len的数据
/*********************************************************************************/
unsigned int cbuf_is_write_able(cbuffer_t *cbuffer, unsigned int len)
{
    unsigned int w_len;

    if (!cbuffer) {
        return 0;
    }
    w_len = cbuffer->total_len - cbuffer->data_len;
    if (w_len < len) {
        return 0;
    }

    return w_len;
}

/****************************************************************************************************************
  *Function: cbuf_write_alloc?
? *Description：cbuf写申请函数
? *Param: 
? *Return: 可写指针位置 返回len为可写长度
? *Others: 调用此函数，则外部可以直接操作指针，往指针指向的地址写数据。调用后必须再调用cbuf_write_updata数据才更新生效
/***************************************************************************************************************/
void *cbuf_write_alloc(cbuffer_t *cbuffer, unsigned int *len)
{
    unsigned int data_len;
    unsigned int flags;

    if (!cbuffer) {
        return 0;
    }

    CBUF_ENTER_CRITICAL();

    *len = cbuffer->end - cbuffer->write_ptr;
    data_len = cbuffer->total_len - cbuffer->data_len;
    if (*len == 0) {
        cbuffer->write_ptr = cbuffer->begin;
        *len = data_len;
    }
    if (*len > data_len) {
        *len = data_len;
    }

    CBUF_EXIT_CRITICAL();

    return cbuffer->write_ptr;
}

/****************************************************************************************************************
  *Function: cbuf_write_updata?
? *Description：cbuf写申请后，数据更新函数
? *Param:  len 必须与cbuf_write_alloc 返回的len一致
? *Return: 
? *Others: 紧跟在cbuf_write_alloc调用之后 
/***************************************************************************************************************/
void cbuf_write_updata(cbuffer_t *cbuffer, unsigned int len)
{
    unsigned int flags;

    CBUF_ENTER_CRITICAL();
    cbuffer->tmp_ptr = cbuffer->write_ptr += len;
    cbuffer->tmp_len = cbuffer->data_len += len;
    CBUF_EXIT_CRITICAL();
}


/****************************************************************************************************************
  *Function: cbuf_read_alloc
? *Description：cbuf读申请函数
? *Param:  
? *Return: 可读指针位置 返回len为可读长度
? *Others: 调用此函数，则外部可以直接操作指针，读取指针指向的地址的数据。调用后必须再调用cbuf_read_updata读走的数据才能被更新
/***************************************************************************************************************/
void *cbuf_read_alloc(cbuffer_t *cbuffer, unsigned int *len)
{
    unsigned int flags;
    unsigned int data_len;

    if (!cbuffer) {
        return 0;
    }

    CBUF_ENTER_CRITICAL();

    if ((unsigned int)cbuffer->read_ptr >= (unsigned int)cbuffer->end) {
        cbuffer->read_ptr = (unsigned char *)cbuffer->begin;
    }

    data_len = cbuffer->data_len ;

    *len  = (unsigned int)cbuffer->end - (unsigned int)cbuffer->read_ptr;
    if (data_len <= *len) {
        *len = data_len;
    }

    CBUF_EXIT_CRITICAL();

    return cbuffer->read_ptr;
}

/****************************************************************************************************************
  *Function: cbuf_read_updata?
? *Description：cbuf读申请后，数据更新函数
? *Param:  len 必须与cbuf_read_alloc 返回的len一致
? *Return: 
? *Others: 紧跟在cbuf_read_alloc调用之后 
/***************************************************************************************************************/
void cbuf_read_updata(cbuffer_t *cbuffer, unsigned int len)
{
    unsigned int flags;

    CBUF_ENTER_CRITICAL();

    cbuffer->read_ptr += len;
    if ((unsigned int)cbuffer->read_ptr >= (unsigned int)cbuffer->end) {
        cbuffer->read_ptr = (unsigned char *)cbuffer->begin;
    }

    cbuffer->tmp_len -= len;
    cbuffer->data_len -= len;

    CBUF_EXIT_CRITICAL();
}


/****************************************************************************************************************
  *Function: cbuf_init?
? *Description：cbuf初始化函数
? *Param: buf为手动申请的内存,cbuffer为需要操作的cbuf结构体指针 
? *Return: 
? *Others: 
/***************************************************************************************************************/
void cbuf_init(cbuffer_t *cbuffer, void *buf, unsigned int size)
{
    cbuffer->data_len = 0;
    cbuffer->tmp_len = 0 ;
    cbuffer->begin = buf;
    cbuffer->read_ptr = buf;
    cbuffer->write_ptr = buf;
    cbuffer->tmp_ptr = buf;
    cbuffer->end = (unsigned char *)buf + size;
    cbuffer->total_len = size;
    /* spin_lock_init(&cbuffer->lock); */
}


/****************************************************************************************************************
  *Function: cbuf_clear?
? *Description：cbuf清空函数
? *Param: cbuffer为需要操作的cbuf结构体指针 
? *Return: 
? *Others: 
/***************************************************************************************************************/
void cbuf_clear(cbuffer_t *cbuffer)
{
    unsigned int flags;

    CBUF_ENTER_CRITICAL();

    cbuffer->read_ptr = cbuffer->begin;
    cbuffer->tmp_ptr = cbuffer->write_ptr = cbuffer->begin;
    cbuffer->data_len = 0;
    cbuffer->tmp_len = 0 ;

    CBUF_EXIT_CRITICAL();

}


unsigned int cbuf_rewrite(cbuffer_t *cbuffer, void *begin, void *buf, unsigned int len)
{
    unsigned int flags;
    unsigned int length;
    unsigned int remain_len;
    unsigned char *write_ptr = (unsigned char *)begin ;

    if (!cbuffer) {
        return 0;
    }

    length = (unsigned int)cbuffer->end - (unsigned int)write_ptr;
    if (length >= len) {
        cb_memcpy(cbuffer->write_ptr, buf, len);
        write_ptr += len;
    } else {
        remain_len = len - length;
        cb_memcpy(write_ptr, buf, length);
        cb_memcpy(cbuffer->begin, ((unsigned char *)buf) + length, remain_len);
    }


    return len;
}



void *cbuf_get_writeptr(cbuffer_t *cbuffer)
{
    if ((unsigned int)cbuffer->write_ptr >= (unsigned int)cbuffer->end) {
        cbuffer->write_ptr = (unsigned char *)cbuffer->begin;
    }

    return cbuffer->write_ptr;
}

unsigned int cbuf_get_data_size(cbuffer_t *cbuffer)
{
    return cbuffer->data_len;
}

void *cbuf_get_readptr(cbuffer_t *cbuffer)
{
    if ((unsigned int)cbuffer->read_ptr >= (unsigned int)cbuffer->end) {
        cbuffer->read_ptr = (unsigned char *)cbuffer->begin;
    }

    return cbuffer->read_ptr;
}

