#ifndef COMMAND_LINE_H
#define COMMAND_LINE_H

#include <stdbool.h>
#include <stdlib.h>

struct CliArguments
{
    int argc;
    char** args;
} typedef CliArguments;

struct CliCommand
{
    char* name;
    char* desc;
    void* defaultValue;
} typedef CliCommand;



bool cliParseFlag(CliArguments, CliCommand command);
char* cliParseString(CliArguments, CliCommand command);

int cliFindArgumentPosition(CliArguments, char* name);

#endif