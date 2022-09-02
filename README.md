# c-compiler

C (subset) compiler written in C.


## Syntax

```ebnf
program    = external*
external   = declaration ("(" ("int" ident ("," "int" ident)*)? ")" "{" stmt* "}"
           | declaration ";"
stmt       = expr ";"
           | "if" "(" expr ")" stmt ("else" stmt)?
           | "while "(" expr ")" stmt
           | "for" "(" expr? ";" expr? ";" expr? ")" stmt
           | "{" stmt* "}"
           | "return" expr ";"
expr       = assign
assign     = equality ("=" assign)?
equality   = relational ("==" relational | "!=" relational)*
relational = add ("<" add | "<=" add | ">" add | ">=" add)*
add        = mul ("+" mul | "-" mul)*
mul        = unary ("*" unary | "/" unary)*
unary      = ("+" | "-" | "&" | "*" | "sizeof")? postfix
postfix    = primary ("[" expr "]")*
primary    = num | string | "(" expr ")"
           | declaration ("=" (assign | "{" expr_list? "}"))?
           | ident ("(" expr_list? ")")?

declaration = type_name nested_type
type_name   = "int" | "char"
nested_type = "*"* ("(" nested_type ")" | ident) ("[" num "]")*
expr_list   = expr ("," expr_list)?
```


## References

- [低レイヤを知りたい人のためのCコンパイラ作成入門](https://www.sigbus.info/compilerbook)
