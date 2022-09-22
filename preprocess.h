#include "tokenize.h"
#include "string.h"

typedef struct Macro Macro;
struct Macro
{
	String *ident;
	Token *replace;
};

Token *preprocess(Token *tok);
