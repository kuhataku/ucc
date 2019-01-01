CC=gcc

ucc: ucc.c

test: ucc
	./test.sh

format: ucc.c
	clang-format -i ucc.c

clean:
	rm -rf ucc *.o *- tmp*
