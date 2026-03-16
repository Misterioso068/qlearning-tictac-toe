#ifndef ERROR_LOGGER_H
#define ERROR_LOGGER_H

#include <stdio.h>

//Regular bold text
#define BBLK "\e[1;30m"
#define BRED "\e[1;31m"
#define BGRN "\e[1;32m"
#define BYEL "\e[1;33m"
#define BBLU "\e[1;34m"
#define BMAG "\e[1;35m"
#define BCYN "\e[1;36m"
#define BWHT "\e[1;37m"

#define COLOR_RESET "\e[0m"

static void error(const char* msg) {
    fprintf(stderr, BRED "ERROR: " COLOR_RESET "%s\n", msg);
}

static void warning(const char* msg) {
    fprintf(stderr, BYEL "%s" COLOR_RESET "\n", msg);
}

#endif // ERROR_LOGGER_H