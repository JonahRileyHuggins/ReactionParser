# ReactionParser

A simple C program that evaluates arithmetic expressions using the **Shunting Yard algorithm**. Supports:

- `+`, `-`, `*`, `/`, `%`, `^`
- Parentheses for grouping
- Unary minus
- Floating-point numbers (doubles)

## Sources and Attribution

- Original Shunting Yard algorithm implementation: [literateprograms.org](https://literateprograms.org/shunting_yard_algorithm__c_.html)
- Double conversion support inspired by: [Anthony DiGirolamo, GitHub gist](https://gist.github.com/AnthonyDiGirolamo/1179218)
- Vectorized and maintained by: Jonah Huggins

## Single-File vs. Header/Source Implementation
A standalone, single-file version of the library is available on the `single-file` branch for easy integration. It is also included in release v1.0.

## Build Instructions

```bash
mkdir build
cd build
cmake ..
cmake --build .
````

This will create the executable `ReactionParser` in the `build` directory.

## Usage

```bash
./ReactionParser "3 + 4 * 2"
```

Example output:

```
11.000000
```

## Unit Tests

A test suite is provided in `tests/test_reactionparser.c`:

```bash
./test_reactionparser
```