#include "formatter.h"

void printWithFormatting(char* originalExpression, Vector tokens) {
    size_t i = 0;
    char c = originalExpression[i];
    size_t tokenIndex = 0;
    while (c != '\0')
    {
        TokenNode* token = vectorGet(&tokens, tokenIndex);
        if (token != NULL && token->str == originalExpression + i) {
            i += token->length;
            tokenIndex++;
            printToken(token);
            setConsoleRendition(CONSOLE_DEFAULT);
        }
        else {
            i++;
            putchar(c);
        }
        c = originalExpression[i];
    }
    //putchar('\n');
}

void printToken(TokenNode* token) {
    setConsoleRendition(getTokenColor(token->type));

    size_t i;
    for (i = 0; i < (size_t)token->length; i++)
    {
        putchar(token->str[i]);
    }
}

ConsoleGraphicsMode getTokenColor(enum TokenType type) {
    switch (type)
    {
    case TOKEN_OPEN_BRACKET:
    case TOKEN_CLOSE_BRACKET: return CONSOLE_FOREGROUND_YELLOW;
    case TOKEN_PLUS:
    case TOKEN_MINUS:
    case TOKEN_MULTIPLY:
    case TOKEN_DIVIDE: return CONSOLE_FOREGROUND_RED;
    
    default: return CONSOLE_DEFAULT;
    }
}