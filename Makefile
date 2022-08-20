CFLAGS=-std=c11 -g -static -Wall
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

mycc: $(OBJS)
	$(CC) -o mycc $(OBJS) $(LDFLAGS)

$(OBJS): mycc.h

.PHONY: test
test: mycc
	./test.sh

.PHONY: clean
clean:
	rm -f mycc *.o *~ tmp*
