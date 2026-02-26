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

typedef struct BST_Node{
    struct BST_Node* left;
    struct BST_Node* right;
    int data;
} BST_Node;

void free_bst(BST_Node* head){
    if(head==NULL){
        return;
    }
    free_bst(head->left);
    free_bst(head->right);
    free(head);
}

void task5(){
/*
Remove the largest element in a binary search tree. Do this by removing the rightmost child
*/

BST_Node* head = malloc(sizeof(BST_Node));
head->data = 8;
head->left = malloc(sizeof(BST_Node));
head->left->data = 4;
head->right = malloc(sizeof(BST_Node));
head->right->data= 12;
head->right->left = malloc(sizeof(BST_Node));
head->right->left->data = 10;
head->right->right=malloc(sizeof(BST_Node));
head->right->right->data = 14;

BST_Node* cursor = head;
BST_Node* cursor2 = head->right;

while (cursor2!=NULL){
    if (cursor2->right==NULL && cursor2->left==NULL){
        cursor->right = NULL;
    }
    else if(cursor2->right==NULL && cursor2->left!=NULL){
        cursor->right = cursor2->left;
        free(cursor2);
    }
    else{
        cursor2 = cursor2->right;
        cursor = cursor->right;
    }
}

free_bst(head);

}

int main(int argc, char* argv[]){
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
   elapsed = (end.tv_sec - start.tv_sec) * 10000000 + (end.tv_usec - start.tv_usec);
   //divides 50 because we want average
   printf("Workload average: %ld microseconds\n", elapsed / 50);
   
   return EXIT_SUCCESS;
}