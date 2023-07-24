CC = gcc
CFLAGS = -Wall -pedantic
DEPS = 	arg_parser.h \
		file_handler.h \
		ring_builder.h \
		task_manager.h \
		data_processor.h \
		result_printer.h

OBJ = 	char_count.o \
		arg_parser.o \
		file_handler.o \
		ring_builder.o \
		task_manager.o \
		data_processor.o \
		result_printer.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

char_count: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) -lm

.PHONY: clean

clean: 
	rm -f *~ *.o $(OBJ) char_count

run:
	./char_count 5 text_files