#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>

#define PAGE_ENTRIES 256
#define PAGE_SIZE 256
#define NUM_TLB_ENTRIES 16
#define MAX_FRAMES 256
typedef struct{
    int frame_number;
    bool valid; //valid/invalid bit
}PageTableEntry;
typedef struct{
    PageTableEntry entries[PAGE_ENTRIES];
}PageTable;

//TODO: I actually don't know if we need a TLB Entry or if I can just add it straight into the TLB
//An entry in the TLB consist of a logical memory page number and its corresponding physical memory frame number, 16 entry TLB 
typedef struct TLBEntry{ //TLB has to be fifo
    int page_num; //key
    int frame_num; //val
    bool valid; 
    struct TLBEntry *next;
}TLBEntry;

typedef struct{ //linkedlist for fifo
    TLBEntry * front;
    TLBEntry * rear;    
    int current_size;
    int max_capacity; //set this as 16
}TLB;

typedef struct{
    unsigned char *data; //data w/in each frame
    int num_frames; //curr frame number
    int num_used; //num of frames used
    int frame_page[MAX_FRAMES];
    int last_used[MAX_FRAMES]; //lru purposes
    int fifo_next;
}PhysMem;

bool TLBHIT = false; //false = miss, true = hit

void tlb_create(TLB *t){
    t->front = NULL;
    t->rear = NULL;
    t->current_size = 0;
    t->max_capacity = NUM_TLB_ENTRIES;
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
void enqueue_TLB(TLB *t, int page_num, int frame_num){
    if(t->current_size == t->max_capacity){
        dequeue(t);
    }
    TLBEntry *newT = (TLBEntry *)malloc(sizeof(TLBEntry));
    if(!newT){
        printf("Queue overflow\n");
        exit(1);
    }
    newT->page_num = page_num;
    newT->frame_num = frame_num;
    newT->valid = true;
    newT->next = NULL;
    //queue empty
    if(t->rear == NULL){
        t->front = t->rear = newT;
    } else{
        t->rear->next = newT;
        t->rear = newT;
    }
    t->current_size += 1;
}

/*easiest way to do this is by using the operators for bit-masking and bit-shifting. 
Once you can correctly establish the page number and offset from an integer number, you are ready to begin*/
int get_pagenum(int32_t num){
    int page_num = (num >> 8) & 0xFF; // 8-15 bits
    return page_num;
}
int get_pageoffset(int32_t num){
    int offset = num & 0xFF; //last 8 bits
    return offset;
}

int main(int argc, char * argv[]){
    if(argc < 2 || argc > 4){
        printf("WRONG input statement");
        return 1;
    }

    //open addresses.txt
    FILE *fp = fopen(argv[1], "rb");
    if(fp == NULL){
        printf("File cannot be opened");
        return 1;
    }
    //default 256 and FIFO
    int frames = 256;
    char *pra; //algo
    pra = "fifo";

    //create physical mem
    PhysMem *pm = (PhysMem *)calloc(1, sizeof(*pm));
    if(!pm){
        printf("Physical Memory failed");
        return 1;
    }

    if(argc >= 3){
        frames = atoi(argv[2]); //frame number
        if(frames <= 0 || frames > 256){
            printf("Frame number not betweeen 0 to 256");
            return 1;
        }
    } 
    if( argc == 4){
        pra = argv[3]; //check if its fifo, lru, opt
        if(strcmp(pra, "fifo") != 0 && strcmp(pra, "opt") != 0 && strcmp(pra, "lru") != 0){
            printf("Not fifo, opt, or lru");
            return 1;
        }
    } 
    pm->num_frames = frames;
    pm->data = (unsigned char *)calloc(frames, PAGE_SIZE);
    if(!pm->data){
        printf("PhysMem failed");
        return 1; 
    }
    //treat backingstore as a random access table, use fopen, fread, fseek, fclose
    FILE *bin = fopen("BACKING_STORE.bin", "r");
    if(bin == NULL){
        printf("Couldn't open BACKING_STORE.bin file");
        return 1;
    }

    int translated_addr = 0;
    int page_faults = 0;
    int TLB_hit = 0;
    int TLB_miss = 0;

    //create page table 
    PageTable *pt = (PageTable *)calloc(1, sizeof(*pt));
    if(!pt){
        printf("Page table failed");
        return 1;
    }
    TLB *tlb = (TLB *)malloc(sizeof(*tlb));
    if(!tlb){
        printf("TLB failed");
        return 1;
    }
    tlb_create(tlb);

    //translate logical ->physical w/ TLB and page table
    char *buf = NULL;
    size_t len = 0;
    ssize_t read;

    int arr_cap = 128;
    int arr_count = 0;
    int *arr = malloc(sizeof(int) * arr_cap);
    if(!arr){
        printf("array failed");
        return 1;
    }

    while((read = getline(&buf, &len, fp)) != -1){
        //extract the buffer and put it into the extraction function
        char *endptr;
        char *b = buf;
        long address = strtol(b, &endptr, 10);
      
        /*if(endptr == b || *endptr != '\0'){
            printf("Invalid address");
            return 1;
        }*/

        //TODO: save page_num and page_offset somewhere maybe in a list and then run it thorugh the TLB hit/miss
        //idk if that would be the right implementation or to just directly implement the TLB hit or miss w/in this read loop directly     
        if(arr_cap == arr_count){
            arr_cap *= 2;
            int *temp = realloc(arr, arr_cap * sizeof(int));
            if(!temp){
                free(arr);
                return 1;
            }
            arr = temp;
        }
        arr[arr_count++] = (int)address;
        translated_addr += 1; 
    }
    free(buf);
    fclose(fp);

    for(int i = 0; i < arr_count; i++){
        int address = arr[i];
        int page_num = get_pagenum(address); 
        int page_offset = get_pageoffset(address); //offset is the same in the physical address
        int frame = -1;
        TLBHIT = false;
       
        //find the frame entry + check if hit or miss on TLB
        TLBEntry *curr_entry = tlb->front;
        while(curr_entry != NULL){
            if(curr_entry->valid && curr_entry->page_num == page_num){
                frame = curr_entry->frame_num;
            }
            curr_entry = curr_entry->next;
        }
        if(frame != -1){
            TLBHIT = true;
            TLB_hit += 1;
        }
                
        //else miss, memory reference to page table must be made
        if(!TLBHIT){
            TLB_miss += 1;
            //check if found in page table
            if(pt->entries[page_num].valid){
                frame = pt->entries[page_num].frame_number;
            }
            //if not found, we need to check bin
            else{
                page_faults += 1;
                //find if there's still frame space in physmem
                if(pm->num_used < pm->num_frames){
                    frame = pm->num_used;
                    pm->num_used += 1;
                } else { //otherwise, we have to do page replacement from bin to phys mem
                    //diff page replacement algos             
                    if(strcmp(pra, "fifo") == 0){
                        //replace oldest page w/ fifo, goes to next frame
                        frame = pm->fifo_next;
                        pm->fifo_next = (pm->fifo_next + 1) % pm->num_frames;
                    } else if(strcmp(pra, "opt") == 0){
                        //opt algorithm
                        //look at all the future addresses and see which ever comes last
                        int farthest = -1;
                        int victim = 0;
                        for(int k = 0; k < pm->num_frames; k++){
                            int next_used = arr_count; //max num
                            for(int l = i + 1; l < arr_count; l++){
                                if(get_pagenum(arr[l]) == pm->frame_page[k]){
                                    next_used = l;
                                    break;
                                }
                            }
                            if(next_used > farthest){
                                farthest = next_used;
                                victim = k;
                            }
                        }
                        frame = victim;
                    } else if(strcmp(pra, "lru") == 0){
                        //replace the one that was least recently ues (implement stack)
                        frame = 0; 
                        for(int f = 1; f < pm->num_frames; f++){
                            if(pm->last_used[f] < pm->last_used[frame]){
                                frame = f;
                            }
                        }
                    }
                    //remove old page from mem
                    int old_page = pm->frame_page[frame];
                    pt->entries[old_page].valid = false;
                    TLBEntry *old_entry = tlb->front;
                    while(old_entry != NULL){
                        if(old_entry->valid && old_entry->page_num == old_page){
                            old_entry->valid = false;
                        }
                        old_entry = old_entry->next;
                    }                    
                }
                //fseek based on frame number
                //then fread it to be the current page table
                //store in page frame, update page table and TLB
                if(fseek(bin, (long)page_num*PAGE_SIZE, SEEK_SET) != 0
                    || fread(pm->data + frame*PAGE_SIZE, 1, PAGE_SIZE, bin) != PAGE_SIZE){
                    printf("can't read page from .bin file");
                    return 1; 
                }

                //put into page table
                pm->frame_page[frame] = page_num;
                pt->entries[page_num].frame_number = frame;
                pt->entries[page_num].valid = true;
            }
                //add to tlb
            enqueue_TLB(tlb, page_num, frame);

        }
        pm->last_used[frame] = i; //to keep track of lru
        
        //addr = frame# * pagesize + offset#
        int physaddr = frame*PAGE_SIZE + page_offset;
        signed char val = (signed char)pm->data[physaddr];

        /*1. The logical address being translated (the integer value being read from addresses.txt).
        2. The corresponding physical address (what your program translates the logical address to).
        2.5 the index of the logical address
        3. The signed byte value stored in physical memory at the translated physical address.
        in x, y, z, w format*/
        printf("%d, %d, %d, ", address, val, i);
        for(int j = 0; j < PAGE_SIZE; j++){
            printf("%02X", pm->data[frame*PAGE_SIZE + j]);
        }
            printf("\n");

    }

    double TLB_hitrate = 0.0;
    double page_fault_rate = 0.0;
    if(translated_addr > 0){
        TLB_hitrate = (double)TLB_hit/translated_addr;
        page_fault_rate = (double)page_faults/translated_addr; //TODO: it's page faults/memory references
    } 
    
    fclose(bin);
    while(tlb->front != NULL){
        dequeue(tlb);
    }
    free(arr);
    free(pm->data);
    free(pm);
    free(pt);
    free(tlb);
    
    printf("Number of Translated Addresses = %d\n", translated_addr);
    printf("Page Faults = %d\n", page_faults);
    printf("Page Fault Rate = %.3f\n", page_fault_rate);
    printf("TLB Hits = %d\n", TLB_hit);
    printf("TLB Misses = %d\n", TLB_miss);
    printf("TLB Hit Rate = %.3f\n", TLB_hitrate);
    return 0;
}
