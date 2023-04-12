#include <stdio.h>
#include <strsafe.h>
#include <stdlib.h>
#include <stdbool.h>

#include "tokenizer.h"
#include "parser.h"
#include "fancy_console.h"
#include "command_line.h"
#include "formatter.h"
#include "calc.h"


int oneShotExecution(CliArguments cli);
int givenExpressionExecution(CliArguments cli, char* expression);
int interactiveExecution(CliArguments cli);
int helpExecution(CliArguments cli);
int promptAndCalculate(bool verbose, int expressionOffset);
int calculateExpression(char* expression, bool verbose, int expressionOffset);
void printParsingError(ParsingResult result, char* rawExpression, int expressionOffset);


const CliCommand cmdHelp = {
    .name = "-h --help",
    .desc = "Print help and exit",
    .defaultValue = false
};

const CliCommand cmdVerbose = {
    .name = "-v --verbose",
    .desc = "Verbose mode",
    .defaultValue = false
};

const CliCommand cmdInteractive = {
    .name = "-i --interactive",
    .desc = "Interactive mode",
    .defaultValue = false
};

const CliCommand cmdExpression = {
    .name = "-e --expression",
    .desc = "Calculate given expression and exit",
    .defaultValue = NULL
};

const CliCommand* commands[] = {
    &cmdHelp,
    &cmdVerbose,
    &cmdInteractive,
    &cmdExpression
};

int main(int argc, char** args) {

    CliArguments cli = {
        .argc = argc,
        .args = args
    };

    bool helpRequested = cliParseFlag(cli, cmdHelp);
    
    if (helpRequested) {
        return helpExecution(cli);
    }

    char* givenExpression = cliParseString(cli, cmdExpression);
    if (givenExpression != NULL) {
        return givenExpressionExecution(cli, givenExpression);
    }
    if (cliParseFlag(cli, cmdInteractive)) {
        return interactiveExecution(cli);
    }
    return oneShotExecution(cli);
}

int oneShotExecution(CliArguments cli) {
    bool verbose = cliParseFlag(cli, cmdVerbose);

    printf("Expression: ");
    return promptAndCalculate(verbose, 12);
}

int givenExpressionExecution(CliArguments cli, char* expression) {
    bool verbose = cliParseFlag(cli, cmdVerbose);

    return calculateExpression(expression, verbose, 0);
}

int interactiveExecution(CliArguments cli) {
    bool verbose = cliParseFlag(cli, cmdVerbose);

    printf("Interactive mode: enter expressions, Ctrl+C to exit\n\n");

    while (true)
    {
        printf("> ");
        int code = promptAndCalculate(verbose, 2);
        if (code != 0) return code;
    }

    return 0;
}

int helpExecution(CliArguments cli) {
    printf("calc.c - Simple calculator\n");

    printf("Usage:\n\tcalc.exe [Options]\n\n");

    printf("Options:\n\n");

    size_t commandsCount = sizeof(commands) / sizeof(CliCommand*);
    for (size_t i = 0; i < commandsCount; i++)
    {
        printf("\t%s \t- %s\n", commands[i]->name, commands[i]->desc);
    }
    
    putchar('\n');

    return 0;
}


int promptAndCalculate(bool verbose, int expressionOffset) {
    char* input = getline();

    int r = calculateExpression(input, verbose, expressionOffset);

    free(input);
    return r;
}

int calculateExpression(char* expression, bool verbose, int expressionOffset) {
    Vector tokens = tokenize(expression);
    BlockNode rootBlock;
    memset(&rootBlock, 0, sizeof(BlockNode));
    ExpressionNode* expRoot = NULL;

    if (verbose)
        printWithFormatting(expression, tokens);
    
    if (tokens.size == 0) {
        printf_s("x Input is empty\n\n");
        goto end;
    }

    ParsingResult result = buildBlocksTree(tokens, &rootBlock);
    if (result.isError) {
        printParsingError(result, expression, expressionOffset);
        goto end;
    }

    if (verbose)
        printBlocksTree(rootBlock);

    result = buildExpressionTree(rootBlock, &expRoot);
    if (result.isError) {
        printParsingError(result, expression, expressionOffset);
        goto end;
    }

    float resultValue = evaluateExpressionNode(expRoot);
    
    printf_s("= %f\n\n", resultValue);

    end:
    vectorFree(&tokens);
    freeBlocksTree(rootBlock);
    freeExpressionTree(expRoot);
    return 0;
}

void printParsingError(ParsingResult result, char* rawExpression, int expressionOffset) {
    if (result.errorPosition != PARSING_ERROR_POSITION_NONE) {
        if (result.errorPosition == PARSING_ERROR_POSITION_END) {
            result.errorPosition = rawExpression + strlen(rawExpression) - 1;
        }
        int errorCursorPos = expressionOffset + (result.errorPosition - rawExpression);
        for (size_t i = 0; i < errorCursorPos; i++)
        {
            putchar(' ');
        }
        putchar('^');
    }
    putchar('\n');
    setConsoleRendition(CONSOLE_FOREGROUND_RED);
    printf("ERROR: ");
    setConsoleRendition(CONSOLE_DEFAULT);
    printf("%s\n\n", result.errorMessage);
}