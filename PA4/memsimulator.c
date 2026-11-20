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


//TODO: I actually don't know if we need a TLB Entry or if I can just add it straight into the TLB
//An entry in the TLB consist of a logical memory page number and its corresponding physical memory frame number, 16 entry TLB 
typedef struct{ //TLB has to be fifo
    int page_num[PAGE_SIZE]; //key
    int frame_num; //val
    struct TLBEntry* next;
}TLBEntry;

typedef struct{ //linkedlist for fifo
    TLBEntry * front;
    TLBEntry * rear;    
    int current_size;
    int max_capacity; //set this as 16
}TLB;

bool TLBHIT = false; //false = miss, true = hit

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
    int frames = 256;
    char *pra; //algo
    pra = "fifo";
    if(argc == 3){
        frames = atoi(argv[1]); //frame number
        if(frames <= 0 && frames > 256){
            printf("Frame number not betweeen 0 to 256");
            return 1;
        }

        pra = argv[2]; //check if its fifo, lru, opt
        if(strcmp(pra, "fifo") == 0 || strcmp(pra, "opt") == 0 || strcmp(pra, "lru") == 0){
            printf("Not fifo, opt, or lru");
            return 1;
        }
    } 

    int translated_addr = 0;
    int page_faults = 0;
    double page_fault_rate = page_faults/translated_addr; //TODO: it's page faults/memory references
    int TLB_hit = 0;
    int TLB_miss = 0;
    double TLB_hitrate = TLB_hit/(TLB_hit + TLB_miss);

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

        //TODO: save page_num and page_offset somewhere maybe in a list and then run it thorugh the TLB hit/miss
        //idk if that would be the right implementation or to just directly implement the TLB hit or miss w/in this read loop directly
        int page_num = get_pagenum(address); 
        int page_offset = get_pageoffset(address); //offset is the same in the physical address

        translated_addr += 1; 
    }

    //check if hit or miss on TLB
    int i = 0;
    currtlb = tlb->front;
    while(i < tlb->current_size)
    {
        //if hit, frame number becomes available*from TLB) and used to access memory
        if(page_num == tlb->curr){
            TLB_hit += 1;
        }            
        i++;
    }
    //else miss, memory reference to page table must be made
    if(!TLBHIT){
        TLB_miss += 1;
        //reference to page table

        //add entry to tlb
        TLBEntry *tlbe = (*TLBEntry)malloc(sizeof(*tlbe));
        TLBEntry temp = tlb->rear;
        //TODO: fill in all the blanks

        PageTableEntry *pte = (*PageTableEntry)malloc(sizeof(*pte));
    }

    PageTableEntry *pte = (*PageTableEntry)malloc(sizeof(*pte));

    //read from logical addresses, translate to physical using a TLB and page table
    //extract the page number from logical address
    //see if its a hit or miss on tlb
    //based on this, add or remove from the page table


    //if not in page table, we have to do a 256 byte page recall from BackingStore.bin 
    //diff page replacement algos

    //treat backingstore as a random access table, use fopen, fread, fseek, fclose
    FILE *bin = fopen("BACKING_STORE.bin", "r");
    if(bin == NULL){
        printf("Couldn't open BACKING_STORE.bin file");
        return 1;
    }

    if(strcmp(pra, "fifo")){
        //replace oldest page w/ counter? or create a queue.h w/ queue.c files
        
    } else if(strcmp(pra, "opt")){
        //TODO: Do we have to do this one??
    } else if(strcmp(pra, "lru")){
        //replace the one that was least recently ues (implement stack)
    }

    //fseek based on frame number
    //then fread it to be the current page table
    //store in page frame, update page table and TLB

    fclose(bin);
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