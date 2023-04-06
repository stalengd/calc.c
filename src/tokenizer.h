#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <stdbool.h>
#include <stdio.h>
#include "vector.h"

enum TokenType {
    TOKEN_NONE,
    TOKEN_NUMBER,
    TOKEN_OPEN_BRACKET,
    TOKEN_CLOSE_BRACKET,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MULTIPLY,
    TOKEN_DIVIDE
};

struct TokenNode {
    enum TokenType type;
    int length;
    const char* str;
} typedef TokenNode;

char * getline(void);
bool isDigit(char c);
bool isNumberChar(char c);
bool isSpace(char c);
enum TokenType getTokenType(char c);
void printTokenFromPointer(void* p);
int getTokenTypeLimit(enum TokenType type);
Vector tokenize(const char* str);

#endif