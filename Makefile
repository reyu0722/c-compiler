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
	./main codegen.c > stage2/codegen.s
	./main error.c > stage2/error.s
	./main file.c > stage2/file.s
	./main main.c > stage2/main.s
	./main parse.c > stage2/parse.s
	./main preprocess.c > stage2/preprocess.s
	./main string.c > stage2/string.s
	./main tokenize.c > stage2/tokenize.s
	./main type.c > stage2/type.s
	$(CC) -c stage2/codegen.s -o stage2/codegen.o
	$(CC) -c stage2/error.s -o stage2/error.o
	$(CC) -c stage2/file.s -o stage2/file.o
	$(CC) -c stage2/main.s -o stage2/main.o
	$(CC) -c stage2/parse.s -o stage2/parse.o
	$(CC) -c stage2/preprocess.s -o stage2/preprocess.o
	$(CC) -c stage2/string.s -o stage2/string.o
	$(CC) -c stage2/tokenize.s -o stage2/tokenize.o
	$(CC) -c stage2/type.s -o stage2/type.o
	$(CC) -o main2 $(addprefix stage2/, $(OBJS)) $(LDFLAGS)

main3: main main2
	./main2 codegen.c > stage3/codegen.s
	./main2 error.c > stage3/error.s
	./main2 file.c > stage3/file.s
	./main2 main.c > stage3/main.s
	./main2 parse.c > stage3/parse.s
	./main2 preprocess.c > stage3/preprocess.s
	./main2 string.c > stage3/string.s
	./main2 tokenize.c > stage3/tokenize.s
	./main2 type.c > stage3/type.s
	$(CC) -c stage3/codegen.s -o stage3/codegen.o
	$(CC) -c stage3/error.s -o stage3/error.o
	$(CC) -c stage3/file.s -o stage3/file.o
	$(CC) -c stage3/main.s -o stage3/main.o
	$(CC) -c stage3/parse.s -o stage3/parse.o
	$(CC) -c stage3/preprocess.s -o stage3/preprocess.o
	$(CC) -c stage3/string.s -o stage3/string.o
	$(CC) -c stage3/tokenize.s -o stage3/tokenize.o
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
	diff stage2/codegen.s stage3/codegen.s
	@if [ $$? -ne 0 ]; then exit 1; fi
	diff stage2/error.s stage3/error.s
	@if [ $$? -ne 0 ]; then exit 1; fi
	diff stage2/file.s stage3/file.s
	@if [ $$? -ne 0 ]; then exit 1; fi
	diff stage2/main.s stage3/main.s
	@if [ $$? -ne 0 ]; then exit 1; fi
	diff stage2/parse.s stage3/parse.s
	@if [ $$? -ne 0 ]; then exit 1; fi
	diff stage2/preprocess.s stage3/preprocess.s
	@if [ $$? -ne 0 ]; then exit 1; fi
	diff stage2/string.s stage3/string.s
	@if [ $$? -ne 0 ]; then exit 1; fi
	diff stage2/tokenize.s stage3/tokenize.s
	@if [ $$? -ne 0 ]; then exit 1; fi
	diff stage2/type.s stage3/type.s
	@if [ $$? -ne 0 ]; then exit 1; fi

	@echo OK

.PHONY: test-all
test-all: test test2 test3 diff-test

.PHONY: clean
clean:
	rm -f main main2 main3 *.o **/*.o *~ tmp* **/*.s test/*.out peda*
