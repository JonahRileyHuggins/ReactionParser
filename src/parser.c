/**
 * @file parser.c
 * @brief ReactionParser: a string parser and evaluator for arithmetic expressions.
 *
 * This program uses the Shunting Yard algorithm to convert infix notation expressions
 * (like 3 + 4 * 2) to postfix notation and evaluate them. It supports:
 *   - Basic arithmetic: +, -, *, /, %
 *   - Exponentiation: ^
 *   - Unary minus
 *   - Parentheses
 *   - Floating point numbers
 *
 * Original implementation based on:
 *   - Shunting Yard Algorithm in C: https://literateprograms.org/shunting_yard_algorithm__c_.html
 *
 * Modifications for double precision arithmetic inspired by:
 *   - Anthony DiGirolamo, 2014: https://gist.github.com/AnthonyDiGirolamo/1179218
 *
 * Additional modifications and integration for ReactionParser project by:
 *   - Jonah Huggins
 *
 * @note This file includes custom stack implementations for operators and operands,
 *       and demonstrates operator precedence and right/left associativity handling.
 *
 * @date 2025
 */


// --- library import --- //
#include <stdio.h> 
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <omp.h>

#include "parser.h"

// -- Operator eval functions:
static inline double eval_uminus(double arg1, double arg2) {
    return -arg1;
}

static inline double eval_exponent(double arg1, double arg2) {
    
    return pow(arg1, arg2);
}

static inline double eval_multiply(double arg1, double arg2) {
    return arg1*arg2;
}

static inline double eval_divide(double arg1, double arg2) {
    return arg1/arg2;  // element-wise division of 4 doubles
}

static inline double eval_modulo(double arg1, double arg2) {
    if(arg2 == 0.0){ // handles division by zero
        fprintf(stderr, "ERROR: Division by zero\n");
        exit(EXIT_FAILURE);
    }
    return fmodf(arg1,arg2); 
}

static inline double eval_add(double arg1, double arg2) {
    return arg1+arg2;
}

static inline double eval_subtract(double arg1, double arg2) {

    return arg1 -arg2; 
}


// -- Operator table details
enum {ASSOC_NONE=0, ASSOC_LEFT, ASSOC_RIGHT}; 
struct Operator {
    char operator; // string literal value
    int precedence; // i.e. PEMDAS
    int association; // handedness of operator
    int unary; // bool
    double (*eval)(double arg1, double arg2); //evaluation function
} operators[] = {
    {'_', 9, ASSOC_RIGHT, 1, eval_uminus},
    {'^', 10, ASSOC_RIGHT, 0, eval_exponent},
    {'*', 8, ASSOC_LEFT, 0, eval_multiply},
    {'/', 8, ASSOC_LEFT, 0, eval_divide},
    {'%', 8, ASSOC_LEFT, 0, eval_modulo},
    {'+', 5, ASSOC_LEFT, 0, eval_add},
    {'-', 5, ASSOC_LEFT, 0, eval_subtract},
    {'(', 0, ASSOC_NONE, 0, NULL},
    {')', 0, ASSOC_NONE, 0, NULL},
};

static struct Operator *op_lookup[OP_MAX];

static void init_operator_lookup(void) {
    #pragma omp simd
    for (int i = 0; i < sizeof operators / sizeof operators[0]; ++i) {
        unsigned char c = operators[i].operator;
        op_lookup[c] = &operators[i];
    }
}

static inline struct Operator *get_operator(char ch) {
    return op_lookup[(unsigned char)ch];
}

// -- stack manipulating functions
struct Operator *opstack[MAXOPSTACK]; //operator stack
int nopstack=0; // stores number of operators in the expression

double numstack[MAXNUMSTACK]; //operand-number stack
int nnumstack=0; // number of operands in expression

static inline void push_opstack(struct Operator *op) 
{
    if (nopstack>MAXOPSTACK-1) {
        fprintf(stderr, "ERROR: Operator stack overflow\n"); // use greater size for operator stack
        exit(EXIT_FAILURE);
    }
    opstack[nopstack++]=op; // increment operator stack 1 forward with operator argument 
}

static inline struct Operator *pop_opstack() {

    if (!nopstack) {
        fprintf(stderr, "ERROR: Operator stack empty\n");
        exit(EXIT_FAILURE);
        }
    return opstack[--nopstack];
}

static inline void push_numstack(double operand) {
    if (nnumstack>MAXNUMSTACK-1) {
        fprintf(stderr, "ERROR: Operand stack overflow\n");
        exit(EXIT_FAILURE);
    }
    numstack[nnumstack++]=operand;
    return;
}

static inline double pop_numstack() {
    if (!nnumstack) {
        fprintf(stderr, "ERROR: Operand stack empty\n");
        exit(EXIT_FAILURE);
    }
    return numstack[--nnumstack];
}

static inline void shunt_operator(struct Operator *op) {
    struct Operator *pop; // pointer to Operator struct
    double n1, n2; // left and right operands

    //handle paranthesis by evaluating everything until the matching right parenthasis
    if (op->operator=='(') {
        push_opstack(op);
        return;
    } else if (op->operator==')') {
        // evaluate subexpressions with parenthasis and push result as number to operand stack
        while (nopstack > 0 && opstack[nopstack-1]->operator != '(') {
            pop=pop_opstack(); // retrieve current operator
            n1=pop_numstack(); //n1 becomes most recent operand
            
            // if unary-subtract found; handle as single operator
            if (pop->unary) push_numstack(pop->eval(n1, 0));
            else { // for all other operators:
                n2=pop_numstack(); // n2 is now equal to most recent value in numstack
                push_numstack(pop->eval(n2, n1));
            }
        }
        // evaluate current operator, ensures not at parenthesis yet
        if (!(pop=pop_opstack()) || pop->operator != '(') {

            fprintf(stderr, "ERROR: Stack error. No matching \'(\'\n)");
            exit(EXIT_FAILURE);
        }
        return;
    }
    if (op->association==ASSOC_RIGHT) {
        // handling exponents:
        while (nopstack && op->precedence < opstack[nopstack-1]->precedence) {
            pop=pop_opstack(); // retrieve operator function
            n1=pop_numstack(); // retrieve exponent operand value
            if (pop->unary) push_numstack(pop->eval(n1, 0)); //decrement exponent operand
            else {
                n2=pop_numstack(); // left-hand operand
                push_numstack(pop->eval(n2, n1));
            }
        }
    } else {
        // While the current operator does not take precedence over the former:
        while (nopstack && op->precedence <= opstack[nopstack-1]->precedence) {
            pop=pop_opstack();
            n1=pop_numstack();
            if (pop->unary) push_numstack(pop->eval(n1, 0));
            else {
                n2=pop_numstack();
                push_numstack(pop->eval(n2, n1));
            }
        }
    }
    push_opstack(op); 
}

static inline int isdigit_or_decimal(int c) {
  if (c == '.' || isdigit(c))
    return 1;
  else
    return 0;
}

double parser(const char *expression) {

    const char *expr; 
    char *tstart = NULL;
    struct Operator startoperator = {'X', 0, ASSOC_NONE, 0, NULL};
    struct Operator *op=NULL;
    double n1, n2;

    init_operator_lookup();

    // --- reset stacks ---
    nnumstack = 0;
    nopstack = 0;

    struct Operator *lastoperator = &startoperator;

    // main iteration loop:
    #pragma omp simd
    for (expr = expression; *expr; ++expr) {
        if (!tstart) { 
            // evaluate if current expression is an operator:
            if ((op=get_operator(*expr))) {
                if (lastoperator && (lastoperator == &startoperator || lastoperator->operator != ')')) {
                    if (op->operator == '-') op = get_operator('_');
                    else if (op->operator != '(') {
                        fprintf(
                            stderr, 
                            "ERROR: Illegal use of binary operator (%c)\n", 
                            op->operator
                        );
                        exit(EXIT_FAILURE);
                    }
                }
                /* move the current operator to the operator stack
                in priority order */
                shunt_operator(op);
                lastoperator=op;
            } else if (isdigit_or_decimal(*expr)) tstart = expr;
            else if (!isspace(*expr)) {
                fprintf(stderr, "ERROR: Syntax error %c \n", *expr);
                return EXIT_FAILURE;
            }
        } else {
            if (isspace(*expr)) {
                push_numstack(atof(tstart));
                tstart=NULL;
                lastoperator=NULL;
            } else if ((op=get_operator(*expr))) {
                push_numstack(atof(tstart));
                tstart=NULL;
                shunt_operator(op);
                lastoperator=op;
            } else if (!isdigit_or_decimal(*expr)) {
                fprintf(stderr, "ERROR: Syntax error \n");
                return EXIT_FAILURE;
            }
        }
    }
    // After tokens are handled, evaluate all remaining tokens on top of the operator stack
    if (tstart) push_numstack(atof(tstart));

    while (nopstack) {
        op=pop_opstack();
        n1=pop_numstack();
        if (op->unary) push_numstack(op->eval(n1, 0));
        else {
            n2=pop_numstack();
            push_numstack(op->eval(n2, n1));
        }
    }

    // assertion method to ensure final operand stack has 1 value:
    if (nnumstack != 1) {
        fprintf(stderr, "ERROR: Number stack has %d elements after evaluation. Should be 1. \n", nnumstack);
        return EXIT_FAILURE;
    }
    double result = numstack[0];
    return result;
}
    
