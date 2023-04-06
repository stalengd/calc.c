#ifndef FORMATTER_H
#define FORMATTER_H

#include "vector.h"
#include "tokenizer.h"
#include "fancy_console.h"

void printWithFormatting(char* originalExpression, Vector tokens);
void printToken(TokenNode* token);
ConsoleGraphicsMode getTokenColor(enum TokenType type);

#endif