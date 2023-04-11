#ifndef PARSER_H
#define PARSER_H

#include "tokenizer.h"
#include "vector.h"

struct ExpressionNode {
    enum TokenType type;
    struct ExpressionNode* left;
    struct ExpressionNode* right;
} typedef ExpressionNode;
struct LiteralNode {
    enum TokenType type;
    float number;
} typedef LiteralNode;

struct BlockNode {
    Vector members;
    char* stringStart;
} typedef BlockNode;
struct BlockMember {
    enum TokenType type;
    union Data {
        TokenNode token;
        BlockNode block;
    } data;
} typedef BlockMember;

struct ParsingResult {
    bool isError;
    char* errorPosition;
    char* errorMessage;
} typedef ParsingResult;

#define PARSING_ERROR_POSITION_NONE -1 
#define PARSING_ERROR_POSITION_END -2

ParsingResult buildBlocksTree(Vector tokens, BlockNode* resultPointer);
ParsingResult buildExpressionTree(BlockNode block, ExpressionNode** resultPointer);
ExpressionNode* sortExpressionTree(ExpressionNode* root);
LiteralNode* createLiteralNode(float value);
ParsingResult appendOperatorToTree(ExpressionNode* node, BlockMember* token, ExpressionNode** resultPointer);
ParsingResult replaceBlockWithExpression(ExpressionNode* node);
int getOperationPriority(enum TokenType type);
bool isOperation(enum TokenType type);
void freeBlocksTree(BlockNode node);
void freeExpressionTree(ExpressionNode* node);

#endif