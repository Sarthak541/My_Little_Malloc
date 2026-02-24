#include <stdlib.h>
#include <stdbool.h>
#define MEMLENGTH 4096

static union {
    char bytes[MEMLENGTH];
    double not_used;
} heap;

struct metadata{
    size_t metadata_size;
    bool is_free;
}Metadata;

void * mymalloc (size_t size, char *file, int line){
    static bool initialized = false;
    if (!initialized){
        //initialize logic
        initialized = true;
    }
};
void   myfree (void *ptr, char *file, int line);
