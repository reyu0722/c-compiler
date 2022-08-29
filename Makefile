CFLAGS=-std=c11 -g -static -Wall
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

TEST_SRCS=$(wildcard test/*.c)
TESTS=$(TEST_SRCS:.c=.out)

main: $(OBJS)
	$(CC) -o main $(OBJS) $(LDFLAGS)

$(OBJS): header.h

test/%.out: main $(TEST_SRCS)
	./main test/$*.c > test/$*.s
	$(CC) -o test/$*.out test/$*.s -xc test/common

.PHONY: test
test: $(TESTS)
	@for i in $^; do echo $$i; ./$$i || exit 1; done
	@echo OK


.PHONY: clean
clean:
	rm -f main *.o *~ tmp* test/*.s test/*.out
