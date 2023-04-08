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

void printStringSpan(const char* start, int length) {
    size_t i;
    for (i = 0; i < (size_t)length; i++)
    {
        putchar(start[i]);
    }
}

void printBlocksTree(BlockNode rootBlock) {
    struct BlockNodeWithOffset {
        BlockNode block;
        int offset;
    };

    Vector blocks = vectorCreate(8, sizeof(struct BlockNodeWithOffset));

    struct BlockNodeWithOffset root = {
        .block = rootBlock,
        .offset = 0
    };
    vectorPush(&blocks, &root);
    
    for (size_t i = 0; i < blocks.size; i++)
    {
        struct BlockNodeWithOffset* current = vectorGet(&blocks, i);
        int cursorPos = current->offset;

        for (size_t j = 0; j < cursorPos; j++)
        {
            putchar(' ');
        }
        

        ConsoleGraphicsMode blockConsoleMode = (ConsoleGraphicsMode)(CONSOLE_BACKGROUND_BLACK + i % 7);
        setConsoleRendition(blockConsoleMode);

        for (size_t m = 0; m < current->block.members.size; m++)
        {
            BlockMember* member = vectorGet(&current->block.members, m);
            if (member->type == TOKEN_OPEN_BRACKET) {
                int blockIndex = blocks.size;
                setConsoleRendition((ConsoleGraphicsMode)(CONSOLE_BACKGROUND_BLACK + blockIndex % 7));
                putchar('(');
                putchar(')');
                setConsoleRendition(blockConsoleMode);
                struct BlockNodeWithOffset b = {
                    .block = member->data.block,
                    .offset = cursorPos
                };
                vectorPush(&blocks, &b);
                cursorPos += 2;
            }
            else {
                TokenNode token = member->data.token;
                printStringSpan(token.str, token.length);
                cursorPos += token.length;
            }
            if (m < current->block.members.size - 1) {
                putchar(' ');
                cursorPos += 1;
            }
        }

        setConsoleRendition(CONSOLE_DEFAULT);

        putchar('\n');
    }
    
    vectorFree(&blocks);
}