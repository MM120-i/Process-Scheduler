# Compiler to be used
CC = gcc
# Header files that the project depends on
DEPS = scheduler.h list.h process.h
# Object files to be generated
OBJ = scheduler.o list.o
# Compiler flags
CFLAGS = -Wall
# Standard flag for C99
STD_FLAG = -std=c99
# Libraries to be linked
LIBS = -lm

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) $(STD_FLAG) -c -o $@ $<

scheduler: $(OBJ)
	$(CC) $(CFLAGS) $(STD_FLAG) -o $@ $^ $(LIBS)

clean:
	rm -rf *.o scheduler
