/**
 * Shunting yard algorithm: converts infix notation equations to postfix notation
 * 
 * Credit given to https://literateprograms.org/shunting_yard_algorithm__c_.html 
 * For the primary code and helping undertand the algorithm
 */

// --- library import --- //
#include <stdio.h> 
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

// --- variable definitions --- //
#define MAXOPSTACK 64
#define MAXNUMSTACK 64 

// -- Operator eval functions:
/**
 * @brief Unary subtract function
 * @param arg1 integer operand
 * @todo integer now, convert to double later
 * @todo add avx instructions for optimization
 * 
 * @param arg2 integer operand to remove
 * @todo integer now, convert to double later
 * @todo add avx instructions for optimization
 * 
 * @returns integer argument1
 */
double eval_uminus(double arg1, double arg2) {

    return -arg1;
}

/**
 * @brief Exponentiate function
 * ternary operator, if exponent operand (arg2) is less than 0, return 0. 
 * If not, evaluate exponent for being equal to 0. If it is; return 1, 
 * if not: a).subtract 1 from exponent, b). reevalute method with exponent-1 
 * c). multiply by operand-1 (arg1). Repeats until exponent satisfies return 
 * condition
 * 
 * @param arg1 integer operand to be raised too
 * @todo integer now, convert to double later...
 * 
 * @param arg2 exponent operand
 * @todo integer now, convert to double later...
 * 
 * @returns product of arg1 raised to the power of arg2
 */
double eval_exponent(double arg1, double arg2) {

    return arg2 < 0 ? 0 : (arg2 == 0 ? 1: arg1*eval_exponent(arg1, arg2-1));
}

/**
 * @brief Multiplier function for abstraction
 * @param arg1 variable operand
 * @todo integer now, convert to double later
 * @todo add avx instructions for optimization
 * 
 * @param arg2 multiplier operand
 * @todo integer now, convert to double later...
 * 
 * @returns integer value of arg1 raised to the power of arg2
 */
double eval_multiply(double arg1, double arg2) {

    return arg1*arg2;
}

/**
 * @brief Division function
 * @param arg1 numerator operand
 * @todo integer now, convert to double later
 * @todo add avx instructions for optimization
 * 
 * @param arg2 denomenator operand
 * @todo integer now, convert to double later..
 * @todo add avx instructions for optimization
 * 
 * @returns quotient of arg1 and arg2
 */
double eval_divide(double arg1, double arg2) {
    if(!arg2){ // handles division by zero
        fprintf(stderr, "ERROR: Division by zero\n");
        exit(EXIT_FAILURE);
    }
    return arg1/arg2; 
}

/**
 * @brief Modulus Division function
 * @param arg1 numerator operand
 * @todo integer now, convert to double later
 * @todo add avx instructions for optimization
 * 
 * @param arg2 denomenator operand
 * @todo integer now, convert to double later..
 * @todo add avx instructions for optimization
 * 
 * @returns division remainder of arg1 and arg2
 */
double eval_modulo(double arg1, double arg2) {
    if(arg2 == 0.0){ // handles division by zero
        fprintf(stderr, "ERROR: Division by zero\n");
        exit(EXIT_FAILURE);
    }
    return fmodf(arg1,arg2); 
}

/**
 * @brief Addition function 
 * @param arg1 left operand
 * @todo integer now, convert to double later
 * @todo add avx instructions for optimization
 * 
 * @param arg2 right operand
 * @todo integer now, convert to double later..
 * @todo add avx instructions for optimization
 * 
 * @returns sum of arg1 and arg2
 */
double eval_add(double arg1, double arg2) {

    return arg1+arg2; 
}

/**
 * @brief Subtraction function 
 * @param arg1 left operand
 * @todo integer now, convert to double later
 * @todo add avx instructions for optimization
 * 
 * @param arg2 right operand
 * @todo integer now, convert to double later..
 * @todo add avx instructions for optimization
 * 
 * @returns difference of arg1 and arg2
 */
double eval_subtract(double arg1, double arg2) {

    return arg1-arg2; 
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

/**
 * @brief checks if a character is actually an operator
 * 
 * @param ch operator character-literal to retrieve
 */
struct Operator *get_operator(char ch) {

    for (int i=0; i < sizeof operators/sizeof operators[0]; ++i) {
        if (operators[i].operator==ch) return operators+i;
    }
    return NULL;
}

// -- stack manipulating functions

struct Operator *opstack[MAXOPSTACK]; //operator stack
int nopstack=0; // stores number of operators in the expression

double numstack[MAXNUMSTACK]; //operand-number stack
int nnumstack=0; // number of operands in expression

/**
 * @brief pushes operator to the operator stack, 
 * increments the operator counter at each call
 * 
 * @param op pointer to the operators struct, 1 operator in struct
 */
void push_opstack(struct Operator *op) 
{
    if (nopstack>MAXOPSTACK-1) {
        fprintf(stderr, "ERROR: Operator stack overflow\n"); // use greater size for operator stack
        exit(EXIT_FAILURE);
    }
    opstack[nopstack++]=op; // increment operator stack 1 forward with operator argument 
}

/**
 * @brief Removes operator from the operator stack,
 * decrements operator stack
 */
struct Operator *pop_opstack() {

    if (!nopstack) {
        fprintf(stderr, "ERROR: Operator stack empty\n");
        exit(EXIT_FAILURE);
        }
    return opstack[--nopstack];
}

/**
 * @brief Appends to the operand stack, increment operand stack counter
 * 
 * @param operand integer operand in the equation
 */
void push_numstack(double operand) {
    if (nnumstack>MAXNUMSTACK-1) {
        fprintf(stderr, "ERROR: Operand stack overflow\n");
        exit(EXIT_FAILURE);
    }
    numstack[nnumstack++]=operand;
    return;
}

/**
 * @brief Removes a value from the operand stack, decrement operands stack counter
 */
double pop_numstack() {
    if (!nnumstack) {
        fprintf(stderr, "ERROR: Operand stack empty\n");
        exit(EXIT_FAILURE);
    }
    return numstack[--nnumstack];
}

// -- shunting operators function
/**
 * @brief Handle operator characters once found
 * 
 * @param op operator for evaluation
 */
void shunt_operator(struct Operator *op) {
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

/**
 * @brief refactor method for evaluating decimal characters
 * thanks to Anthony DiGirolamo
 * 
 * @param c character to evaluate
 * 
 * @returns bool 0 || 1
 */
int isdigit_or_decimal(int c) {
  if (c == '.' || isdigit(c))
    return 1;
  else
    return 0;
}

/**
 * @brief Calculate the value of an infix notation equation by converting to postfix 
 * via the Shunting Yard Algorithm    
 */
int main(int argc, char *argv[]) {

    char *expr; 
    char *tstart = NULL;
    struct Operator startoperator = {'X', 0, ASSOC_NONE, 0, NULL};
    struct Operator *op=NULL;
    double n1, n2;

    struct Operator *lastoperator = &startoperator;

    // main iteration loop:
    for (expr=argv[1]; *expr; ++expr) {
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
                fprintf(stderr, "ERROR: Syntax error \n");
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
    printf("%G\n", numstack[0]);
    return EXIT_SUCCESS;
}
