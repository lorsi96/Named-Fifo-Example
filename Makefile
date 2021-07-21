default: all

all : reader writer

# Reader
reader.o: $(shell pwd)/reader/reader.c
	gcc -c $(shell pwd)/reader/reader.c -o $(shell pwd)/build/reader.o

reader: reader.o
	gcc $(shell pwd)/build/reader.o -o $(shell pwd)/build/reader

# Writer
writer.o: $(shell pwd)/writer/writer.c
	gcc -c $(shell pwd)/writer/writer.c -o $(shell pwd)/build/writer.o

writer: writer.o
	gcc $(shell pwd)/build/writer.o -o $(shell pwd)/build/writer
