#ifndef JOS_KERN_E1000_H
#define JOS_KERN_E1000_H

#include <kern/pci.h>
#include <kern/pmap.h>


#define PCI_VENDORID_E1000 0x8086
#define PCI_DEVICEID_E1000 0x100E

volatile uint32_t *e1000_bar0;   // mmio vaddr.

int pci_e1000_attach(struct pci_func *pcif);

struct tx_desc{
    uint64_t   addr;   //buffer address
    uint16_t   lenth;
    uint8_t    cso;
    uint8_t    cmd;
    uint8_t    status;
    uint8_t    css;
    uint16_t   special;
};

#define NTD   32
extern struct tx_desc *tdba;  //tansmit discriptor base address

#define MAX_PKT_SIZE  1518
extern uint8_t *pkt_bufs;

#define TDBAL 0x3800
#define TDBAH 0x3804
#define TDLEN 0x3808
#define TDH   0x3810
#define TDT   0x3818
#define TCTL  0x0400
struct tctl{
    uint32_t  cntl_lo:4;
    uint32_t  ct:8;
    uint32_t  cold:10;
    uint32_t  cntl_hi:4;
    uint32_t  reserved:6;
};

#define TIPG  0x0410

// transmit function: transmit a packet
#define TD_STA_DD   0b0001
#define PKT_BUF_ADDR(tail)   (pkt_bufs + MAX_PKT_SIZE * tail)
int tx_pkt(uint8_t *buff, uint32_t len);



#endif	// JOS_KERN_E1000_H
