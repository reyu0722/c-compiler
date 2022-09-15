#include "tokenize.h"

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
void error_at_token(Token *tok, char *fmt, ...);
void error_at_here(char *fmt, ...);
void assert(bool flag);
