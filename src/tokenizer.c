#include "tokenizer.h"

char * getline(void) {
    char * line = malloc(100), * linep = line;
    size_t lenmax = 100, len = lenmax;
    int c;
    if(line == NULL)
        return NULL;
    for(;;) {
        c = (char)fgetc(stdin);
        if(c == EOF)
            break;
        if(--len == 0) {
            len = lenmax;
            char * linen = realloc(linep, lenmax *= 2);
            if(linen == NULL) {
                free(linep);
                return NULL;
            }
            line = linen + (line - linep);
            linep = linen;
        }
        if((char)(*line++ = c) == '\n')
            break;
    }
    *line = '\0';
    return linep;
}

bool isDigit(char c) {
    return c >= '0' && c <= '9';
}

bool isNumberChar(char c) {
    return isDigit(c) || c == '.';
}

bool isSpace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

enum TokenType getTokenType(char c) {
    if (isNumberChar(c))
        return TOKEN_NUMBER;

    switch (c)
    {
        case '(': return TOKEN_OPEN_BRACKET;
        case ')': return TOKEN_CLOSE_BRACKET;
        case '+': return TOKEN_PLUS;
        case '-': return TOKEN_MINUS;
        case '*': return TOKEN_MULTIPLY;
        case '/': return TOKEN_DIVIDE;

        default: return TOKEN_NONE;
    }
}

void printTokenFromPointer(void* p) {
    TokenNode* token = p;

    printf("[%d] ", token->type);

    size_t i;
    for (i = 0; i < (size_t)token->length; i++)
    {
        putchar(token->str[i]);
    }
    
    putchar('\n');
}

int getTokenTypeLimit(enum TokenType type) {
    switch (type)
    {
    case TOKEN_OPEN_BRACKET:
    case TOKEN_CLOSE_BRACKET:
    case TOKEN_PLUS:
    case TOKEN_MINUS:
    case TOKEN_MULTIPLY:
    case TOKEN_DIVIDE:
        return 1;
    
    default:
        return 99999999;
    }
}

Vector tokenize(const char* str) {
    int charIndex = 0;
    char currentChar = str[charIndex];

    Vector vec = vectorCreate(16, sizeof(TokenNode));

    TokenNode* currentToken = NULL;
    enum TokenType currentType = TOKEN_NONE;
    while (currentChar != '\0')
    {
        enum TokenType t = getTokenType(currentChar);

        if (t == TOKEN_NONE) {
            currentType = t;
            goto next;
        }

        if (currentToken == NULL) {
            TokenNode newToken = {
                .length = 0,
                .type = t,
                .str = str + charIndex
            };
            vectorPush(&vec, &newToken);
            currentToken = vectorLast(&vec);
            currentType = currentToken->type;
        }

        if (t != currentType ||
            currentToken->length >= getTokenTypeLimit(currentType)) {
            TokenNode newToken = {
                .length = 1,
                .type = t,
                .str = str + charIndex
            };
            vectorPush(&vec, &newToken);
            currentToken = vectorLast(&vec);
            currentType = currentToken->type;
        }
        else {
            currentToken->length += 1;
        }


        next:
        charIndex++;
        currentChar = str[charIndex];
    }

    return vec;
}
