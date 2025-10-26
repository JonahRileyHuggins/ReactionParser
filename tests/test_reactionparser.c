#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Execute ReactionParser and capture its stdout
 */
int run_parser(const char *expr, char *output, size_t len) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "./ReactionParser \"%s\" > tmp_output.txt 2>&1", expr);
    int ret = system(cmd);
    FILE *f = fopen("tmp_output.txt", "r");
    if (!f) return -1;
    fgets(output, len, f);
    fclose(f);
    // remove trailing newline if present
    output[strcspn(output, "\n")] = 0;
    return WEXITSTATUS(ret);
}

/**
 * @brief Assert helper
 */
void assert_eq(const char *expr, const char *expected) {
    char out[128];
    int status = run_parser(expr, out, sizeof(out));
    if (status == 0 && strcmp(out, expected) == 0) {
        printf("[PASS] %s = %s\n", expr, out);
    } else {
        printf("[FAIL] %s → got \"%s\" (exit %d), expected \"%s\"\n",
               expr, out, status, expected);
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Assert that parser *fails* (exit code != 0)
 */
void assert_fail(const char *expr) {
    char out[128];
    int status = run_parser(expr, out, sizeof(out));
    if (status != 0) {
        printf("[PASS] (expected fail) %s → \"%s\"\n", expr, out);
    } else {
        printf("[FAIL] %s unexpectedly succeeded with \"%s\"\n", expr, out);
        exit(EXIT_FAILURE);
    }
}

int main(void) {

    printf("=== ReactionParser Unit Tests ===\n");

    // --- Basic arithmetic
    assert_eq("3+4", "7");
    assert_eq("10-2", "8");
    assert_eq("8*5", "40");
    assert_eq("20/4", "5");
    assert_eq("7%3", "1");

    // --- Operator precedence
    assert_eq("3+4*2", "11");
    assert_eq("3*4+2", "14");
    assert_eq("10-4/2", "8");
    assert_eq("8/4*2", "4");

    // --- Parentheses
    assert_eq("(3+4)*2", "14");
    assert_eq("(8/(4-2))", "4");
    assert_eq("(3*(2+5))/7", "3");

    // --- Exponentiation (right-assoc)
    assert_eq("2^3^2", "512");
    assert_eq("(2^3)^2", "64");
    assert_eq("3^1^2", "3");

    // --- Unary minus
    assert_eq("-3+5", "2");
    assert_eq("4*-2", "-8");
    assert_eq("-2^2", "-4");     // typical binding
    assert_eq("(-2)^2", "4");
    assert_eq("-(-3)", "3");

    // --- Mixed precedence
    assert_eq("5+3*2^2", "17");
    assert_eq("(5+3)*2^2", "32");
    assert_eq("2^3*2", "16");
    assert_eq("10/2*3", "15");
    assert_eq("1+2-3*4/2^2", "0");

    // --- Error handling
    assert_fail("3++4");
    assert_fail("5*/2");
    assert_fail("((2+3)");
    assert_fail("2+3)");
    assert_fail("/5+2");
    assert_fail("2^");

    printf("All tests passed successfully.\n");
    return EXIT_SUCCESS;
}
