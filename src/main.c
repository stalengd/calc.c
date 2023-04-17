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
#include "timeutil.h"


int oneShotExecution(CliArguments cli);
int givenExpressionExecution(CliArguments cli, char* expression);
int interactiveExecution(CliArguments cli);
int helpExecution(CliArguments cli);
int promptAndCalculate(CliArguments cli, int expressionOffset);
int calculateExpression(CliArguments cli, char* expression, int expressionOffset);
void printParsingError(ParsingResult result, char* rawExpression, int expressionOffset);
void printTimer(datetime startTime, datetime endTime);


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

const CliCommand cmdTime = {
    .name = "-t --time",
    .desc = "Time calculation",
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
    &cmdTime,
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
    printf("Expression: ");
    return promptAndCalculate(cli, 12);
}

int givenExpressionExecution(CliArguments cli, char* expression) {
    return calculateExpression(cli, expression, 0);
}

int interactiveExecution(CliArguments cli) {
    bool verbose = cliParseFlag(cli, cmdVerbose);

    printf("Interactive mode: enter expressions, Ctrl+C to exit\n\n");

    while (true)
    {
        printf("> ");
        int code = promptAndCalculate(cli, 2);
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


int promptAndCalculate(CliArguments cli, int expressionOffset) {
    char* input = getline();

    int r = calculateExpression(cli, input, expressionOffset);

    free(input);
    return r;
}

int calculateExpression(CliArguments cli, char* expression, int expressionOffset) {
    bool verbose = cliParseFlag(cli, cmdVerbose);
    bool useTimer = cliParseFlag(cli, cmdTime);

    datetime startTime = time_now();

    Vector tokens = tokenize(expression);
    BlockNode rootBlock;
    memset(&rootBlock, 0, sizeof(BlockNode));
    ExpressionNode* expRoot = NULL;

    
    if (tokens.size == 0) {
        printf_s("x Input is empty\n\n");
        goto end;
    }

    ParsingResult result = buildBlocksTree(tokens, &rootBlock);
    if (result.isError) {
        printParsingError(result, expression, expressionOffset);
        goto end;
    }

    result = buildExpressionTree(rootBlock, &expRoot);
    if (result.isError) {
        printParsingError(result, expression, expressionOffset);
        goto end;
    }

    float resultValue = evaluateExpressionNode(expRoot);

    datetime endTime = time_now();
    
    printf_s("= %f\n\n", resultValue);

    if (useTimer) {
        printTimer(startTime, endTime);
    }

    end:
    if (verbose) {
        printWithFormatting(expression, tokens);
        if (rootBlock.members.size > 0)
            printBlocksTree(rootBlock);
        printf("\n");
    }
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

void printTimer(datetime startTime, datetime endTime) {
    unsigned int nanosecondsPerSeconds = 1000000000;
    long long deltaTimeSeconds = endTime.sec - startTime.sec;
    unsigned int deltaTimeNanoseconds = 0;
    if (endTime.nsec >= startTime.nsec) {
        deltaTimeNanoseconds = endTime.nsec - startTime.nsec;
    }
    else {
        deltaTimeNanoseconds = nanosecondsPerSeconds - startTime.nsec + endTime.nsec;
        deltaTimeSeconds -= 1;
    }
    printf_s("   (%lfs)\n\n", (double)deltaTimeSeconds + (double)deltaTimeNanoseconds / (double)nanosecondsPerSeconds);
}