/**
 * @brief comparing to muParser
 * 
 * To run: first download and install muParser
 * then, compile linked. Tested with GCC, compile command used:
 * `g++ -o muparser_test muparser_test.cpp -lmuparser`
 * to execute: 
 * `./muparser_test`
 */
#include <time.h>
#include <muParser.h>
#include <iostream>
#include <cassert>

int main() {
    try {
         // Define the expression
        std::string expr = "3+4*2-7/5^2+(-3)^2";

        clock_t start, end;

        start = clock();

        for (int i = 0; i < 1000000; i++ ) {
            // Create parser instance
            mu::Parser parser;
            // Set the expression to the parser
            parser.SetExpr(expr);

            // Evaluate the expression
            double result = parser.Eval();
        }

        end = clock();
        double elapsed = (double)(end-start)/CLOCKS_PER_SEC;

        printf("Evaluated %d iterations in %.6f seconds\n", 1000000, elapsed);

    } catch (const mu::Parser::exception_type &e) {
        std::cerr << "Parser error: " << e.GetMsg() << std::endl;
        return 1;
    }

    return 0;
}

