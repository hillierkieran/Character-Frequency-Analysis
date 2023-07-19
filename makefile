COMPILER = gcc
CFLAGS = -Wall -pedantic

EXES = new

all: ${EXES}


new:  new.c 
	${COMPILER} ${CFLAGS} new.c -o new

%.o: %.c %.h  makefile
	${COMPILER} ${CFLAGS} $< -c 

clean: 
	rm -f *~ *.o ${EXES}

run:
	./${EXES} 10 text_files