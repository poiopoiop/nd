
CC=gcc
CFLAGS=-g -Wall \
	   -I../libevent/output/include/ \
	   -I../baidu_libs/public/configure/output/include/ \
	   -I../baidu_libs/public/mcpack/output/include/ \
	   -I../baidu_libs/public/nshead/output/include/ \
	   -L../libevent/output/lib/ \
	   -L../baidu_libs/public/configure/output/lib/ \
	   -L../baidu_libs/public/mcpack/output/lib/ \
	   -L../baidu_libs/public/nshead/output/ \
	   $(LEBOOK_CFLAGS)

EXAMPLE_BINARIES=ndsim

all: examples

examples: $(EXAMPLE_BINARIES)

ndsim: main.o
	$(CC) $(CFLAGS) main.o -o ndsim -levent

.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *~
	rm -f *.o
	rm -f $(EXAMPLE_BINARIES)
