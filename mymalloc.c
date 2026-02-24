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
    //boolean to check if leak was found
    bool found_leak = false;
    //current is a char* uses pointer arithmetic to iterate through the malloc array
    char* current = (char *)heap.bytes;
    while(current< (char *)heap.bytes + MEMLENGTH){
        Metadata* cur_metadata = (Metadata*)current;
        
        //logic if a leak is found
        if ( !cur_metadata->is_free){
            fprintf(stderr,"memory leaked, %zu bytes used at location %p",cur_metadata->data_size,current+sizeof(Metadata));
            found_leak = true;
        }
        
        current+=sizeof(Metadata)+cur_metadata->data_size;
    }
    if(!found_leak){
        printf("Exit Success!");
    }
}
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
void myfree (void *ptr, char *file, int line);
