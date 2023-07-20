COMPILER = gcc
CFLAGS = -Wall -pedantic

EXES = char_count fibonacci experiment

all: ${EXES}

char_count:  char_count.c 
	${COMPILER} ${CFLAGS} char_count.c -o char_count
fibonacci:  fibonacci.c 
	${COMPILER} ${CFLAGS} fibonacci.c -o fibonacci
experiment:  experiment.c 
	${COMPILER} ${CFLAGS} experiment.c -o experiment

%.o: %.c %.h  makefile
	${COMPILER} ${CFLAGS} $< -c 

clean: 
	rm -f *~ *.o ${EXES}

run:
	./${EXES} 5 text_files