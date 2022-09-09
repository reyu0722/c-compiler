#include "stdbool.h"

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
void assert(bool flag);
