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
#include <math.h>
#include <stdio.h> 
#include <stdlib.h>
#include <ctype.h>
#include <threads.h>

#include "parser.h"

// constants:
#define MAXOPSTACK 64
#define MAXNUMSTACK 64
#define OP_MAX 128
#define IS_DIGIT_OR_DECIMAL(c) ((c) == '.' || ((unsigned)((c) - '0') < 10))
#define GET_OPERATOR(c) (op_lookup[(unsigned char)(c)])

// -- Operator eval functions:
static inline double eval_uminus(double arg1, double arg2) {return -arg1;}
static inline double eval_exponent(double arg1, double arg2) {return pow(arg1, arg2);}
static inline double eval_multiply(double arg1, double arg2) {return arg1*arg2;}
static inline double eval_divide(double arg1, double arg2) {return arg1/arg2;}
static inline double eval_add(double arg1, double arg2) {return arg1+arg2;}
static inline double eval_subtract(double arg1, double arg2) {return arg1 -arg2;}
static inline double eval_modulo(double arg1, double arg2) {return fmodf(arg1,arg2);}

// -- Operator table details
enum {ASSOC_NONE=0, ASSOC_LEFT, ASSOC_RIGHT}; 
struct Operator {
    char operator; // string literal value
    int precedence; // i.e. PEMDAS
    int association; // handedness of operator
    int unary; // bool
    double (*eval)(double arg1, double arg2); //evaluation function
} operators[] = {
    {'_', 10, ASSOC_RIGHT, 1, eval_uminus},
    {'^', 9, ASSOC_RIGHT, 0, eval_exponent},
    {'*', 8, ASSOC_LEFT, 0, eval_multiply},
    {'/', 8, ASSOC_LEFT, 0, eval_divide},
    {'%', 8, ASSOC_LEFT, 0, eval_modulo},
    {'+', 5, ASSOC_LEFT, 0, eval_add},
    {'-', 5, ASSOC_LEFT, 0, eval_subtract},
    {'(', 0, ASSOC_NONE, 0, NULL},
    {')', 0, ASSOC_NONE, 0, NULL},
};

static struct Operator startoperator = {'X', 0, ASSOC_NONE, 0, (void*)0};
static thread_local struct Operator *op = (void*)0;
static thread_local struct Operator *pop;
static thread_local char *expr = (void*)0;
static thread_local char *tstart = (void*)0;

static struct Operator *op_lookup[OP_MAX];

static void init_operator_lookup(void) {
    for (int i = 0; i < sizeof operators / sizeof operators[0]; ++i) {
        op_lookup[operators[i].operator] = &operators[i];
    }
}

// -- stack manipulating functions
typedef struct {
    struct Operator *opstack[MAXOPSTACK];
    int nopstack;
    double numstack[MAXNUMSTACK];
    int nnumstack;
} ParserContext;

static inline void push_opstack(ParserContext *ctx, struct Operator *op) 
{
    if (ctx->nopstack>MAXOPSTACK-1) {
        fprintf(stderr, "ERROR: Operator stack overflow\n"); // use greater size for operator stack
        exit(EXIT_FAILURE);
    }
    ctx->opstack[ctx->nopstack++]=op; // increment operator stack 1 forward with operator argument 
}

static inline struct Operator *pop_opstack(ParserContext *ctx) {

    if (!ctx->nopstack) {
        fprintf(stderr, "ERROR: Operator stack empty\n");
        exit(EXIT_FAILURE);
        }
    return ctx->opstack[--ctx->nopstack];
}

static inline void push_numstack(ParserContext *ctx, double operand) {
    if (ctx->nnumstack>MAXNUMSTACK-1) {
        fprintf(stderr, "ERROR: Operand stack overflow\n");
        exit(EXIT_FAILURE);
    }
    ctx->numstack[ctx->nnumstack++]=operand;
    return;
}

static inline double pop_numstack(ParserContext *ctx) {
    if (!ctx->nnumstack) {
        fprintf(stderr, "ERROR: Operand stack empty\n");
        exit(EXIT_FAILURE);
    }
    return ctx->numstack[--ctx->nnumstack];
}

static inline void shunt_operator(ParserContext *ctx, struct Operator *op) {
    double n1, n2; // left and right operands

    //handle paranthesis by evaluating everything until the matching right parenthasis
    if (op->operator=='(') {
        push_opstack(ctx, op);
        return;
    } else if (op->operator==')') {
        // evaluate subexpressions with parenthasis and push result as number to operand stack
        while (ctx->nopstack > 0 && ctx->opstack[ctx->nopstack-1]->operator != '(') {
            pop=pop_opstack(ctx); // retrieve current operator
            n1=pop_numstack(ctx); //n1 becomes most recent operand
            
            // if unary-subtract found; handle as single operator
            if (pop->unary) push_numstack(ctx, pop->eval(n1, 0));
            else { // for all other operators:
                n2=pop_numstack(ctx); // n2 is now equal to most recent value in numstack
                push_numstack(ctx,pop->eval(n2, n1));
            }
        }
        // evaluate current operator, ensures not at parenthesis yet
        if (!(pop=pop_opstack(ctx)) || pop->operator != '(') {

            fprintf(stderr, "ERROR: Stack error. No matching \'(\'\n)");
            exit(EXIT_FAILURE);
        }
        return;
    }
    if (op->association==ASSOC_RIGHT) {
        // handling exponents:
        while (ctx->nopstack && op->precedence < ctx->opstack[ctx->nopstack-1]->precedence) {
            pop=pop_opstack(ctx); // retrieve operator function
            n1=pop_numstack(ctx); // retrieve exponent operand value
            if (pop->unary) push_numstack(ctx, pop->eval(n1, 0)); //decrement exponent operand
            else {
                n2=pop_numstack(ctx); // left-hand operand
                push_numstack(ctx, pop->eval(n2, n1));
            }
        }
    } else {
        // While the current operator does not take precedence over the former:
        while (ctx->nopstack && op->precedence <= ctx->opstack[ctx->nopstack-1]->precedence) {
            pop=pop_opstack(ctx);
            n1=pop_numstack(ctx);
            if (pop->unary) push_numstack(ctx, pop->eval(n1, 0));
            else {
                n2=pop_numstack(ctx);
                push_numstack(ctx, pop->eval(n2, n1));
            }
        }
    }
    push_opstack(ctx, op); 
}

enum TokenType { T_OPERATOR, T_NUMBER, T_WHITESPACE, T_INVALID };
static inline enum TokenType classify_char(char c) {
    if (IS_DIGIT_OR_DECIMAL(c)) return T_NUMBER;
    if (isspace(c)) return T_WHITESPACE;
    if (GET_OPERATOR(c)) return T_OPERATOR;
    return T_INVALID;
}

double parser(const char *expression) {
    const char *expr = expression;
    double n1, n2;

    ParserContext ctx = {0};

    init_operator_lookup();

    // --- reset stacks ---
    ctx.nnumstack = 0;
    ctx.nopstack = 0;

    struct Operator *lastoperator = &startoperator;

    // main iteration loop:
    for (expr = expression; *expr; ++expr) {
        enum TokenType token = classify_char(*expr);
        if (!tstart) {
            if (token == T_OPERATOR) {
                op=GET_OPERATOR(*expr);
                if (lastoperator && (lastoperator == &startoperator || lastoperator->operator != ')')) {
                    if (op->operator == '-') op = GET_OPERATOR('_');
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
                shunt_operator(&ctx, op);
                lastoperator=op;
            } else if (token == T_NUMBER) {
                tstart = expr;
            } else if (token == T_INVALID) {
                fprintf(stderr, "ERROR: Syntax error %c \n", *expr);
                return EXIT_FAILURE;
            }
        } else {
            if (token == T_WHITESPACE) {
                push_numstack(&ctx, strtod(tstart, NULL));
                tstart=NULL;
                lastoperator=NULL;
            } else if (token == T_OPERATOR) {
                push_numstack(&ctx, strtod(tstart, NULL));
                tstart=NULL;
                op = GET_OPERATOR(*expr);
                shunt_operator(&ctx, op);
                lastoperator=op;
            } else if (token != T_NUMBER) {
                fprintf(stderr, "ERROR: Syntax error \n");
                return EXIT_FAILURE;
            }
        }
    }
    // After tokens are handled, evaluate all remaining tokens on top of the operator stack
    if (tstart) push_numstack(&ctx, strtod(tstart, NULL));

    while (ctx.nopstack) {
        op=pop_opstack(&ctx);
        n1=pop_numstack(&ctx);
        if (op->unary) push_numstack(&ctx, op->eval(n1, 0));
        else {
            n2=pop_numstack(&ctx);
            push_numstack(&ctx, op->eval(n2, n1));
        }
    }

    // assertion method to ensure final operand stack has 1 value:
    if (ctx.nnumstack != 1) {
        fprintf(stderr, "ERROR: Number stack has %d elements after evaluation. Should be 1. \n", ctx.nnumstack);
        return EXIT_FAILURE;
    }
    double result = ctx.numstack[0];
    return result;
}
    
