**Simple C-like Processor (SCLP)**

**Description**
- **Summary:** A small compiler front-end and code generator for a C-like teaching language. It performs lexical analysis, parsing, semantic checks, generates three-address code (TAC), an intermediate RTL form, and produces SPIM/MIPS-style assembly.
- **Purpose:** Course project for a CS compiler course (CS 306). This repository is a minimal, readable implementation intended for learning compiler construction.

**Features**
- **Lexer & Parser:** Implemented using `flex` and `bison` (`scanner.l`, `parser.y`).
- **AST & Semantic Checks:** Builds an AST and performs type/scope checks (`ast.hpp`, `symtab.hpp`).
- **Codegen:** Generates TAC, RTL and SPIM assembly (`tac.cpp`, `rtl.cpp`, `spim.cpp`).
- **Language:** Supports `int`, `float`, `bool`, `string`, pointers, arrays, `struct`s, `for`/`while`/`do` loops, `if/else`, `try/catch/throw`, `print`/`read`, functions and return values.

**Requirements**
- **Build tools:** `make`, `g++` (C++11 or later)
- **Compiler tools:** `flex` (or `lex`), `bison` (or `yacc`)
- On Windows, use MSYS2/MinGW or WSL to provide these tools.

**Build**
1. Ensure `flex` and `bison` are installed and on your PATH.
2. Build with `make`:

```sh
make
```

This creates the executable named `sclp` (see [Makefile](Makefile#L1)).

**Clean**

```sh
make clean
```

**Usage**
- Basic usage: `./sclp [options] <source-file>`
- If no input file is given the compiler reads from `stdin`.
- Command-line options (implemented in [main.cpp](main.cpp#L1)):

- **--show-tokens:** dump lexical tokens to `SOURCE.toks` or stdout
- **--show-ast:** print AST to `SOURCE.ast` or stdout
- **--show-tac:** print three-address code to `SOURCE.tac` or stdout
- **--show-rtl:** print RTL to `SOURCE.rtl` or stdout
- **--show-spim:** emit SPIM assembly to `SOURCE.spim` or stdout
- **--show-comments:** include comments in printed outputs
- **--sa-scan/--sa-parse/--sa-ast/--sa-tac/--sa-rtl:** run that phase as a standalone check (scanner/parse/ast/tac/rtl)

Examples:

```sh
# Build
make

# Generate SPIM assembly for test.c (writes to test.c.spim)
./sclp --show-spim test.c

# Print AST to stdout
./sclp --show-ast test.c

# Run only the scanner to list tokens
./sclp --sa-scan test.c
```

**Output files**
- `SOURCE.toks` — token listing when `--show-tokens` is used
- `SOURCE.ast` — textual AST when `--show-ast` is used
- `SOURCE.tac` — three-address code when `--show-tac` is used
- `SOURCE.rtl` — RTL listing when `--show-rtl` is used
- `SOURCE.spim` — generated SPIM/MIPS assembly when `--show-spim` is used

**Project layout**
- **[Makefile](Makefile#L1)**: build rules (uses `flex` & `bison`).
- **[scanner.l](scanner.l#L1)**: lexical rules and token printing.
- **[parser.y](parser.y#L1)**: grammar, AST construction and semantic checks.
- **[main.cpp](main.cpp#L1)**: driver, command-line options and phase orchestration.
- **[ast.hpp](ast.hpp)**, **[tac.hpp](tac.hpp)**, **[rtl.hpp](rtl.hpp)**, **[spim.hpp](spim.hpp)**: core IR and codegen headers.
- **[tac.cpp](tac.cpp)**, **[rtl.cpp](rtl.cpp)**, **[spim.cpp](spim.cpp)**: code generation logic.
- **test.c**: small example input program.

**Notes & Tips**
- The parser uses semantic checks; type or undeclared-variable errors are reported with line numbers (see `parser.y`).
- If `help.txt` is present the program will print it when `--help` is supplied (see `main.cpp`).
- Use WSL or MSYS2 on Windows to avoid toolchain inconsistencies.

**Credits**
- This project is based on the original SCLP teaching materials: https://www.cse.iitb.ac.in/~uday/sclp-web/
