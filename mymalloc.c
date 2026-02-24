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
    size_t data_size;
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
        fprintf(stderr,"mymalloc: %zu bytes leaked in %zu objects.",total_bytes_leaked,num_objects_leaked);
    }
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
    return NULL;
};

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
    Metadata* cur_metadata = (Metadata*)((char*) ptr - sizeof(Metadata));
    cur_metadata->is_free = true;

};
