/**
 * @file parser.h
 * @brief Header file declaring methods used in parser.c
 *
 * @date 2025
 */
// --- library import --- //
#include <immintrin.h>
#include <stdio.h> 
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <omp.h>

// --- variable definitions --- //
#define MAXOPSTACK 64
#define MAXNUMSTACK 64 

// -- Operator eval functions:
/**
 * @brief Unary subtract function
 * 
 * @param arg1 integer operand
 * @param arg2 integer operand to remove
 * 
 * @returns integer argument1
 */
static inline __m256d eval_uminus(__m256d arg1, __m256d arg2);

/**
 * @brief Exponentiate function
 * 
 * @param arg1 integer operand to be raised too
 * @param arg2 exponent operand
 * 
 * @returns product of arg1 raised to the power of arg2
 */
static inline double eval_exponent(double arg1, double arg2);

/**
 * @brief Multiplier function for abstraction
 * 
 * @param arg1 variable operand
 * @param arg2 multiplier operand
 * 
 * @returns integer value of arg1 raised to the power of arg2
 */
static inline __m256d eval_multiply(__m256d arg1, __m256d arg2);
/**
 * @brief Division function
 * 
 * @param arg1 numerator operand
 * @param arg2 denomenator operand
 * 
 * @returns quotient of arg1 and arg2
 */
static inline __m256d eval_divide(__m256d arg1, __m256d arg2);

/**
 * @brief Modulus Division function
 * 
 * @param arg1 numerator operand
 * @param arg2 denomenator operand
 * 
 * @returns division remainder of arg1 and arg2
 */
static inline double eval_modulo(double arg1, double arg2);

/**
 * @brief Addition function 
 * 
 * @param arg1 left operand
 * @param arg2 right operand
 * 
 * @returns sum of arg1 and arg2
 */
static inline __m256d eval_add(__m256d arg1, __m256d arg2);

/**
 * @brief Subtraction function 
 * @param arg1 left operand
 * @param arg2 right operand
 * 
 * @returns difference of arg1 and arg2
 */
static inline __m256d eval_subtract(__m256d arg1, __m256d arg2);
/**
 * @brief checks if a character is actually an operator
 * 
 * @param ch operator character-literal to retrieve
 */
static inline struct Operator *get_operator(char ch);

/**
 * @brief pushes operator to the operator stack, 
 * increments the operator counter at each call
 * 
 * @param op pointer to the operators struct, 1 operator in struct
 */
static inline void push_opstack(struct Operator *op);

/**
 * @brief Removes operator from the operator stack,
 * decrements operator stack
 */
static inline struct Operator *pop_opstack();

/**
 * @brief Appends to the operand stack, increment operand stack counter
 * 
 * @param operand integer operand in the equation
 */
static inline void push_numstack(double operand);

/**
 * @brief Removes a value from the operand stack, decrement operands stack counter
 */
static inline double pop_numstack();

// -- shunting operators function
/**
 * @brief Handle operator characters once found
 * 
 * @param op operator for evaluation
 */
static inline void shunt_operator(struct Operator *op);

/**
 * @brief refactor method for evaluating decimal characters
 * thanks to Anthony DiGirolamo
 * 
 * @param c character to evaluate
 * 
 * @returns bool 0 || 1
 */
static inline int isdigit_or_decimal(int c);

/**
 * @brief Evaluate an expression string (argv[1..argc-1])
 * @param argc argument count
 * @param argv argument vector
 * @return result float expression result
 */
double parser(int argc, char *argv[]);