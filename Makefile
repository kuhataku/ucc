CC=gcc

ucc: ucc.c

test: ucc
	./test.sh

clean:
	rm -rf ucc *.o *- tmp*
