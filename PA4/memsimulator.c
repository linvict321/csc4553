#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdboolh>
#define _POSIX_C_SOURCE 200809L

#define PAGE_ENTRIES 256
#define PAGE_SIZE 256
#define NUM_TLB_ENTRIES 16
typedef struct{
    int frame_number;
    bool valid; //valid/invalid bit
    bool dirty; //1 = modified, 0 = unmodified
}PageTableEntry;
typedef struct{
    PageTableEntry entries[PAGE_ENTRIES];
}PageTable;
typedef struct{
    PageTable *tables[PAGE_SIZE];
}PageDir;

//An entry in the TLB consist of a logical memory page number and its corresponding physical memory frame number, 16 entry TLB 
typedef struct{ //TLB has to be fifo
    int page_num[PAGE_SIZE]; //key
    int frame_num; //val
    struct TLBEntry* next;
}TLBEntry;

//TODO: 16 entries
typedef struct{ //linkedlist for fifo
    TLBEntry * front;
    TLBEntry * rear;    
    int current_size;
    int max_capacity;
}TLB;

//bool TLBHIT = false; //false = miss, true = hit

void enqueue_TLB(TLB *t, int page_num, int frame_num){
    TLBEntry *newT = (TLBEntry *)malloc(sizeof(TLBEntry));
    if(!newT){
        printf("Queue overflow");
        return;
    }
    //queue empty
    if(t->rear == NULL){
        t->front = t->rear = newT;
        return;
    }
    t->rear->next = newT;
    t->rear = newT;
    t->current_size += 1;
}
void dequeue(TLB *t){
    if(t->front == NULL){
        printf("Queue Empty");
        return;
    }
    TLBEntry *temp = t->front;
    t->front = t->front->next;
    if(t->front == NULL){
        t->rear = NULL;
    }

    t->current_size -= 1;
    free(temp);
}


//TODO: write a program that extracts the page number and offset
int16_t get_pagenum(int32_t num){
    /*easiest way to do this is by using the operators for bit-masking and
    bit-shifting. Once you can correctly establish the page number and offset from an
    integer number, you are ready to begin*/
    int16_t page_num = (num >> 8) & 0xFF; // 8-15 bits
    return page_num;
}
int16_t get_pageoffset(int32_t num){
    int16_t offset = num & 0xFF; //last 8 bits
    return offset;
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

    //create page table and page dir
    PageDir *pd = (*PageDir)malloc(sizeof(*pd));
    if(!pd){
        printf("Page Directory failed");
        return NULL;
    }
    PageTable *pt = (*PageTable)malloc(sizeof(*pt));
    if(!pt){
        printf("Page table failed");
        return NULL;
    }
    TLB *tlb = (*TLB)malloc(sizeof(*tlb));
    if(!tlb){
        printf("TLB failed");
        return NULL;
    }
    tlb->max_capacity = 16;

    //translate logical ->physical w/ TLB and page table
    int page_table[PAGE_ENTRIES];
    char *buf = NULL;
    size_t len = 0;
    ssize_t read;
    while((read = getline(&buf, &len, fp)) != -1)}{
        //extract the buffer and put it into the extraction function
        int address = atoi(buf);
        if(!address){
            printf("Invalid address");
            return 1;
        }
        int page_num = get_pagenum(address);
        int page_offset = get_pageoffset(address);

        //check if hit or miss on TLB
        while(int i != 15)
        {
            /* code */
        }
        

        PageTableEntry *pte = (*PageTableEntry)malloc(sizeof(*pte));
    }

    //read from logical addresses, translate to physical using a TLB and page table
    //extract the page number from logical address
    //see if its a hit or miss on tlb
    //based on this, add or remove from the page table
    //if not in page table, we have to do a 256 byte page recall from BackingStore.bin 
    //treat backingstore as a random access table, use fopen, fread, fseek, fclose

    //store in page frame, update page table and TLB

    fclose(fp);
    free(pd);
    free(pt);
    free(tlb);
    
    printf("Number of Translated Addresses = %d", translated_addr);
    printf("Page Faults = %d", page_faults);
    printf("Page Fault Rate = %d", page_fault_rate);
    printf("TLB Hits = %d", TLB_hit);
    printf("TLB Misses = %d", TLB_miss);
    printf("TLB Hit Rate = %d", TLB_hitrate);

}