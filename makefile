COMPILER = gcc
CFLAGS = -Wall -pedantic

EXES = char_count

all: ${EXES}

char_count:  char_count.c 
	${COMPILER} ${CFLAGS} char_count.c -o char_count -lm

%.o: %.c %.h  makefile
	${COMPILER} ${CFLAGS} $< -c 

clean: 
	rm -f *~ *.o ${EXES}

run:
	./${EXES} 5 text_files