#include <stdlib.h>
#include <stdbool.h>
#define MEMLENGTH 4096

//static initialization variable
static bool initialized = false;

static union {
    char bytes[MEMLENGTH];
    double not_used;
} heap;

typedef struct{
    size_t metadata_size;
    bool is_free;
} Metadata;

// function to add the initial metadata to the heap
void initialize_heap(){
    Metadata * initial_metadata = (Metadata *)heap.bytes;

    // initialize the metadata size, the first metadata is the size of the entire free space available
    initial_metadata -> metadata_size = MEMLENGTH - sizeof(Metadata);
    initial_metadata -> is_free = true;
}

void * mymalloc (size_t size, char *file, int line){
    if (!initialized){
        //initialize logic
        initialize_heap();
        initialized = true;
    }
    return NULL;
};
void myfree (void *ptr, char *file, int line);
