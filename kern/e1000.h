#ifndef JOS_KERN_E1000_H
#define JOS_KERN_E1000_H

#include <kern/pci.h>
#include <kern/pmap.h>
#include <inc/ns.h>

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

// rx
int rx_pkt(struct jif_pkt *pkt);

// definitions for receive

struct rx_desc{
    uint64_t   addr;   //buffer address
    uint16_t   lenth;
    uint16_t   pkt_cksum;
    uint8_t    status;
    uint8_t    errs;
    uint16_t   special;
};

#define NRD   128
extern struct rx_desc *rdba;  //receive discriptor base address

#define RX_BUF_SIZE  2048
extern uint8_t *rx_bufs;

#define RAL  0x5400      // + 8*n    0<=n<16
#define RAH  0x5404        
#define MTA  0x5200      //[127:0]
#define RDBAL 0x2800
#define RDBAH 0x2804 
#define RDLEN 0x2808
#define RDH   0x2810
#define RDT   0x2818
#define RCTL  0x0100
#define IMS   0x00D0

struct ims{
    uint32_t txdw:1;
    uint32_t txqe:1;
    uint32_t lsc:1;
    uint32_t rxseq:1;
    uint32_t rxdmt0:1;
    uint32_t rsd0:1;
    uint32_t rxo:1;
    uint32_t rxt0:1;
    uint32_t rsd1:1;
    uint32_t mdac:1;
    uint32_t rxcfg:1;
    uint32_t rsd2:1;
    uint32_t phyint:1;
    uint32_t gpi:2;
    uint32_t txd_low:1;
    uint32_t srpd:1;
    uint32_t rsd3:15;
};


//13.4.22
struct rctl{
    uint32_t   rsd0:1;
    uint32_t   en:1;
    uint32_t   sbp:1;
    uint32_t   upe:1;
    uint32_t   mpe:1;
    uint32_t   lpe:1;
    uint32_t   lbm:2;
    uint32_t   rdmts:2;
    uint32_t   rsd1:2;
    uint32_t   mo:2;
    uint32_t   rsd2:1;
    uint32_t   bam:1;
    uint32_t   bsize:2;
    uint32_t   vfe:1;
    uint32_t   cfien:1;
    uint32_t   cfi:1;
    uint32_t   rsd3:1;
    uint32_t   dpf:1;
    uint32_t   pmcf:1;
    uint32_t   rsd4:1;
    uint32_t   bsex:1;
    uint32_t   secrc:1;
    uint32_t   rsd5:5;
};

#endif	// JOS_KERN_E1000_H
