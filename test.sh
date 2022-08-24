#!/bin/bash
assert() {
  expected="$1"
  input="$2"

  ./mycc "$input" > tmp.s
  cc -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

assert 0 "main(){0;}"
assert 42 "main() {42;}"
assert 21 "main() {5+20-4;}"
assert 41 "main() {12 + 34 - 5 ;}"
assert 47 "main() {5+6*7;}"
assert 15 "main() {5*(9-6);}"
assert 4 "main() {(3+5)/2;}"
assert 10 "main() {-10+20;}"
assert 1 "main() {1 == 1;}"
assert 0 "main() {1 == 2;}"
assert 1 "main() {1 != 2;}"
assert 0 "main() {1 <= 0;}"
assert 1 "main() {1 <= 1;}"
assert 0 "main() {1 < 1;}"
assert 1 "main() {1 < 2;}"
assert 3 "main() {a = 2; b = 3;}"
assert 10 "main() {a = 3 + 2; b = a * 2;}"
assert 3 "main() {a = 1; b = 2; c = a + b;}"
assert 6 "main() {foo = 3; bar = 2; baz = foo * bar;}"
assert 10 "main() {return 10;}"
assert 3 "main() {a = 1; b = 2; return a + b;}"
assert 4 "main() {a = 1; if (a == 1) a = a + 3; return a;}"
assert 10 "main() {a = 10; if (1 == 2) a = 2; return a;}"
assert 4 "main() {if (1 == 1) return 4; else return 5;}"
assert 5 "main() {if (1 == 2) return 4; else return 5;}"
assert 6 "main() {if (1 == 2) return 4; else if (2 == 3) return 5; else return 6;}"
assert 10 "main() {i = 0; while (i < 10) i = i + 1; return i;}"
assert 3 "main() {i = 0; for (i = 0; i < 3; i = i + 1) i = i; return i;}"
assert 45 "main() {i = 0; a = 0; for(i = 0; i < 10; i = i + 1) a = a + i; return a;}"
assert 10 "main() {{return 10;}}"
assert 55 "main() {a = 0; i = 0; while (i <= 10) {a = a + i; i = i + 1;} return a;}"
assert 10 "foo() {return 10;} main() {return foo();}"
assert 2 "foo() {a = 1; return 0;} main() {a = 2; b = foo(); return a;}"
assert 5 "foo(a, b) {return a+b;} main() {return foo(2, 3);}"
assert 55 "fib(i) {if (i == 1) return 1; if (i == 2) return 1; return fib(i - 1) + fib(i - 2);} main() {return fib(10);}"
assert 21 "bar(a, b, c, d, e, f) {return a + b + c + d + e + f;} main() {return bar(1, 2, 3, 4, 5, 6);}"
assert 8 "main() { a = 8; b = &a; return *b;}"

echo OK