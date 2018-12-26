#ifndef CIRCULAR_BUF_INTERFACE_H
#define CIRCULAR_BUF_INTERFACE_H

// #include "typedef.h"
// #include "system/spinlock.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>




typedef struct _cbuffer {
    unsigned char  *begin;
    unsigned char  *end;
    unsigned char  *read_ptr;
    unsigned char  *write_ptr;
    unsigned char  *tmp_ptr ;
    unsigned int tmp_len;
    unsigned int data_len;
    unsigned int total_len;
    // spinlock_t lock;
} cbuffer_t;


extern void cbuf_init(cbuffer_t *cbuffer, void *buf, unsigned int size);

extern unsigned int cbuf_read(cbuffer_t *cbuffer, void *buf, unsigned int len);

extern unsigned int cbuf_write(cbuffer_t *cbuffer, void *buf, unsigned int len);

extern unsigned int cbuf_is_write_able(cbuffer_t *cbuffer, unsigned int len);

extern void *cbuf_write_alloc(cbuffer_t *cbuffer, unsigned int *len);

extern void cbuf_write_updata(cbuffer_t *cbuffer, unsigned int len);

extern void *cbuf_read_alloc(cbuffer_t *cbuffer, unsigned int *len);

extern void cbuf_read_updata(cbuffer_t *cbuffer, unsigned int len);

extern void cbuf_clear(cbuffer_t *cbuffer);

extern unsigned int cbuf_rewrite(cbuffer_t *cbuffer, void *begin, void *buf, unsigned int len);

extern void  cbuf_discard_prewrite(cbuffer_t *cbuffer);

extern void cbuf_updata_prewrite(cbuffer_t *cbuffer);

extern unsigned int cbuf_prewrite(cbuffer_t *cbuffer, void *buf, unsigned int len);

extern void *cbuf_get_writeptr(cbuffer_t *cbuffer);

extern unsigned int cbuf_get_data_size(cbuffer_t *cbuffer);

extern void *cbuf_get_readptr(cbuffer_t *cbuffer);


#endif