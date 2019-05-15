/* file: new_fs.h
   Define or describe the layout in disk.
   In other words, the file system format is here.  
*/
#ifndef LOS_FS
#define LOS_FS

#include<sys/types.h>
#include<stdint.h>
//#include<inc/assert.h>

#define  ROOTINO     1 // root i-number
#define  BSIZE       512  // block size
#define  NDIRECT     12
#define  NINDIRECT   (BSIZE / sizeof(uint32_t))   // addrs type is uint32_t
#define  MAXFILE     (NDIRECT + NINDIRECT)   // measure in blocks
#define  MAXOPBLOCKS 10 //max # of blocks any FS op writes
#define  LOGSIZE     (MAXOPBLOCKS*3) // max data blocks in on-disk log
#define  NBUF        (MAXOPBLOCKS*3) // size of disk block cache 


struct superblock {
    uint32_t size;  //size of file system image (blocks)
    uint32_t nblocks; // number of data blocks
    uint32_t ninodes; // number of inodes
    uint32_t nlog;   //number of  log blocks
    uint32_t logstart; //block number  of first log block
    uint32_t inodestart; // block number of first inode block
    uint32_t bmapstart; // blokc number of first free map block
};

// on-disk inode structure   16B
struct dinode{
    short type; // file type
    short major; // major device number 
    short minor; 
    short nlink;  // number of links  to inode in file system;
    uint32_t size;  // size of file (bytes)
    uint32_t addrs[NDIRECT+1];  // data block address
};

// inodes per block
#define IPB (BSIZE / sizeof(struct dinode))

// inumber and superblock in,  the block that inode resides out. 
#define IBLOCK(i, sb) (sb.inodestart + (i)/IPB)

// bit map bits per block
#define BPB (BSIZE * 8)

// block of free map containing bit for block b
#define BBLOCK(b, sb) (sb.bmapstart + (b)/BPB)

// directory is a file  containing a sequence  of dirent structures.
#define DIRSIZ 14

struct dirent{
    u_short inum;    
    char name[DIRSIZ];
};


#define T_DIR  1   // Directory
#define T_FILE 2   // File
#define T_DEV  3   // Device

// avoid clash with host struct stat
#define stat  los_stat

struct stat {
  short type;  // Type of file
  int dev;     // File system's disk device
  uint ino;    // Inode number
  short nlink; // Number of links to file
  uint size;   // Size of file in bytes
};


#endif