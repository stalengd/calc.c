#include "calc.h"

int calculate(char* rawExpression, float* resultPointer) {
    Vector tokens = tokenize(rawExpression);
    int exitCode = 0;
    
    BlockNode rootBlock;
    memset(&rootBlock, 0, sizeof(BlockNode));
    ExpressionNode* expRoot = NULL;

    if (tokens.size == 0) {
        exitCode = 1;
        goto end;
    }

    ParsingResult result = buildBlocksTree(tokens, &rootBlock);
    if (result.isError) {
        exitCode = 1;
        goto end;
    }
    
    result = buildExpressionTree(rootBlock, &expRoot);
        if (result.isError) {
        exitCode = 1;
        goto end;
    }

    float resultValue = evaluateExpressionNode(expRoot);
    *resultPointer = resultValue;

    end:
    vectorFree(&tokens);
    freeBlocksTree(rootBlock);
    freeExpressionTree(expRoot);
    return exitCode;
}

float evaluateExpressionNode(ExpressionNode* node) {
    if (node->type == TOKEN_NUMBER) 
        return ((LiteralNode*)node)->number;

    float leftVal = evaluateExpressionNode(node->left);
    float rightVal = evaluateExpressionNode(node->right);

    switch (node->type)
    {
    case TOKEN_PLUS: return leftVal + rightVal;
    case TOKEN_MINUS: return leftVal - rightVal;
    case TOKEN_MULTIPLY: return leftVal * rightVal;
    case TOKEN_DIVIDE: return leftVal / rightVal;
    
    default:
        return 0; // TODO: error
    }
}