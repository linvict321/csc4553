#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdboolh>
#define _POSIX_C_SOURCE 200809L


#define PAGE_ENTRIES 256
#define PAGE_SIZE 256
#define NUM_TLB_ENTRIES 256
*typedef struct{
    int frame_number;
    bool valid;
    bool dirty; //1 = modified, 0 = unmodified


}PageTable;

//An entry in the TLB consist of a logical memory page number and its corresponding physical memory frame number, 16 entry TLB 
typedef struct{
    int page_num[16]; //key
    int frame_num[16]; //val
} TLB;
bool TLBHIT = false; //false = miss, true = hit

//TODO: write a program that extracts the page number and offset
void extract(void){
    /*easiest way to do this is by using the operators for bit-masking and
    bit-shifting. Once you can correctly establish the page number and offset from an
    integer number, you are ready to begin*/
}

int main(int argc, char * argv[]){

    if(argc != 1){
        printf("Missing input statement");
        return 1;
    }

    //open addresses.txt
    FILE *fp = fopen(argv[0], "r");
    if(fp == NULL){
        printf("File cannot be opened");
        return 1;
    }
    //default 256 and FIFO
    int frames = 0;
    char pra[]; //algo
    if(argc == 3){
        frames = atoi(argv[1]); //frame number
        if(frames <= 0 && frames > 256){
            printf("Frame number not betweeen 0 to 256");
            return 1;
        }

        *pra = argv[2]; //check if its fifo, lru, opt
        if(strcmp(pra, "fifo") == 0 || strcmp(pra, "opt") == 0 || strcmp(pra, "lru") == 0){
            printf("Not fifo, opt, or lru");
            return 1;
        }
    } else{
        frames = 256;
        pra[] = "fifo";
    }

    int translated_addr = 0;
    int page_faults = 0;
    int page_fault_rate = 0;
    int TLB_hit = 0;
    int TLB_miss = 0;
    int TLB_hitrate = TLB_hit/(TLB_hit + TLB_miss);

    //translate logical ->physical w/ TLB and page table
    int page_table[PAGE_ENTRIES];
    char *buf = NULL;
    size_t len = 0;
    ssize_t read;
    while((read = getline(&buf, &len, fp)) != -1)}{

    }


    //read from logical addresses, translate to physical using a TLB and page table
    //extract the page number from logical address
    //see if its a hit or miss on tlb
    //based on this, add or remove from the page table
    //if not in page table, we have to do a 256 byte page recall from BackingStore.bin 
    //treat backingstore as a random access table, use fopen, fread, fseek, fclose

    //store in page frame, update page table and TLB


    fclose(fp);

    printf("Number of Translated Addresses = %d", translated_addr);
    printf("Page Faults = %d", page_faults);
    printf("Page Fault Rate = %d", page_fault_rate);
    printf("TLB Hits = %d", TLB_hit);
    printf("TLB Misses = %d", TLB_miss);
    printf("TLB Hit Rate = %d", TLB_hitrate);

}