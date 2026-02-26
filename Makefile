CC = gcc
CFLAGS = -Wall -Wextra -g -fsanitize=address,undefined

# List both executables here
TARGETS = memtest memgrind

# Common object files
LIB_OBJ = mymalloc.o

# All target: builds both programs
all: $(TARGETS)

# Link memtest
memtest: memtest.o $(LIB_OBJ)
	$(CC) $(CFLAGS) -o memtest memtest.o $(LIB_OBJ)

# Link memgrind
memgrind: memgrind.o $(LIB_OBJ)
	$(CC) $(CFLAGS) -o memgrind memgrind.o $(LIB_OBJ)

# Compile rules
mymalloc.o: mymalloc.c mymalloc.h
	$(CC) $(CFLAGS) -c mymalloc.c

memtest.o: memtest.c mymalloc.h
	$(CC) $(CFLAGS) -c memtest.c

memgrind.o: memgrind.c mymalloc.h
	$(CC) $(CFLAGS) -c memgrind.c

clean:
	rm -f *.o $(TARGETS)