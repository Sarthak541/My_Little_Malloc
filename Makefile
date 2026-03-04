CC = gcc
CFLAGS = -Wall -Wextra -g -fsanitize=address,undefined

# List all executables here
TARGETS = testsuite realmalloc leaker

# Common object files
LIB_OBJ = mymalloc.o

# All target: builds both programs
all: $(TARGETS)

# Link testsuite
testsuite: testsuite.o $(LIB_OBJ)
	$(CC) $(CFLAGS) -o testsuite testsuite.o $(LIB_OBJ)

#Compile realmalloc
realmalloc: testsuite.c
	$(CC) $(CFLAGS) -DREALMALLOC -o realmalloc testsuite.c

leaker: testsuite.c
	$(CC) $(CFLAGS) -DREALMALLOC -DLEAK -o leaker testsuite.c

# Compile rules
mymalloc.o: mymalloc.c mymalloc.h
	$(CC) $(CFLAGS) -c mymalloc.c

testsuite.o: testsuite.c mymalloc.h
	$(CC) $(CFLAGS) -c testsuite.c


clean:
	rm -f *.o $(TARGETS)