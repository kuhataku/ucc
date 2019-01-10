CC=clang
CFLAGS=-g -Wall -std=c11
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

ucc: $(OBJS)

$(OBJS): ucc.h

test: ucc
	./ucc -test
	./test.sh

format: ucc.c
	clang-format -i ucc.c

clean:
	rm -rf ucc *.o *- tmp*
