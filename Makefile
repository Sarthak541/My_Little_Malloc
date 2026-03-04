CC = gcc
CFLAGS = -Wall -Wextra -g -fsanitize=address,undefined

# List both executables here
TARGETS = testsuite memgrind

# Common object files
LIB_OBJ = mymalloc.o

# All target: builds both programs
all: $(TARGETS)

# Link memtest
testsuite: testsuite.o $(LIB_OBJ)
	$(CC) $(CFLAGS) -o testsuite testsuite.o $(LIB_OBJ)

# Link memgrind
memgrind: memgrind.o $(LIB_OBJ)
	$(CC) $(CFLAGS) -o memgrind memgrind.o $(LIB_OBJ)

# Compile rules
mymalloc.o: mymalloc.c mymalloc.h
	$(CC) $(CFLAGS) -c mymalloc.c

testsuite.o: testsuite.c mymalloc.h
	$(CC) $(CFLAGS) -c testsuite.c

memgrind.o: memgrind.c mymalloc.h
	$(CC) $(CFLAGS) -c memgrind.c

clean:
	rm -f *.o $(TARGETS)