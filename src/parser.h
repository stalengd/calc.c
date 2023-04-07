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
    //struct BlockNode* prev;
    Vector members;
} typedef BlockNode;
struct BlockMember {
    enum TokenType type;
    union Data {
        TokenNode token;
        BlockNode block;
    } data;
} typedef BlockMember;

BlockNode buildBlocksTree(Vector tokens);
ExpressionNode* buildExpressionTree(BlockNode block);
ExpressionNode* sortExpressionTree(ExpressionNode* root);
LiteralNode* createLiteralNode(float value);
ExpressionNode* appendOperatorToTree(ExpressionNode* node, enum TokenType type);
void replaceBlockWithExpression(ExpressionNode* node);
int getOperationPriority(enum TokenType type);
bool isOperation(enum TokenType type);
void freeBlocksTree(BlockNode node);
void freeExpressionTree(ExpressionNode* node);

#endif