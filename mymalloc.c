#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#define MEMLENGTH 4096

static bool initialized = false;

static union {
    char bytes[MEMLENGTH];
    double not_used;
} heap;

typedef struct{
    int next_data_size;
    unsigned int prev_data_size;
} Metadata;

void initialize_heap();
void check_memory_leak_exit();
void* mymalloc(size_t size, char* file, int line);
void coalesce(void* ptr);
bool valid_pointer(void* ptr);
void myfree (void *ptr, char *file, int line);
void unsafe_free (void *ptr);

// function to add the initial metadata to the heap
void initialize_heap(){
    // don't do anything if initialized
    if(initialized){
        return;
    }
    
    Metadata * initial_metadata = (Metadata *)heap.bytes;
    // initialize the metadata size, the first metadata is the size of the entire free space available
    // negative data size means it is being used/unavailable, positive means it is available
    initial_metadata -> next_data_size = MEMLENGTH - sizeof(Metadata);
    //Basically the prev_data_size = 0 is the initial metadata
    initial_metadata -> prev_data_size = 0;

    //handle exit handler during initialization
    atexit(check_memory_leak_exit);
    initialized=true;
}

//exit logic
void check_memory_leak_exit(){
    printf("\n Checking memory leak\n");

    //current is a char* uses pointer arithmetic to iterate through the malloc array
    char* current = (char *)heap.bytes;

    size_t total_bytes_leaked = 0;
    size_t num_objects_leaked = 0;
    while(current< (char *)heap.bytes + MEMLENGTH){
        Metadata* cur_metadata = (Metadata*)current;
        
        //logic if a leak is found
        if ( cur_metadata->next_data_size<0){
            total_bytes_leaked+=abs(cur_metadata->next_data_size);
            num_objects_leaked+=1;
        }
        
        current+=sizeof(Metadata)+abs(cur_metadata->next_data_size);
    }
    if(total_bytes_leaked>0){
        fprintf(stderr,"mymalloc: %zu bytes leaked in %zu objects.\n",total_bytes_leaked,num_objects_leaked);
    }
    printf("0 bytes leaked, memory free successful\n");
}   

//malloc logic
void * mymalloc (size_t size, char *file, int line){

    initialize_heap();

    char* heap_end = (char*)heap.bytes+MEMLENGTH;
    char* cur_heap_spot = (char*)heap.bytes;

    size = (int)size;
    int size_req = (size+7)&~7;
    while (cur_heap_spot<heap_end){
        Metadata* cur_metadata = (Metadata*)cur_heap_spot;
        if (cur_metadata->next_data_size>0 && cur_metadata->next_data_size>=size_req){
            //Splitting process, we'll split if we can reasonably fit another metadata
            int split_req = size_req + sizeof(Metadata) + 8;
            //we will split if we can fit a new metadata and at least 8 bytes
            if (cur_metadata->next_data_size>=split_req){
                char* new_metadata_spot = cur_heap_spot + sizeof(Metadata) + size_req;
                Metadata* new_metadata = (Metadata*) new_metadata_spot;
                new_metadata->next_data_size = cur_metadata->next_data_size-size_req-sizeof(Metadata);
                new_metadata->prev_data_size = size_req;
                cur_metadata->next_data_size = size_req;
                
                //update the post metadata as well
                char* post_split = new_metadata_spot + sizeof(Metadata) + new_metadata->next_data_size;
                if (post_split<heap_end){
                    Metadata* post_metadata = (Metadata *) post_split;
                    post_metadata->prev_data_size = new_metadata->next_data_size;
                }
                
            }
            cur_metadata->next_data_size*=-1;
            
            return cur_heap_spot+sizeof(Metadata);
        }
        cur_heap_spot+=sizeof(Metadata)+abs(cur_metadata->next_data_size);
    }
    
    fprintf(stderr,"Malloc unable to allocate, size too large.  Error at line %d in file %s \n", line, file);

    return NULL;
}

void coalesce(void* ptr) {
    char* heap_end = (char*)heap.bytes + MEMLENGTH;
    char* curptr = (char*)ptr;
    Metadata* cur_metadata = (Metadata *)(curptr-sizeof(Metadata));
    bool right_free = false;
    bool left_free = false;
    Metadata* my_left;
    Metadata* my_right;
    
    if((curptr+cur_metadata->next_data_size) < heap_end){
        Metadata* right_metadata = (Metadata *)(curptr+cur_metadata->next_data_size);
        my_right = right_metadata;
        if (right_metadata->next_data_size>0){
            right_free = true;
        }
    }
    if((curptr-cur_metadata->prev_data_size-2*sizeof(Metadata))>=(char*)heap.bytes){
        Metadata* left_metadata = (Metadata *)(curptr-cur_metadata->prev_data_size-2*sizeof(Metadata));
        my_left = left_metadata;
        if (left_metadata->next_data_size>0){
            left_free = true;
        }
    }

    if (right_free && left_free){
        char* next = (char*) my_right + my_right->next_data_size + sizeof(Metadata);
        my_left->next_data_size+= 2*sizeof(Metadata) + cur_metadata->next_data_size +my_right->next_data_size;
        if (next<heap_end){
            Metadata* next_m = (Metadata*)next;
            next_m->prev_data_size = my_left->next_data_size;
        }
    }
    else if (right_free && !left_free){
        char* next = (char*) my_right + my_right->next_data_size + sizeof(Metadata);
        cur_metadata->next_data_size += sizeof(Metadata) + my_right->next_data_size;
        if (next<heap_end){
            Metadata* next_m = (Metadata*)next;
            next_m->prev_data_size = cur_metadata->next_data_size;
        }

    }
    else if(!right_free && left_free){
        char* next = ((char*)cur_metadata)+cur_metadata->next_data_size+sizeof(Metadata);
        my_left->next_data_size += sizeof(Metadata) + cur_metadata->next_data_size;

        if( next < heap_end){
            Metadata* next_metadata = (Metadata*) next;
            next_metadata->prev_data_size = my_left->next_data_size;
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
            return cur_metadata->next_data_size<0;
        }
        current += sizeof(Metadata) + abs(cur_metadata->next_data_size);
    }
    //pointer in bounds but does not match chunk's payload
    return false;
}

//free logic
void myfree (void *ptr, char *file, int line){
    initialize_heap();
    //code to go to the beginning of a metadata instead of the chunk
    if (valid_pointer(ptr)){
        Metadata* cur_metadata = (Metadata*)((char*) ptr - sizeof(Metadata));
        cur_metadata->next_data_size*= -1;
        coalesce(ptr);
    } 
    else{
        fprintf(stderr,"free: Inappropriate pointer (%s:%d)\n",file,line);
    }


}

void unsafe_free (void *ptr){
    initialize_heap();
    Metadata* cur_metadata = (Metadata*)((char*)ptr - sizeof(Metadata));
    cur_metadata->next_data_size*=-1;
    coalesce(ptr);
}

