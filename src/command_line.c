#include "command_line.h"

bool cliParseFlag(CliArguments args, CliCommand command) {
    char* name = command.name;
    int pos = cliFindArgumentPosition(args, name);
    return pos > -1;
}

int cliFindArgumentPosition(CliArguments args, char* name) {
    for (size_t i = 0; i < args.argc; i++)
    {
        char* arg = args.args[i];
        size_t j1 = 0;
        size_t j2 = 0;
        char c1 = arg[j1];
        char c2 = name[j2];
        bool areEqual = false;
        while (c1 != '\0' && c2 != '\0')
        {
            if (c1 == c2) {
                j1++;
                j2++;
                areEqual = true;
            }
            else {
                j1 = 0;
                j2++;
                c2 = name[j2];
                while (c2 != ' ' && c2 != '\0') {
                    j2++;
                    c2 = name[j2];
                }
                if (c2 == ' ') {
                    j2++;
                }
                areEqual = false;
            }
            c1 = arg[j1];
            c2 = name[j2];
        }
        
        if (areEqual) {
            return i;
        }
    }
    return -1;
}