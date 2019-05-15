#define FSSIZE  1000  //size of file system in blocks   (default block size:512Bybe)
                      // the reason that FSSIZE is here while not in new_fs.h is
                      // that new_fs.h just define the format of disk.
                      // Here, we define the disk itself.