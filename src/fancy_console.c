#include "fancy_console.h"

void setConsoleRendition(ConsoleGraphicsMode mode) {
    printf("\033[%dm", mode);
}

void setConsoleForegroundRGB(int r, int g, int b) {
    printf("\033[%d;2;%d;%d;%dm", CONSOLE_FOREGROUND_EXTENDED, r, g, b);
}

void setConsoleBackgroundRGB(int r, int g, int b) {
    printf("\033[%d;2;%d;%d;%dm", CONSOLE_BACKGROUND_EXTENDED, r, g, b);
}