
CC=g++
CFLAGS=-g -Wall \
	   -I../libevent/output/include/ \
	   -I../baidu_libs/public/configure/output/include/ \
	   -I../baidu_libs/public/mcpack/output/include/ \
	   -I../baidu_libs/public/nshead/output/include/ \
	   -I../baidu_libs/lib2-64/bsl/include/ \
	   -I../baidu_libs/lib2-64/ullib/include/ \
	   -L../libevent/output/lib/ \
	   -L../baidu_libs/public/configure/output/lib/ \
	   -L../baidu_libs/public/mcpack/output/lib/ \
	   -L../baidu_libs/public/nshead/output/ \
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
