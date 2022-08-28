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

assert 0 "int main(){0;}"
assert 42 "int main() {42;}"
assert 21 "int main() {5+20-4;}"
assert 41 "int main() {12 + 34 - 5 ;}"
assert 47 "int main() {5+6*7;}"
assert 15 "int main() {5*(9-6);}"
assert 4 "int main() {(3+5)/2;}"
assert 10 "int main() {-10+20;}"
assert 1 "int main() {1 == 1;}"
assert 0 "int main() {1 == 2;}"
assert 1 "int main() {1 != 2;}"
assert 0 "int main() {1 <= 0;}"
assert 1 "int main() {1 <= 1;}"
assert 0 "int main() {1 < 1;}"
assert 1 "int main() {1 < 2;}"
assert 3 "int main() {int a; int b; a = 2; b = 3;}"
assert 10 "int main() {int a; int b; a = 3 + 2; b = a * 2;}"
assert 3 "int main() {int a; int b; int c; a = 1; b = 2; c = a + b;}"
assert 6 "int main() {int foo; int bar; int baz; foo = 3; bar = 2; baz = foo * bar;}"
assert 10 "int main() {return 10;}"
assert 3 "int main() {int a; int b; a = 1; b = 2; return a + b;}"
assert 4 "int main() {int a; a = 1; if (a == 1) a = a + 3; return a;}"
assert 10 "int main() {int a; a = 10; if (1 == 2) a = 2; return a;}"
assert 4 "int main() {if (1 == 1) return 4; else return 5;}"
assert 5 "int main() {if (1 == 2) return 4; else return 5;}"
assert 6 "int main() {if (1 == 2) return 4; else if (2 == 3) return 5; else return 6;}"
assert 10 "int main() {int i; i = 0; while (i < 10) i = i + 1; return i;}"
assert 3 "int main() {int i; i = 0; for (i = 0; i < 3; i = i + 1) i = i; return i;}"
assert 45 "int main() {int i; int a; i = 0; a = 0; for(i = 0; i < 10; i = i + 1) a = a + i; return a;}"
assert 10 "int main() {{return 10;}}"
assert 55 "int main() {int a; int i; a = 0; i = 0; while (i <= 10) {a = a + i; i = i + 1;} return a;}"
assert 10 "int foo() {return 10;} int main() {return foo();}"
assert 2 "int foo() {int a; a = 1; return 0;} int main() {int a; int b; a = 2; b = foo(); return a;}"
assert 5 "int foo(int a, int b) {return a+b;} int main() {return foo(2, 3);}"
assert 55 "int fib(int i) {if (i == 1) return 1; if (i == 2) return 1; return fib(i - 1) + fib(i - 2);} int main() {return fib(10);}"
assert 21 "int bar(int a, int b, int c, int d, int e, int f) {return a + b + c + d + e + f;} int main() {return bar(1, 2, 3, 4, 5, 6);}"
assert 8 "int main() { int a; int *b; a = 8; b = &a; return *b;}"
assert 10 "int main() {int x; int *y; x = 8; y = &x; *y = *y + 2; return x;}"
# assert 4 "int main() {return sizeof(0);}"
assert 8 "int main() {int *a; return sizeof a;}"
# assert 4 "int main() {int a; return sizeof(a + 1);}"
assert 24 "int main() {int a[3]; return sizeof a;}"
assert 10 "int main() {int a[2]; *(a + 1) = 10; return *(a + 1);}"
assert 3 "int main() {int a[2]; int b; *(a + 1) = 3; b = 4; return *(a + 1);}"
assert 4 "int main() {int arr[10]; int i; for (i=0; i<10; i = i + 1) arr[i] = i; return arr[4];}"

echo OK