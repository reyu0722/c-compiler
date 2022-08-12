CFLAGS=-std=c11 -g -static

main: main.c

test: main
	./test.sh

.PHONY: test clean
clean:
	rm -f main *.o *~ tmp*
