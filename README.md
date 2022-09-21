# c-compiler

C (subset) compiler written in C.


## Syntax

```ebnf
program    = external*
external   = declaration "(" ("int" ident ("," "int" ident)*)? | "void" ")" (";" | "{" stmt* "}")
           | declaration ";"
           | "enum" ident "{" ident ("," ident)* "}" ";"
           | ("struct" | "union") ident? "{" declaration ("," declaration)* "}" ";"
           | ("struct" | "union") "{" declaration ("," declaration)* "}" ";"
           | "typedef" type_name ident ";"
           | preprocessor
label_stmt = label? stmt
label      = "case" expr ":"
           | "default" ":"
stmt       = expr ";"
           | "if" "(" expr ")" stmt ("else" stmt)?
           | "switch" "(" expr ")" stmt
           | "while "(" expr ")" stmt
           | "for" "(" expr? ";" expr? ";" expr? ")" stmt
           | "{" stmt* "}"
           | "return" expr? ";"
           | "break" ";"
           | "contunie" ";"
expr       = assign
assign     = equality (("=" | "+=" | "-=" | "*=" | "/=") assign)?
equality   = relational (("&&" | "||" | "==" | "!=") relational)*
relational = add ("<" add | "<=" add | ">" add | ">=" add)*
add        = mul ("+" mul | "-" mul)*
mul        = unary ("*" unary | "/" unary)*
unary      = ("+" | "-" | "&" | "*" | "sizeof" | "++" | "--")? postfix
postfix    = primary ("[" expr "]")*
           | primary ("." | "->") ident
           | primary ("++" | "--")
primary    = num | string | "(" expr ")"
           | declaration ("=" (assign | "{" expr_list? "}"))?
           | ident ("(" expr_list? ")")?

preprocessor = "#" "include" string

declaration = type_name nested_type
type_name   = "int" | "char" | "struct" ident
nested_type = "*"* ("(" nested_type ")" | ident) ("[" num "]")*
expr_list   = expr ("," expr_list)?
```


## References

- [N1570](http://port70.net/~nsz/c/c11/n1570.html) (C11 final working draft)
- [System V Application Binary Interface AMD64 Architecture Processor Supplement](https://www.uclibc.org/docs/psABI-x86_64.pdf)
- [低レイヤを知りたい人のためのCコンパイラ作成入門](https://www.sigbus.info/compilerbook)
- [C reference - cppreference.com](https://en.cppreference.com/w/c)
- [2022年セキュリティ・キャンプL3ゼミ(Cコンパイラ班)ログ](https://sozysozbot.github.io/seccamp-2022-c-compiler-seminar/)
- https://github.com/season1618/books/blob/main/c-compiler/index.md
- [rui314/chibicc: A small C compiler](https://github.com/rui314/chibicc)
- [hsjoihs/c-compiler: seccamp2018 c compiler](https://github.com/hsjoihs/c-compiler)
- [Imperi13/my-cc](https://github.com/Imperi13/my-cc)
- [season1618/c-compiler](https://github.com/season1618/c-compiler)
