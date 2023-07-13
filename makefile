COMPILER = gcc
CFLAGS = -Wall -pedantic

EXES = ring

all: ${EXES}


ring:  ring.c 
	${COMPILER} ${CFLAGS} ring.c -o ring
chain:  chain.c 

%.o: %.c %.h  makefile
	${COMPILER} ${CFLAGS} $< -c 

clean: 
	rm -f *~ *.o ${EXES}