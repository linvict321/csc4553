#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#define _POSIX_C_SOURCE 200809L


#define PAGE_ENTRIES 256
#define PAGE_SIZE 256
#define NUM_TLB_ENTRIES 256

/*typedef struct{
    int frame_number;
    int valid_invalid_bit; //0 = val, 1 = invalid
}PageTable;
*/
typedef struct{
    int page_num[NUM_TLB_ENTRIES]; //key
    int frame_num[NUM_TLB_ENTRIES]; //val
} TLB;

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
    char pra[];
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

    fclose(fp);

    printf("Number of Translated Addresses = %d", translated_addr);
    printf("Page Faults = %d", page_faults);
    printf("Page Fault Rate = %d", page_fault_rate);
    printf("TLB Hits = %d", TLB_hit);
    printf("TLB Misses = %d", TLB_miss);
    printf("TLB Hit Rate = %d", TLB_hitrate);

}