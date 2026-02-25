#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h> 

// Compile with -DREALMALLOC to use the real malloc() instead of mymalloc()
#ifndef REALMALLOC
#include "mymalloc.h"
#endif

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
}
void task5(){
/*
Remove the second largest element in a binary tree. Do this by removing the parent of
the rightmost child
*/
}

int main(int argc, char* argv[]){

    task1();
    task2();
    task3();
    task4();
    task5();
    
    return EXIT_SUCCESS;
}