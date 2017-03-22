#!/bin/sh
g++ -g -Wall -I ../bd_libs/public/configure/output/include/ -I ../bd_libs/lib2-64/bsl/include/ -I ../bd_libs/lib2-64/ullib/include/ -I ../libevent/output/include/ -c event.cpp -o event.o
g++ -g -Wall -I ../bd_libs/public/configure/output/include/ -I ../bd_libs/lib2-64/bsl/include/ -I ../bd_libs/lib2-64/ullib/include/ -c log.cpp -o log.o
g++ -g -Wall -I ../bd_libs/public/configure/output/include/ -I ../bd_libs/lib2-64/bsl/include/ -I ../bd_libs/lib2-64/ullib/include/ -c test.cpp -o test.o
g++ -g -Wall -I ../bd_libs/public/configure/output/include/ -I ../bd_libs/lib2-64/bsl/include/ -I ../bd_libs/lib2-64/ullib/include/ -c file.cpp -o file.o 
g++ -g -Wall -I ../bd_libs/public/configure/output/include/ -I ../bd_libs/lib2-64/bsl/include/ -I ../bd_libs/lib2-64/ullib/include/ -c main.cpp -o main.o 
#g++ -g -Wall -I ../bd_libs/public/configure/output/include/ -I ../bd_libs/lib2-64/bsl/include/ -I ../bd_libs/lib2-64/ullib/include/ -I ../libevent/output/include/ -L ../bd_libs/public/configure/output/lib/ -L ../bd_libs/lib2-64/bsl/lib/ -L ../bd_libs/lib2-64/ullib/lib/ -L ../libevent/output/lib/ event.o log.o test.o -o test -lpthread -levent

g++ -g -Wall \
    -I ../bd_libs/public/configure/output/include/ \
    -I ../bd_libs/lib2-64/bsl/include/ \
    -I ../bd_libs/lib2-64/ullib/include/ \
    -I ../libevent/output/include/ \
    event.o \
    log.o\
    file.o\
    main.o\
    -Xlinker "-(" \
    ../libevent/output/lib/libevent.a \
    ../bd_libs/lib2-64/bsl/lib/libbsl.a \
    ../bd_libs/lib2-64/bsl/lib/libbsl_ResourcePool.a \
    ../bd_libs/lib2-64/bsl/lib/libbsl_archive.a \
    ../bd_libs/lib2-64/bsl/lib/libbsl_buffer.a \
    ../bd_libs/lib2-64/bsl/lib/libbsl_check_cast.a \
    ../bd_libs/lib2-64/bsl/lib/libbsl_exception.a  \
    ../bd_libs/lib2-64/bsl/lib/libbsl_pool.a \
    ../bd_libs/lib2-64/bsl/lib/libbsl_utils.a \
    ../bd_libs/lib2-64/bsl/lib/libbsl_var.a \
    ../bd_libs/lib2-64/bsl/lib/libbsl_var_implement.a \
    ../bd_libs/lib2-64/bsl/lib/libbsl_var_utils.a \
    ../bd_libs/lib2-64/ullib/lib/libullib.a \
    -lpthread -rdynamic -Xlinker "-)" -o test
