CC = gcc
CFLAGS = -Wall -pedantic
DEPS = arg_parser.h get_files.h make_ring.h
OBJ = char_count.o arg_parser.o get_files.o make_ring.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

char_count: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) -lm

.PHONY: clean

clean: 
	rm -f *~ *.o $(OBJ)

run:
	./char_count 5 text_files