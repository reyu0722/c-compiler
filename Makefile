CFLAGS=-std=c11 -g -static -Wall
SRCS=$(wildcard *.c)
HEADERS=$(wildcard *.h)
OBJS=$(SRCS:.c=.o)

TEST_SRCS=$(wildcard test/*.c)
TESTS=$(TEST_SRCS:.c=.out)
TESTS2=$(TEST_SRCS:.c=.2.out)
TESTS3=$(TEST_SRCS:.c=.3.out)

main: $(OBJS)
	$(CC) -o main $(OBJS) $(LDFLAGS)

$(OBJS): $(HEADERS)

main2: main
	cp $(OBJS) stage2/
	./main string.c > stage2/string.s
	./main type.c > stage2/type.s
	$(CC) -c stage2/string.s -o stage2/string.o
	$(CC) -c stage2/type.s -o stage2/type.o
	$(CC) -o main2 $(addprefix stage2/, $(OBJS)) $(LDFLAGS)

main3: main main2
	cp $(OBJS) stage3/
	./main2 string.c > stage3/string.s
	./main2 type.c > stage3/type.s
	$(CC) -c stage3/string.s -o stage3/string.o
	$(CC) -c stage3/type.s -o stage3/type.o
	$(CC) -o main3 $(addprefix stage3/, $(OBJS)) $(LDFLAGS)

test/%.out: main $(TEST_SRCS)
	./main test/$*.c > test/$*.s
	$(CC) -o test/$*.out test/$*.s -xc test/common

.PHONY: test
test: $(TESTS)
	@for i in $^; do echo $$i; ./$$i || exit 1; done
	@echo OK

test/%.2.out: main2 $(TEST_SRCS)
	./main2 test/$*.c > test/$*.2.s
	$(CC) -o test/$*.2.out test/$*.2.s -xc test/common

.PHONY: test2
test2: $(TESTS2)
	@for i in $^; do echo $$i; ./$$i || exit 1; done
	@echo OK

test/%.3.out: main3 $(TEST_SRCS)
	./main2 test/$*.c > test/$*.3.s
	$(CC) -o test/$*.3.out test/$*.3.s -xc test/common

.PHONY: test3
test3: $(TESTS3)
	@for i in $^; do echo $$i; ./$$i || exit 1; done
	@echo OK

.PHONY: diff-test
diff-test: main2 main3
	diff stage2/string.s stage3/string.s
	@if [ $$? -ne 0 ]; then exit 1; fi
	diff stage2/type.s stage3/type.s
	@if [ $$? -ne 0 ]; then exit 1; fi
	@echo OK

.PHONY: test-all
test-all: test test2 test3 diff-test

.PHONY: clean
clean:
	rm -f main **/*.o *~ tmp* **/*.s test/*.out
