#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>

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

void memgrind();
void memtest();
void task1();
void task2();
void task3();
void task4();
void task5();

typedef struct BST_Node{
    struct BST_Node* left;
    struct BST_Node* right;
    int data;
} BST_Node;
void free_bst(BST_Node* head);

void task1(){
        for (size_t i = 0; i < 120 ; i++){
        char* my_obj = malloc(1);
        free(my_obj);
    }
}

void task2(){
    char *ptrs[120];
    for(int i =0; i < 120; i++){
        ptrs[i] = malloc(1);
    }
    for(int i = 0; i < 120; i++){
        free(ptrs[i]);
    }
}

void task3() {
    char *ptrs[120];
    int current_count = 0;
    int total_mallocs = 0;

    while (total_mallocs < 120) {
        int action = rand() % 2;

        if (action == 0) { // Allocate
            ptrs[current_count] = (char*)malloc(1);
            if (ptrs[current_count] != NULL) {
                current_count++;
                total_mallocs++;
            }
        } 
        else if (current_count > 0) { 
            int rand_idx = rand() % current_count;
            free(ptrs[rand_idx]);
            ptrs[rand_idx] = ptrs[current_count - 1];
            current_count--;
        }
    }

    // Deallocate remaining
    for (int i = 0; i < current_count; i++) {
        free(ptrs[i]);
    }
}

void task4(){
/*
Get rid of every even element in a linked list.
In order to do this first typedef a struct, then use this struct for the linked list
To remove the third element, first connect the second element to the fourth element
temporarily store the third element
then free the third element
*/
    //defining linked list node
    typedef struct Node {
        int data;
        struct Node* next;
    }Node;
    // creating random linked list under size 10
    int size = (rand()%9) + 1;
    //populate 
    Node* head = malloc(sizeof(Node));
    head->data = 0;
    head->next = NULL;
    Node* current = head;
    //construct linked list
    for(int i = 1; i < size; i++){
        Node* new_node = malloc(sizeof(Node));
        new_node->data = i;
        new_node->next = NULL;
        current->next = new_node;
        current = new_node;
    }
    //remove position 0 separately
    if(head != NULL){
        Node* temp = head;
        head = head->next;
        free(temp);
    }
    //traverse and remove every even position
    //since we removed position 0, we have to remove the odd ones in the current linked list(position 1 is at position 0)
    current = head;
    while(current != NULL && current->next != NULL){
        Node* temp = current->next;
        current->next = current->next->next;
        free(temp);
        current = current->next;
    }
    //free
    current = head;
    while(current != NULL){
        Node* temp = current;
        current = current->next;
        free(temp);
    }
}
BST_Node* insert_node(BST_Node* head, int toinsert){
	
	if (head==NULL){
		BST_Node* new_val = malloc(sizeof(BST_Node));
		//handle memory allocation failure
		if (new_val==NULL) return NULL;
		
		new_val->data=toinsert;
		new_val->left=NULL;
		new_val->right=NULL;
		
		return new_val;
	}
	else if(head->data>toinsert){
		head->left = insert_node(head->left,toinsert);
	}
	else if(head->data<=toinsert){
		head->right = insert_node(head->right,toinsert);
	}

	return head;

}

void remove_rightmost_child(BST_Node* head){
	BST_Node* cursor = head;
	BST_Node* cursor2 = head->right;

	while (cursor2 != NULL) {
		if (cursor2->right == NULL) { // We found the largest!
			if (cursor2->left == NULL) {
				cursor->right = NULL;
			} else {
				cursor->right = cursor2->left;
			}
			free(cursor2);
			cursor2 = NULL; // EXIT the loop
		}
		else {
			cursor = cursor2; // Keep cursor one step behind cursor2
			cursor2 = cursor2->right;
		}
	}
}

void task5(){
	/*
	Remove the largest element in a random size binary search tree. Do this by removing the rightmost child
	*/
	int tree_size = (rand()%5)+5;
	BST_Node* head = NULL;
	for( int i = 0; i< tree_size; i++){
		int val = rand()%20+5;
		head = insert_node(head,val);
	}
	remove_rightmost_child(head);
	free_bst(head);
}

void free_bst(BST_Node* head){
    if(head==NULL){
        return;
    }
    free_bst(head->left);
    free_bst(head->right);
    free(head);
}

void memgrind(){
   struct timeval start;
   struct timeval end;
   //since timeval(which is based on sys/time.h) is long, elapsed is also long
   long elapsed;
   //task1 timing
   gettimeofday(&start, NULL);
   for(int i = 0; i<50; i++){
       task1();
       task2();
       task3();
       task4();
       task5();
   }
   gettimeofday(&end, NULL);
   //think of this as a mixed fraction --> whole number
   elapsed = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
   //divides 50 because we want average
   printf("\n Test: Intensive workload (memgrind)\n");
   printf("Workload average: %ld microseconds\n", elapsed / 50);

}

void memtest(void (*freefunc)(void*)){
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
		freefunc (obj[i]);
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
			if(ptrs[i] == NULL){
				printf("First Allocation: unable to allocate object %d\n", i);
				alloc_errors++;
			}
		}
		//Free
		for(i = 0; i < OBJECTS; i++){
			if(ptrs[i] != NULL) {
				freefunc(ptrs[i]);
				ptrs[i] = NULL;
			}
		}
		//Second Allocation
		for(i = 0; i < OBJECTS; i++){
			ptrs[i] = malloc(OBJSIZE);
			if(ptrs[i] == NULL){
				printf("Second Allocation: unable to allocate object %d --> free may not work\n", i);
				alloc_errors++;
			}
		}
		for(i = 0; i < OBJECTS; i++){
			if(ptrs[i] != NULL) {
				freefunc(ptrs[i]);
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
			if(ptrs[i] == NULL){
				printf("Coalesce setup: unable to allocate object%d\n", i);
			}
		}
		//free small chunks
		for(i = 0; i < OBJECTS; i++){
			if(ptrs[i] != NULL){
				freefunc(ptrs[i]);
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
			printf("Coalesce test passed: large allocation succeeded after freeing small chunks\n");
			freefunc(large);
		}
	}
	
	// Requirement Tested: leaked objects detected 
	if (LEAK){
		printf("\n Test: Leak detection \n");
		printf("LEAK mode enabled: objects from original test were not freed\n");
		printf("Check stderr at program exit for the leak detector message.\n");
	}

}

void free_wrapper(void* ptr){
	free(ptr);
}

#ifndef REALMALLOC
void ufree_wrapper(void* ptr){
	ufree(ptr);
}
#endif

void error_test(){

	// malloc rejects too large allocations
	printf("\n Test: allocate an object that is too big\n");
	int* my_array = (int*)malloc(sizeof(int)*MEMSIZE*2);
	if (my_array == NULL){
		fprintf(stderr,"Malloc unable to allocate size too large\n");
	}

#ifndef REALMALLOC
	printf("\n Test: double free\n");
	int* my_val = (int*)malloc(sizeof(int)*3);
	free(my_val);
	free(my_val);

	printf("\n Test: free pointer inside object\n");
	int* my_val2 = (int*)malloc(sizeof(int)*3);
	free(my_val2+1);
	//this actually frees my val for the next test case
	free(my_val2);

	printf("\n Test: free non heap object\n");
	static int x[128];
	int*y = &x[5];
	free(y);
#endif



}

int main() {

	//wrappers are used to bypass the macro
	memtest(free_wrapper);
#ifndef REALMALLOC
	memtest(ufree_wrapper);
#else
	printf("Skipping unsafe free\n");
#endif

	error_test();

	memgrind();
	
	return EXIT_SUCCESS;
}