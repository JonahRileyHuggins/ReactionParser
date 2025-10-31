#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include "parser.h"

int main(void) {

    // eval expression:
    const char *expr = "3+4*2-7/5^2+(-3)^2"; 

    // iterations
    const int iterations = 1000000;

    clock_t start, end;

    // Recording start time
    start = clock();

    for (int i = 0; i < iterations; i++) {

        parser(expr);

    }
    end = clock();

    double elapsed = (double)(end-start)/CLOCKS_PER_SEC;

    printf("Evaluated %d iterations in %.6f seconds\n", iterations, elapsed);

    return 0;

}