CFLAGS=-std=c11 -g -static -Wall
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

main: $(OBJS)
	$(CC) -o main $(OBJS) $(LDFLAGS)

$(OBJS): header.h

.PHONY: test
test: main
	./test.sh

.PHONY: clean
clean:
	rm -f main *.o *~ tmp*
