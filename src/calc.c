#include "calc.h"

int calculate(char* rawExpression, float* resultPointer) {
    Vector tokens = tokenize(rawExpression);
    
    if (tokens.size == 0) {
        return 1;
    }

    ExpressionNode* expRoot = buildExpressionTree(tokens);
    float result = evaluateExpressionNode(expRoot);
    *resultPointer = result;
    vectorFree(&tokens);
    return 0;
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


