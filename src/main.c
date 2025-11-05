#include <stdio.h>
#include "parser.h"

/**
 * @brief CLI wrapper
 */
int main(int argc, char *argv[]) {
    char *expression = argv[1];
    double result = parser(expression);
    printf("%.15G\n", result);
    return 0;
}
