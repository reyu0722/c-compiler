# mycc

C (subset) compiler written in C.


## Syntax

```ebnf
program    = external*
external   = type_name "*"* ident ("(" ("int" ident ("," "int" ident)*)? ")" "{" stmt* "}"
           | type_name "*"* ident ("[" num "]")* ";"
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
           | type_name "*"* ident ("[" num "]")*
           | ident ("(" (ident ("," ident)*)? ")")?

type_name  = "int" | "char"
```


## References

- [低レイヤを知りたい人のためのCコンパイラ作成入門](https://www.sigbus.info/compilerbook)
