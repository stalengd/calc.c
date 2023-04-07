#include <stdio.h>
#include <strsafe.h>
#include <stdlib.h>
#include <stdbool.h>

#include "tokenizer.h"
#include "parser.h"
#include "fancy_console.h"
#include "formatter.h"
#include "calc.h"


int main(int argc, char** args) {
    printf("Expression: ");
    char* input = getline();

    Vector tokens = tokenize(input);

    printWithFormatting(input, tokens);
    
    if (tokens.size == 0) {
        printf_s("Input is empty \n");
        return 1;
    }

    BlockNode rootBlock = buildBlocksTree(tokens);
    ExpressionNode* expRoot = buildExpressionTree(rootBlock);

    float result = evaluateExpressionNode(expRoot);
    
    printf_s("= %f\n", result);

    vectorFree(&tokens);
    freeBlocksTree(rootBlock);
    freeExpressionTree(expRoot);

    return 0;
}