#include<unistd.h>
#include<string.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include "new_fs_params.h"
#include "new_fs.h"

#define NINODES  200

//  the unit of variables below is block
int nbitmap = FSSIZE / BPB + 1;
int ninodeblocks = NINODES / IPB + 1;
int nlog = LOGSIZE;
int nmeta;     //number of meta blocks (boot ,sb, nlong, inode, bitmap) 
int nblocks;   //number of data blocks

int fsfd;
struct superblock sb;
char zeroes[BSIZE];
uint32_t freeinode = 1;   
uint32_t freeblock;

void balloc(int);
void wsect(uint32_t, void *);  
void winode(uint32_t, struct dinode*);
void rinode(uint32_t inum, struct dinode *ip);
void rsect(uint32_t sec, void *buf);
uint32_t ialloc(u_short type);
void iappend(uint inum, void *p, int n);

int main(int argc, char *argv[]){
    int i, cc, fd;
    uint32_t rootino, inum, off;
    struct dirent de;
    char buf[BSIZE];
    struct dinode din;  

    if(argc < 2){
        fprintf(stderr, "Usage: mkfs fs.img files...\n");
        exit(1);
    }
    
    assert((BSIZE % sizeof(struct dinode)) == 0);
    assert((BSIZE % sizeof(struct dirent)) == 0);

    fsfd = open(argv[1], O_RDWR|O_CREAT|O_TRUNC, 0666); 
    if(fsfd < 0){
        perror(argv[1]);
        exit(1);
    }

    nmeta = 2 + nlog + ninodeblocks + nbitmap;
    nblocks  = FSSIZE - nmeta;

    sb.size = FSSIZE;
    sb.nblocks = nblocks;
    sb.ninodes = NINODES;
    sb.nlog = nlog;
    sb.logstart = 2;
    sb.inodestart = 2 + nlog;
    sb.bmapstart = 2 + nlog + ninodeblocks;

    printf("nmeta %d (boot, super, log blocks %u inode blocks %u, bitmap blocks %u) blocks %d total %d\n",
     nmeta, nlog, ninodeblocks, nbitmap, nblocks, FSSIZE);

    freeblock = nmeta;  // the  first free block that we can  allocate

    // zero the whole img
    for(i = 0; i<FSSIZE; i++){
        wsect(i, zeroes);
    } 

    // write super block to block 1
    memset(buf, 0, sizeof(buf));
    memmove(buf, &sb, sizeof(sb));
    wsect(1, buf);

    // allocate the inum of root dir ( "/" )
    rootino = ialloc(T_DIR);

    // append "." to "/"
    bzero(&de, sizeof(de));
    de.inum = rootino;
    strcpy(de.name, ".");
    iappend(rootino, &de, sizeof(de));

    // append ".." to "/"
    bzero(&de, sizeof(de));
    de.inum = rootino;
    strcpy(de.name, "..");
    iappend(rootino, &de, sizeof(de));

    for(i = 2; i < argc; i++){
        //printf("file:%s, freeblock start: %u\n", argv[i], freeblock);
        assert(index(argv[i], '/') == NULL);

        if((fd = open(argv[i], O_RDONLY)) < 0){
            perror(argv[i]);
            exit(1);
        }

        if(argv[i][0] == '_'){
            ++argv[i];
        }

        inum = ialloc(T_FILE);

        bzero(&de, sizeof(de));
        de.inum = inum;
        strncpy(de.name, argv[i], DIRSIZ);
        iappend(rootino, &de, sizeof(de));

        while((cc = read(fd, buf, sizeof(buf))) > 0){
            iappend(inum, buf, cc);
        }

        close(fd);
    }    


    // round up dinode.size to BSIZE ALIGN
    rinode(rootino, &din);
    off = din.size;

    off = ((off/BSIZE)+1)*BSIZE;
    din.size = off;
    winode(rootino, &din);

    balloc(freeblock);

    exit(0);
}


void rsect(uint32_t sec, void *buf){
    if(lseek(fsfd, sec*BSIZE, SEEK_SET) != sec * BSIZE){
        perror("lseek");
        exit(1);
    }
    if(read(fsfd, buf, BSIZE)!=BSIZE){
        perror("read");
        exit(1);
    }
}

// write a block to img file
// arg 0  sec: sector number
// arg 1  buf: data that will be writen to img
void wsect(uint32_t sec, void *buf){
    if(lseek(fsfd,sec * BSIZE, SEEK_SET)!= sec * BSIZE){
        perror("lseek");
        exit(1);
    }
    if(write(fsfd, buf, BSIZE) != BSIZE){
        perror("write");
        exit(1);
    }
}

// global valuable (freeinode) records the current free inumber
//        freeinode has a initial val when it is defined
// art 0 type: file type, will be writen to inode
// return val: inode number to the inode
uint32_t ialloc(u_short type){
    uint32_t inum = freeinode++;
    struct dinode din;
    bzero(&din, sizeof(din));
    din.type = type;
    din.nlink = 1;
    din.size = 0;
    winode(inum, &din);
    return inum;
}

void rinode(uint32_t inum, struct dinode *p){
    uint32_t  iblock = IBLOCK(inum, sb);
    char buf[BSIZE];
    // if(read(fsfd, buf, BSIZE) != BSIZE){
    //     perror("read");
    //     exit(1);
    // }
    rsect(iblock, buf);
    memmove(p, (struct dinode *)buf + inum % IPB, sizeof(struct dinode));    
}

void winode(uint32_t inum, struct dinode *p){
    uint32_t  iblock = IBLOCK(inum, sb); 
    char  buf[BSIZE];
    
    rsect(iblock, buf);
    memmove((struct dinode *)buf + inum % IPB, p, sizeof(struct dinode));

    wsect(iblock, buf);
    // if(write(fsfd, buf, BSIZE) != BSIZE){
    //     perror("write");
    //     exit(1);
    // }
}




#define  min(x, y)  ({  \
                        typeof(x) a = x;     \
                        typeof(y) b = y;     \
                        x < y ? x : y;       \
                    })


#define  max(x, y)  ({  \
                        typeof(x) a = x;     \
                        typeof(y) b = y;     \
                        x > y ? x : y;       \
                    })

// append n bytes started at xp to file. inum -> inode ->addrs
// arg0  inum: inum of the file 
// arg1  xp:   buf pointer
// arg2  n:    size that will be writen

void iappend(uint32_t inum, void * xp, int n){
    char *p = (char*)xp;
    uint32_t    fbn, off, n1;
    struct dinode din;
    char buf[BSIZE];
    uint32_t  indirect[NINDIRECT];
    uint32_t  x;

    rinode(inum, &din);
    off = din.size;

    while(n>0){
        fbn = off / BSIZE;
        assert(fbn < MAXFILE);
        if(fbn < NDIRECT){
            if(din.addrs[fbn] == 0){
                din.addrs[fbn] = freeblock++;
            }
            x = din.addrs[fbn];
        } else{
            if(din.addrs[NDIRECT] == 0){
                din.addrs[NDIRECT] = freeblock++;
            }
            rsect(din.addrs[NDIRECT], (char *)indirect);
            if(indirect[fbn - NDIRECT] == 0){
                indirect[fbn - NDIRECT] = freeblock++;
                wsect(din.addrs[NDIRECT], indirect);
            }
            x = indirect[fbn - NDIRECT];
        }

        n1 = min(n, (fbn+1) * BSIZE - off);
        rsect(x, buf);
        bcopy(p, buf + off%BSIZE, n1);
        wsect(x, buf);
        n -= n1;
        off += n1;
        p += n1;
    }
    din.size = off;
    winode(inum, &din);
}


// mark 0 - (used-1) block as allocated and write to bitmap
// arg0 used: number of allocated blocks
void 
balloc(int used){
    u_char buf[BSIZE];
    int i;

    printf("balloc: first %d blocks have been allocated\n", used);
    assert(used < BSIZE*8);  // only one bitmap
    bzero(buf, BSIZE);
    for(i = 0; i < used; i++){
        buf[i/8] = buf[i/8] | (0x1 << (i%8));
    } 
    printf("balloc: write bitmap block at sector %d\n", sb.bmapstart);
    wsect(sb.bmapstart, buf);
}


