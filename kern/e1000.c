#include <kern/e1000.h>
#include <kern/pmap.h>
#include <inc/string.h>
#include <inc/ns.h>
// LAB 6: Your driver code here

struct tx_desc  *tdba     = NULL;
uint8_t         *pkt_bufs = NULL;

struct rx_desc  *rdba     = NULL;
uint8_t         *rx_bufs  = NULL;

static  void  init_tx();
static  void  init_rx();

// void read_nic_reg(uint32_t  offset){
//     cprintf("%u", e1000_bar0[offset>>2]);
// }

// In kern/pci.c, pci_attach_match() will search in a list for corresponding dev.
// Once matching, a function point in struct pci_driver will be called to initialize 
// the device.
// pci_e1000_attach is such a function.
// The corresponding driver structure is :
//      {PCI_VENDORID_E1000, PCI_DEVICEID_E1000, &pci_e1000_attach }

// char regs_tab[10] = {

// };


int
pci_e1000_attach(struct pci_func *pcif){
	pci_func_enable(pcif);
	// cprintf("e1000 reg_base[0]:%08x, reg_size[0]:%08x\n", pcif->reg_base[0], pcif->reg_size[0]);
	e1000_bar0 = mmio_map_region(pcif->reg_base[0], pcif->reg_size[0]);
        
        // Trivil debug info.

        // cprintf("e1000_bar0: %08x\n", (uint32_t)e1000_bar0);
        // int i=0;
        // for(i=0; i<20; i++){
        // 	cprintf("e1000 test register[i] val: %08x\n", e1000_bar0[i]);
        // }

    init_tx();
    init_rx();
	return 0;
}

static void  init_tx(){
    /***
        Following is transmit initialization
        Verbose refers to 8254x familly gigabit ethernet controllers software developer's manul
        chapter 14.5 and  chapter 13
    **/
    //initialize transmit descriptors
    //TDBAL etc is byte offset, while e1000_bar0 is pointer to uint32_t(4 byte)
    e1000_bar0[TDBAL>>2] = PADDR(tdba);
    //cprintf("TDBAL:%08x\n", e1000_bar0[TDBAL>>2]);
    e1000_bar0[TDBAH>>2] = 0;
    //cprintf("TDBAH:%08x\n", e1000_bar0[TDBAH>>2]);
    assert((NTD*sizeof(struct tx_desc) % 128) == 0);
    e1000_bar0[TDLEN>>2] = NTD*sizeof(struct tx_desc);
    
    //initialize head and tail to 0
    e1000_bar0[TDH>>2]   = 0;
    e1000_bar0[TDT>>2]   = 0;

    //initialize transmit control registers
    struct tctl init_tctl;
    *(uint32_t *)(&init_tctl) = 0;
    // set EN and PSP to 1b 
    init_tctl.cntl_lo = 0b1010;
    init_tctl.ct = 0x10;
    init_tctl.cold = 0x40;
    e1000_bar0[TCTL>>2] = *(uint32_t *)(&init_tctl);
    //cprintf("TCTL val: %08x\n", e1000_bar0[TCTL>>2]);
    // initialize transmit IPG
    e1000_bar0[TIPG>>2] = (6<<20) + (4<<10) + 10;
    //cprintf("debug e1000 test register[i] val: %08x\n", e1000_bar0[2]);

    //cprintf("debug e1000: B tx_pkt\n");
    // tx_pkt((uint8_t *)"Packet 00", 9);
    // tx_pkt((uint8_t *)"Packet 01", 9);
    // tx_pkt((uint8_t *)"Packet 02", 9);
    // tx_pkt((uint8_t *)"Packet 03", 9);
    // tx_pkt((uint8_t *)"Packet 04", 9);

    //cprintf("debug e1000: A tx_pkt, return val: %d\n", r);

}

static void init_rx(){
    // initialize rd ring
    int i=0;
    for(i=0; i < NRD; i++){
        rdba[i].addr = PADDR(rx_bufs + i*RX_BUF_SIZE);
    }


    e1000_bar0[RAL>>2] = 0x12005452;
    //cprintf("RAL:%08x\n", e1000_bar0[RAL>>2]);
    e1000_bar0[RAH>>2] = (0x00005634 | (1<<31));
    //cprintf("RAH:%08x\n", e1000_bar0[RAH>>2]);
    //memset((void*)&e1000_bar0[MTA>>2], 0, 512);
    e1000_bar0[MTA>>2] = 0;
    //cprintf("MTA:%08x\n", e1000_bar0[MTA>>2]);
    // struct ims  ims_val;
    // memset((void*)(&ims_val), 0, sizeof(struct ims));
    // ims_val.rxt0 = 1;
    // ims_val.rxo = 1;
    // ims_val.rxdmt0 = 1;
    // ims_val.rxseq = 1;
    // ims_val.lsc = 1;
    // e1000_bar0[IMS>>2] = *(uint32_t*)(&ims_val);


    //rd  ring
    e1000_bar0[RDBAL>>2] = PADDR(rdba);
    //cprintf("RDBAL:%08x\n", e1000_bar0[RDBAL>>2]);
    e1000_bar0[RDBAH>>2] = 0;
    //cprintf("RDBAH:%08x\n", e1000_bar0[RDBAH>>2]);
    e1000_bar0[RDLEN>>2]    = NRD * sizeof(struct rx_desc);
    //cprintf("RDLEN:%08x\n", e1000_bar0[RDLEN>>2]);
    e1000_bar0[RDH>>2]      = 0;
    //cprintf("RDH:%08x\n", e1000_bar0[RDH>>2]);
    e1000_bar0[RDT>>2]      = NRD-1;
    //cprintf("RDT:%08x\n", e1000_bar0[RDT>>2]);

    struct rctl  rctl_val;
    *(uint32_t *)(&rctl_val) = 0;
    rctl_val.en = 1;

    // buf size = 2048B
    rctl_val.bsex = 0;
    rctl_val.bsize = 0b00;
    
    rctl_val.secrc = 1;
    
    rctl_val.bam = 1;
    rctl_val.lbm = 0b11;
    e1000_bar0[RCTL>>2] = *(uint32_t*)(&rctl_val);
    //cprintf("RCTL val: %08x\n", e1000_bar0[RCTL>>2]);
}

// transmit function: transmit a packet
// arg0: pointer to packet
// arg1: size of packet in bytes
// return:  0  on success 
//          -1 indicates next td is not free
int tx_pkt(uint8_t *buff, uint32_t len){
    int i = 0;
    //cprintf("buff addr:%08x\n", (uint32_t)buff);
    //cprintf("buff len:%u\n", len);
    //cprintf("start:");
    // for(i=0;i<len;i++){
    //     cprintf("%c", ((char*)buff)[i]);
    // }
    // cprintf("\n");
    assert(len<MAX_PKT_SIZE);
    uint32_t tail = e1000_bar0[TDT>>2];
    if(tdba[tail].addr == 0 || (tdba[tail].status & TD_STA_DD)){
        uint8_t *dst = PKT_BUF_ADDR(tail);
        memmove(dst, buff, len);

        // fill tranmit descriptor
        tdba[tail].addr = PADDR(dst);
        tdba[tail].lenth = len;
        tdba[tail].cso = 0;
        tdba[tail].cmd |= 0b1001;    // RS & EOP   3.3 Packet Transmission

      
        e1000_bar0[TDT>>2] = (e1000_bar0[TDT>>2] + 1) % NTD;
        return 0;
    }else 
        return -1;
}


/*
    arg0: the addr where expect to receive a packet
    arg0: len of the buff  (now  len must less than or equal to rd.lenth)
    return val: 0 on success
                -1 on failure
*/
int rx_pkt(struct jif_pkt *pkt){
    uint32_t tail = (e1000_bar0[RDT>>2] + 1) % NRD;
    //cprintf("tail:%08x\n", tail);
    //cprintf("rdba[tail].status: %08x\n", rdba[tail].status);
    if(rdba[tail].status & 0b1){
        memmove((void*)(pkt->jp_data), KADDR((*(uint32_t *)&(rdba[tail].addr))), rdba[tail].lenth);
        pkt->jp_len = rdba[tail].lenth;
        rdba[tail].status &= ~0b1;
        e1000_bar0[RDT>>2] = (e1000_bar0[RDT>>2] + 1)%NRD ;
        return 0;
    }
    return -1;
}