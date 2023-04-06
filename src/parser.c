#include "parser.h"

ExpressionNode* buildExpressionTree(Vector tokens) {
    ExpressionNode* expRoot = malloc(sizeof(ExpressionNode));
    expRoot->type = TOKEN_NONE;
    expRoot->left = NULL;
    expRoot->right = NULL;

    Block* currentBlock = NULL;

    ExpressionNode* node = expRoot;
    char buffer[256];
    TokenNode* token = vectorGet(&tokens, 0);
    int tokenIndex = 0;
    while (token != NULL)
    {
        switch (token->type)
        {
        case TOKEN_NUMBER: {
            LiteralNode* numberNode = malloc(sizeof(LiteralNode));
            numberNode->type = TOKEN_NUMBER;
            numberNode->number = strtof(token->str, NULL);
            if (node->left == NULL) {
                node->left = (ExpressionNode*)numberNode;
            }
            else {
                node->right = (ExpressionNode*)numberNode;
            }
            break;
        }
        case TOKEN_PLUS: {
            if (node->right == NULL) {
                node->type = TOKEN_PLUS;
            }
            else {
                ExpressionNode* newNode = malloc(sizeof(ExpressionNode));
                newNode->type = TOKEN_PLUS;
                newNode->left = node->right;
                newNode->right = NULL;
                node->right = newNode;
                node = newNode;
            }
            break;
        }
        case TOKEN_MINUS: {
            TokenNode* nextToken = vectorGet(&tokens, tokenIndex + 1);
            if (nextToken == NULL) {
                // TODO: error
            }

            if (node->right == NULL) {
                if (nextToken->type == TOKEN_OPEN_BRACKET) {
                    node->type = TOKEN_MINUS;
                }
                else {
                    ExpressionNode* newNode = malloc(sizeof(ExpressionNode));
                    newNode->type = TOKEN_MINUS;
                    newNode->left = node->left;
                    newNode->right = NULL;
                    node->left = newNode;
                    if (expRoot == node) 
                        expRoot = newNode;
                    node = newNode;
                }
            }
            else {
                if (nextToken->type == TOKEN_OPEN_BRACKET) {
                    ExpressionNode* newNode = malloc(sizeof(ExpressionNode));
                    newNode->type = TOKEN_MINUS;
                    newNode->left = node->right;
                    newNode->right = NULL;
                    node->right = newNode;
                    if (expRoot == node) 
                        expRoot = newNode;
                    node = newNode;
                }
                else {
                    ExpressionNode* newNode = malloc(sizeof(ExpressionNode));
                    newNode->type = TOKEN_MINUS;
                    newNode->left = node;
                    newNode->right = NULL;
                    node->left = newNode;
                    if (expRoot == node) 
                        expRoot = newNode;
                    node = newNode;
                }
            }
            break;
        }
        case TOKEN_OPEN_BRACKET: {
            Block* prev = currentBlock;
            currentBlock = malloc(sizeof(currentBlock));
            currentBlock->prev = prev;
            currentBlock->expression = node;

            if (node->left != NULL) {
                ExpressionNode* newNode = malloc(sizeof(ExpressionNode));
                newNode->type = TOKEN_NONE;
                newNode->left = NULL;
                newNode->right = NULL;
                node->right = newNode;
                node = newNode;
            }

            break;
        }
        case TOKEN_CLOSE_BRACKET: {
            // TODO: throw error if no opening bracket
            node = currentBlock->expression;
            currentBlock = currentBlock->prev;

            break;
        }
        
        default:
            break;
        }

        tokenIndex++;
        token = vectorGet(&tokens, tokenIndex);
    }

    return expRoot;
}