#ifndef CALC_H
#define CALC_H

#include <stdio.h>
#include <strsafe.h>
#include <stdlib.h>
#include <stdbool.h>

#include "tokenizer.h"
#include "parser.h"
#include "fancy_console.h"
#include "formatter.h"

int calculate(char* rawExpression, float* resultPointer);
float evaluateExpressionNode(ExpressionNode* node);

#endif