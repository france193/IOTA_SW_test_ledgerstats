//
// Created by Francesco Longo on 12/06/2019.
//

#include "functions.h"

// error function that exits
void error_exit(int errorNum, char *msg) {
    printf("[Fatal Error]: %s \nExiting...\n", msg);
    exit(errorNum);
}