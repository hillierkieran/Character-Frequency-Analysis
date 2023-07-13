COMPILER = gcc
CFLAGS = -Wall -pedantic

EXES = charfreq

all: ${EXES}


charfreq:  charfreq.c 
	${COMPILER} ${CFLAGS} charfreq.c -o charfreq]

%.o: %.c %.h  makefile
	${COMPILER} ${CFLAGS} $< -c 

clean: 
	rm -f *~ *.o ${EXES}

run:
	${EXES}