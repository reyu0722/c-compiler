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

assert 0 "0;"
assert 42 "42;"
assert 21 "5+20-4;"
assert 41 " 12 + 34 - 5 ;"
assert 47 '5+6*7;'
assert 15 '5*(9-6);'
assert 4 '(3+5)/2;'
assert 10 '-10+20;'
assert 1 '1 == 1;'
assert 0 '1 == 2;'
assert 1 '1 != 2;'
assert 0 '1 <= 0;'
assert 1 '1 <= 1;'
assert 0 '1 < 1;'
assert 1 '1 < 2;'
assert 3 'a = 2; b = 3;'
assert 10 'a = 3 + 2; b = a * 2;'
assert 3 'a = 1; b = 2; c = a + b;'
assert 6 'foo = 3; bar = 2; baz = foo * bar;'
assert 10 'return 10;'
assert 3 'a = 1; b = 2; return a + b;'
assert 4 'a = 1; if (a == 1) a = a + 3; return a;'
assert 10 'a = 10; if (1 == 2) a = 2; return a;'
assert 4 'if (1 == 1) return 4; else return 5;'
assert 5 'if (1 == 2) return 4; else return 5;'
assert 6 'if (1 == 2) return 4; else if (2 == 3) return 5; else return 6;'
assert 10 'i = 0; while (i < 10) i = i + 1; return i;'
assert 3 'i = 0; for (i = 0; i < 3; i = i + 1) i = i; return i;'
assert 45 'i = 0; a = 0; for(i = 0; i < 10; i = i + 1) a = a + i; return a;'
assert 10 '{return 10;}'
assert 55 'a = 0; i = 0; while (i <= 10) {a = a + i; i = i + 1;} return a;'

echo OK