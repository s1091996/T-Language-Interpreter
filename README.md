# T-Language Interpreter

A small interpreter for the T programming language, built with Flex and Bison.
It parses source code into an abstract syntax tree (AST) and executes programs
through a tree-walking evaluator.

## Features

- Integer and decimal numbers
- Local variable declarations and assignments
- Arithmetic expressions: `+`, `-`, `*`, and `/`
- Comparisons: `>`, `<`, `>=`, `<=`, `==`, and `!=`
- `IF ... ELSE` statements
- `READ` and `WRITE` statements
- Function declarations, calls, and `RETURN`
- C-style block comments: `/* ... */`

## Project structure

| File | Purpose |
| --- | --- |
| `t_syntax_tokenizer.l` | Flex lexer: converts source text into tokens. |
| `t_program_parser.y` | Bison grammar: validates syntax and constructs AST nodes. |
| `t_program_execution.c` | AST node creation and tree-walking execution. |
| `t_program_structures.h` | AST data structures and function declarations. |
| `t2c.c`, `t2c.h` | Program entry point and shared lexer/parser declarations. |
| `Makefile` | Build commands. |
| `test*.t` | Example T-language programs. |

## Requirements

- GCC
- Flex
- Bison
- Make

On Windows, the project can be built in a Linux-like environment such as WSL,
MSYS2, or MinGW, provided the tools above are available.

## Build

```bash
make parse
```

This creates the `parse` executable and generates the parser and lexer source
files required for the build.

## Run

```bash
./parse test.t
```

For example, `test.t` evaluates an `IF` condition. Since `10 < 3` is false,
the interpreter executes the `ELSE` block and prints:

```text
output3 3
output4 4
```

## Example program

```text
INT MAIN f()
BEGIN
    REAL radius;
    READ(radius, "Enter a radius:");
    REAL circumference := 2 * 3.14 * radius;
    WRITE(circumference, "Circumference:");
END
```

## How it works

```text
T source code
    -> Flex lexer
    -> Bison parser
    -> Abstract syntax tree (AST)
    -> Tree-walking evaluator
    -> Program output
```

The parser builds AST nodes while matching grammar rules. After parsing
finishes, the evaluator walks the tree and executes statements such as
assignments, conditionals, input, output, and function calls.

## Notes

This project is an interpreter: it executes the parsed AST directly rather
than generating a separate executable, assembly file, or C source file.
