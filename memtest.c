#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


// Compile with -DREALMALLOC to use the real malloc() instead of mymalloc()
#ifndef REALMALLOC
#include "mymalloc.h"
#endif

// Compile with -DLEAK to leak memory
#ifndef LEAK
#define LEAK 0
#endif

#define MEMSIZE 4096
#define HEADERSIZE 8
#define OBJECTS 64
#define OBJSIZE (MEMSIZE / OBJECTS - HEADERSIZE)

int 
main (int argc, char **argv)
{
	char *obj[OBJECTS];
	int i, j, errors = 0;
	
	// fill memory with objects
	for (i = 0; i < OBJECTS; i++) {
		obj[i] = malloc (OBJSIZE);
		if (obj[i] == NULL) {
		    printf ("Unable to allocate object %d\n", i);
		    exit (EXIT_FAILURE);
		}
	}
	
	// fill each object with distinct bytes
	for (i = 0; i < OBJECTS; i++) {
		memset (obj[i], i, OBJSIZE);
	}
	
	// check that all objects contain the correct bytes
	for (i = 0; i < OBJECTS; i++) {
		for (j = 0; j < OBJSIZE; j++) {
			if (obj[i][j] != i) {
				errors++;
				printf ("Object %d byte %d incorrect: %d\n", i, j, obj[i][j]);
			}
		}
	}

	// free all objects
	if (!LEAK) {
	    for (i = 0; i < OBJECTS; i++) {
		free (obj[i]);
	    }
	}
	
	printf("%d incorrect bytes\n", errors);

	// Requirement Tested: free() deallocates memory
	// (1) allocates objects until heap if full, (2) frees, (3) then allocates again
	//If free() works --> reallocation should work
	printf("\n Test: free() deallocates memory \n");
	{
		char *ptrs[OBJECTS];
		int alloc_errors = 0;
		//First Allocation
		for(i = 0; i < OBJECTS; i++){
			ptrs[i] = malloc(OBJSIZE);
			if(ptrs[i] = NULL){
				printf("First Allocation: unable to allocate object %d\n", i);
				alloc_errors++;
			}
		}
		//Free
		for(i = 0; i < OBJECTS; i++){
			if(ptrs[i] != NULL) {
				free(ptrs[i]);
				ptrs[i] = NULL;
			}
		}
		//Second Allocation
		for(i = 0; i < OBJECTS; i++){
			ptrs[i] = malloc(OBJSIZE);
			if(ptrs[i] = NULL){
				printf("Second Allocation: unable to allocate object %d --> free may not work\n", i);
				alloc_errors++;
			}
		}
		for(i = 0; i < OBJECTS; i++){
			if(ptrs[i] != NULL) {
				free(ptrs[i]);
			}
		}
		if(alloc_errors == 0){
			printf("free() deallocates memory; test passed\n");
		}
		else{
			printf("free() does not deallocate memory; test failed with %d errors\n", alloc_errors);
		}
	}
	
	// Requirement Tested: coealescing adjacent free blocks 
	// (1) allocates many small objects to heap, (2) frees them, (3) attempts a large allocation
	// If coalescing works --> step 3 will work
	printf("\n Test: coealescing adjacent free blocks \n");
	{
		//allocate small chunks
		char *ptrs[OBJECTS];
		for(i = 0; i < OBJECTS; i++){
			ptrs[i] = malloc(OBJSIZE);
			if(ptrs[i] = NULL){
				printf("Coalesce setup: unable to allocate object%d\n", i);
			}
		}
		//free small chunks
		for(i = 0; i < OBJECTS; i++){
			if(ptrs[i] = NULL){
				free(ptrs[i]);
				ptrs[i] = NULL;
			}
		}
		//allocates large
		size_t large_size = MEMSIZE - HEADERSIZE * 2;
		char* large = malloc(large_size);
		if(large == NULL){
			printf("Coalesce test failed: could not allocate %zu bytes after freeing all objects\n", large_size);
		}
		else{
			printf("Coalesce test passed: large allocation succeeded after freeing small chunks\n", large_size);
			free(large);
		}
	}

	// Requirement Tested: leaked objects detected 
	if (LEAK){
		printf("\n Test: Leak detection \n");
		printf("LEAK mode enabled: objects from original test were not freed\n");
		printf("Check stderr at program exit for the leak detector message.\n");
	}
	
	return EXIT_SUCCESS;
}
