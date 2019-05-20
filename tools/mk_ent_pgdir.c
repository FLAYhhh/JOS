#include<stdio.h>
#include<stdlib.h>
#include<error.h>

int main(){
    FILE * fp = fopen("entrypgdir.c", "w");
    if (fp == NULL){
        perror("fopen");
        exit(1);
    }

    int wn = 0;
    fprintf(fp,"#include <inc/mmu.h>                 \n"
               "#include <inc/memlayout.h>           \n"
                                                          
                "pte_t entry_pgtable[NPTENTRIES];     \n"   
 
                "__attribute__((__aligned__(PGSIZE))) \n   "
                "pde_t entry_pgdir[NPDENTRIES] = {    \n   "
                "\t  [0]= ((uintptr_t)entry_pgtable - KERNBASE) + PTE_P, \n  "
                "\t  [KERNBASE>>PDXSHIFT]= ((uintptr_t)entry_pgtable - KERNBASE) + PTE_P + PTE_W \n  "
                "};  \n  "
                "__attribute__((__aligned__(PGSIZE))) \n "
                "pte_t entry_pgtable[NPTENTRIES] = { \n   "
                );  
    
    int i; int pgaddr = 0;
    for(i=0;i<1024;i++){
        fprintf(fp, "\t0x%06x | PTE_P | PTE_W,\n", pgaddr);
        pgaddr += 0x1000;
    }
    
    
    fprintf(fp, "};\n");

    
}