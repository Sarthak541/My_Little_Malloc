
# Malloc Implementation in C

malloc() is a widely used function to use dynamically allocated memory in C. Alongside 
malloc(), the free() function is used to deallocate memory.  This project provides an 
implementation of malloc() in C alongside a rigorous suite of tests and a read me containing detailed information about the project.

### Build Guide
In order to build this project, you must have gcc installed.  After cloning the repository, type "make" in the terminal
to compile the executables.  You can run the executables to test the code.  Type "make clean" to remove all executables
and object files.

### Understanding malloc
My understanding of malloc is that we are creating a global variable called a HEAP, which is
basically an array, and using that to store variable data.  To do this I used metadata that
tells us the size of the allocated memory and whether or not it is free.  Our metadata acts like
a linked list because the size of the allocated memory tells us where the next metadata will be.

## Section 1 - mymalloc.c design + testing myfree()

### myfree()
To begin, let me explain our process of creating myfree().  We first call a function to check for 
a valid pointer.  This does 3 things -
- Checks if pointer is null
- Checks if pointer is out of bounds of the heap
- Checks if the pointer is valid within the heap
If not then the pointer is not valid.  Finally we check if the pointer has already been freed. 
Since double free is an error, this also results in another error message.
Every time we called free we also called coalesce, which is an O(n) operation to check for free.
There are further optimizations for this coalesce() function which we chose not to pursue.

### mymalloc()
mymalloc is the main function and to implement this, we went through the heap until we found a large
enough empty space and returned the metadata.  In order not to take up too much space we also implemented
a splitting function for mymalloc().  If my malloc() does not work, we printed to stderr.

## Section 2 - memtest.c

memtest.c tests the following -

- Check if malloc reserves any unallocated memory

- Check if free deallocates memory
We do this by allocating objects until the heap is full, freeing the objects then allocating the objects again

- Check if the heap coalesces metadata
We do this by allocating many tiny objects, freeing them, then allocating 1 big object

- Test if malloc works if the memory size allocated is too large

Additionally, we have added leak detection which applies anytime malloc is used.  This is implemented with an exit handler
during initialization and is present within mymalloc.c

## Section 3 - memgrind.c

Memgrind.c contains 5 tests that contain stress testing for our code.
> None of the Memgrind tests take any arguments  
We run these 5 tasks and then check their average time using the <sys/time.h> library provided to us in C.

The five tasks are - 

- Task 1 : malloc() and free() a 1 byte object 120 times
Since we are freeing a 1 byte obejct, I chose to cast to a char* because chars are 1 byte in C.

- Task 2 : Store 1 byte malloc() values in an array of pointers then dealocate them with free().
This is pretty self explanatory, since it was 1 byte again we used called 1 byte to malloc and used char*.

- Task 3: Randomly allocate and deallocate an array of 120 pointers.
I wrote this in a for loop where during each loop, I would modulus a random integer by 2 and allocate or deallocate 
depending on the result. I would use a random number generator and modulus with the number of allocated integers
to deallocate numbers from the array randomly.  Then I would fill in the deallocated portion with the last pointer 
in the array to shrink the available allocated space with each deallocation.

- Task 4: Remove all even position elements from a linked list.
I created a linked list of variable size using the rand() function and iterated through to get rid of the even position
ones.  I would temporarily store a pointer to these removed nodes in order to free them.

- Task 5: Remove the largest element from the binary search tree
Once again this is pretty simple.  I would iterate through a binary search tree until I found a value that was very large.
I would proceed to remove this value and replace it either with its left node or with no node if it does not have a left node.

> Note that we freed the link list and the binary search tree after completed our tasks.

## Section 4 - Makefile

We created our Makefile to have 5 executables. Each of our executables would use malloc meaning that they all needed to be linked
with a mymalloc.o file.  Therefore, mymalloc.o was added to our LIB_OBJ file.  In order to compile, the other object files, we included
mymalloc.h with all the other files.

For example -
mymalloc.o: mymalloc.c mymalloc.h
	$(CC) $(CFLAGS) -c mymalloc.c

mymalloc.h is added for all the compilations because it contains the function prototypes. Just like mymalloc.o is there for all of the linking.

## Future Goals

- Combine memtest.c and memgrind.c into 1 file called testsuite.c -COMPLETED
- Implement all test cases including the check of free - COMPLETED
- Implement a new unsafe free function (ufree()) that does not check if a pointer is valid before doing free - COMPLETED
- Implement the O(1) coalesce and free using doubly linked lists - COMPLETED
- Improve ReadMe.md by moving the concept explanation to the wiki
- Change the makefile to make an extra -DREALMALLOC and -DLEAK for the test file -COMPLETED
- Separate testsuite.c into 3 functions.  Test that check whether a function works, tests that check if an error is triggered, test memgrind.c for intensive workload testing - COMPLETED
- Improve the binary tree task into a more creative task in memgrind.c - COMPLETED
- Remove all warnings during compilation by fixing the warnings
