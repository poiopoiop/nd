
CC=g++
CFLAGS=-g -Wall \
	   -I../libevent/output/include/ \
	   -I../bd_libs/public/configure/output/include/ \
	   -I../bd_libs/public/mcpack/output/include/ \
	   -I../bd_libs/public/nshead/output/include/ \
	   -I../bd_libs/lib2-64/bsl/include/ \
	   -I../bd_libs/lib2-64/ullib/include/ \
	   -L../libevent/output/lib/ \
	   -L../bd_libs/public/configure/output/lib/ \
	   -L../bd_libs/public/mcpack/output/lib/ \
	   -L../bd_libs/public/nshead/output/ \
	   $(LEBOOK_CFLAGS)

EXAMPLE_BINARIES=ndsim

all: examples

examples: $(EXAMPLE_BINARIES)

ndsim: main.o file.o
	$(CC) $(CFLAGS) main.o file.o -o ndsim -levent -lpthread

.cpp.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *~
	rm -f *.o
	rm -f $(EXAMPLE_BINARIES)
