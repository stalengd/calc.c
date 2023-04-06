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

struct ExpressionTreeBuilder {
    ExpressionNode* root;
    ExpressionNode* current;
} typedef ExpressionTreeBuilder;

struct Block {
    struct Block* prev;
    ExpressionNode* expression;
} typedef Block;


ExpressionNode* buildExpressionTree(Vector tokens);

#endif