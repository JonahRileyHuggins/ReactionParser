#include <stdio.h>

double parser(int argc, char *argv[]);  // forward declaration

/**
 * @brief CLI wrapper
 */
int main(int argc, char *argv[]) {
    double result = parser(argc, argv);
    printf("%G\n", result);
    return 0;
}
