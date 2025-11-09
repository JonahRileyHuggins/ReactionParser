/**
 * @file parser.h
 * @brief Header file declaring methods used in parser.c
 *
 * @date 2025
 */
// --- library import --- //
#include <stdio.h> 
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <omp.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Evaluate an expression string (argv[1..argc-1])
 * @param expression the string expression to be evaluated
 * @return result float expression result
 */
double parser(const char *expression);

#ifdef __cplusplus
}
#endif