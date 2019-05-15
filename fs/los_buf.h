#ifndef LOS_BUF
#define LOS_BUF

#include "los_fs.h"
#include "los_types.h"
#include "los_sleeplock.h"

struct buf{
    int flags;
    uint dev;
    uint blockno;
    struct sleeplock lock;  
    uint refcnt;
    struct buf *prev;  // LRU cache list    
    struct buf *next;  
    struct buf *qnext;   //disk queue
    uchar data[BSIZE];
};

#define B_VALID 0x2 // buffers has been read from disk
#define B_DIRTY 0x4 // buffers needs to be written to disk

#endif