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
int interactiveExecution(CliArguments cli);
int helpExecution(CliArguments cli);


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

const CliCommand* commands[] = {
    &cmdHelp,
    &cmdVerbose,
    &cmdInteractive
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

    if (cliParseFlag(cli, cmdInteractive)) {
        return interactiveExecution(cli);
    }
    return oneShotExecution(cli);
}

int oneShotExecution(CliArguments cli) {
    bool verbose = cliParseFlag(cli, cmdVerbose);

    printf("Expression: ");
    char* input = getline();

    Vector tokens = tokenize(input);

    if (verbose)
        printWithFormatting(input, tokens);
    
    if (tokens.size == 0) {
        printf_s("Input is empty \n");
        return 1;
    }

    BlockNode rootBlock = buildBlocksTree(tokens);

    if (verbose)
        printBlocksTree(rootBlock);

    ExpressionNode* expRoot = buildExpressionTree(rootBlock);

    float result = evaluateExpressionNode(expRoot);
    
    printf_s("= %f\n", result);

    vectorFree(&tokens);
    freeBlocksTree(rootBlock);
    freeExpressionTree(expRoot);
    free(input);

    return 0;
}

int interactiveExecution(CliArguments cli) {
    bool verbose = cliParseFlag(cli, cmdVerbose);

    printf("Interactive mode: enter expressions, Ctrl+C to exit\n\n");

    while (true)
    {
        printf("> ");
        char* input = getline();

        Vector tokens = tokenize(input);

        if (verbose)
            printWithFormatting(input, tokens);
        
        if (tokens.size == 0) {
            printf_s("x Input is empty\n\n");
            continue;
        }

        BlockNode rootBlock = buildBlocksTree(tokens);

        if (verbose)
            printBlocksTree(rootBlock);

        ExpressionNode* expRoot = buildExpressionTree(rootBlock);

        float result = evaluateExpressionNode(expRoot);
        
        printf_s("= %f\n\n", result);

        vectorFree(&tokens);
        freeBlocksTree(rootBlock);
        freeExpressionTree(expRoot);
        free(input);
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