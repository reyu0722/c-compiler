# mycc

C (subset) compiler written in C.


## Syntax

```ebnf
program    = function*
function   = "int" "(" ("int" ident ("," "int" ident)*)? ")" "{" stmt* "}"
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
primary    = num | "(" expr ")"
           | "int" ident ("[" num "]")*
           | ident ("(" (ident ("," ident)*)? ")")?
```


## References

- [低レイヤを知りたい人のためのCコンパイラ作成入門](https://www.sigbus.info/compilerbook)
