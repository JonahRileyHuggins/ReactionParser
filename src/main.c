#include <stdio.h>
#include "parser.h"

/**
 * @brief CLI wrapper
 */
int main(int argc, char *argv[]) {
    double result = parser(argc, argv);
    printf("%.15G\n", result);
    return 0;
}
