#include <stdio.h>
#include "parser.h"

/**
 * @brief CLI wrapper
 */
int main(int argc, char *argv[]) {
    char *expression = argv;
    double result = parser(expression);
    printf("%.15G\n", result);
    return 0;
}
