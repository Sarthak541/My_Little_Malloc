#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#define MEMLENGTH 4096

//static initialization variable
static bool initialized = false;

static union {
    char bytes[MEMLENGTH];
    double not_used;
} heap;

typedef struct{
    unsigned int data_size;
    bool is_free;
} Metadata;

// function to add the initial metadata to the heap
void initialize_heap(){
    Metadata * initial_metadata = (Metadata *)heap.bytes;

    // initialize the metadata size, the first metadata is the size of the entire free space available
    initial_metadata -> data_size = MEMLENGTH - sizeof(Metadata);
    initial_metadata -> is_free = true;
}

//exit logic
void check_memory_leak_exit(){
    printf("Checking memory leak\n");

    //current is a char* uses pointer arithmetic to iterate through the malloc array
    char* current = (char *)heap.bytes;

    size_t total_bytes_leaked = 0;
    size_t num_objects_leaked = 0;
    while(current< (char *)heap.bytes + MEMLENGTH){
        Metadata* cur_metadata = (Metadata*)current;
        
        //logic if a leak is found
        if ( !cur_metadata->is_free){
            total_bytes_leaked+=cur_metadata->data_size;
            num_objects_leaked+=1;
        }
        
        current+=sizeof(Metadata)+cur_metadata->data_size;
    }
    if(total_bytes_leaked>0){
        fprintf(stderr,"mymalloc: %zu bytes leaked in %zu objects.\n",total_bytes_leaked,num_objects_leaked);
    }
    printf("0 bytes leaked, memory free successful\n");
}   

//malloc logic
void * mymalloc (size_t size, char *file, int line){
    if (!initialized){
        //initialize logic
        initialize_heap();
        
        //set exit handler
        atexit(check_memory_leak_exit);
        
        initialized = true;
    }

    char* heap_end = (char*)heap.bytes+MEMLENGTH;
    char* cur_heap_spot = (char*)heap.bytes;

    size_t size_req = (size+7)&~7;
    while (cur_heap_spot<heap_end){
        Metadata* cur_metadata = (Metadata*)cur_heap_spot;
        if (cur_metadata->is_free && cur_metadata->data_size>=size_req){
            //Splitting process, we'll split if we can reasonably fit another metadata
            size_t split_req = size_req + sizeof(Metadata) + 8;
            //we will split if we can fit a new metadata and at least 8 bytes
            if (cur_metadata->data_size>=split_req){
                char* new_metadata_spot = cur_heap_spot + sizeof(Metadata) + size_req;
                Metadata* new_metadata = (Metadata*) new_metadata_spot;
                new_metadata->data_size = cur_metadata->data_size-size_req-sizeof(Metadata);
                new_metadata->is_free = true;
                cur_metadata->data_size=size_req;
            }
            cur_metadata->is_free=false;
            
            return cur_heap_spot+sizeof(Metadata);
        }
        cur_heap_spot+=sizeof(Metadata)+cur_metadata->data_size;
    }
    return NULL;
}

void coalesce() {
    char* heap_end = (char*)heap.bytes + MEMLENGTH;
    char* curleft = (char*)heap.bytes;

    while (curleft < heap_end) {
        Metadata* left_m = (Metadata*)curleft;
        
        char* curright = curleft + sizeof(Metadata) + left_m->data_size;

        // If the right block is outside the heap, we end
        if (curright >= heap_end) {
            break;
        }

        Metadata* right_m = (Metadata*)curright;

        if (left_m->is_free && right_m->is_free) {
            // Merge logic
            left_m->data_size += sizeof(Metadata) + right_m->data_size;
        } else {
            // No merge so iterate
            curleft = curright;
        }
    }
}

//valid_pointer checks if agiven pointer is a valid payload pointer
bool valid_pointer(void* ptr){
    if(ptr == NULL){
        return false;
    }
    //checks if pointer is within bounds
    char* heap_start = (char*)heap.bytes + sizeof(Metadata);
    char* heap_end = (char*)heap.bytes + MEMLENGTH;
    if((char*)ptr < heap_start || (char*)ptr >= heap_end){
        return false;
    }
    //check if pointer matters payload start
    char* current = (char*)heap.bytes;
    while(current < heap_end){
        Metadata* cur_metadata = (Metadata*)current;
        char* payload = current + sizeof(Metadata);
        if(payload == (char*)ptr){
            //matches payload , return true if payload is not free
            return !cur_metadata->is_free;
        }
        current += sizeof(Metadata) + cur_metadata->data_size;
    }
    //pointer in bounds but does not match chunk's payload
    return false;
}

//free logic
void myfree (void *ptr, char *file, int line){
    if (!initialized){
        //initialize logic
        initialize_heap();
        
        //set exit handler
        atexit(check_memory_leak_exit);
        
        initialized = true;
    }
    //code to go to the beginning of a metadata instead of the chunk
    if (valid_pointer(ptr)){
        Metadata* cur_metadata = (Metadata*)((char*) ptr - sizeof(Metadata));
        cur_metadata->is_free = true;
    }
    else{
        fprintf(stderr,"free: Inappropriate pointer (%s:%d)\n",file,line);
        exit(2);
    }

    //call coalesce after every free to ensure all memory is accessible
    coalesce();

}


