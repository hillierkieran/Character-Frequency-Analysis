CC = gcc
CFLAGS = -Wall -pedantic
DEPS = 	arg_parser.h \
		get_files.h \
		make_ring.h \
		task_manager.h \
		data_processor.h \
		result_printer.h

OBJ = 	char_count.o \
		arg_parser.o \
		get_files.o \
		make_ring.o \
		task_manager.o \
		data_processor.o \
		result_printer.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

char_count: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) -lm

.PHONY: clean

clean: 
	rm -f *~ *.o $(OBJ)

run:
	./char_count 5 text_files