# CVM++ — Custom Virtual Machine

A lightweight scripting language built from scratch in C++ that compiles down to proprietary bytecode, executed by a custom stack-based Virtual Machine.

---

## What It Does

CVM++ takes source code written in a simple custom language (`.cvm` files), runs it through a full compilation pipeline, and executes it:

```
Source Code (.cvm)
      ↓
  Tokenizer       →  tokens
      ↓
   Parser         →  Abstract Syntax Tree (AST)
      ↓
  Compiler        →  bytecode instructions
      ↓
Virtual Machine   →  execution result
```

---

## Project Structure

```
CVM_plus_plus/
├── src/
│   ├── Tokenizer.hpp / Tokenizer.cpp         — lexical analysis
│   ├── Parser.hpp / Parser.cpp               — recursive descent parser + AST
│   ├── Statement.hpp / Statement.cpp         — AST node types
│   ├── Type.hpp / Type.cpp                   — type system
│   ├── FunctionDefinition.hpp / .cpp         — function definitions
│   ├── Instruction.hpp / Instruction.cpp     — ISA / opcode definitions
│   ├── BytecodeInterpreter.hpp / .cpp        — stack-based VM execution engine
│   └── main.cpp                              — compiler, REPL, file runner
└── tests/
    ├── test_operators.cvm
    ├── test_if.cvm
    ├── test_bool.cvm
    ├── test_let.cvm
    ├── test_input.cvm
    └── demo.cvm
```

---

## Building

Requires g++ with C++17 support. No external dependencies.

```bash
g++ -std=c++17 -o cvm \
  src/main.cpp \
  src/Tokenizer.cpp \
  src/Parser.cpp \
  src/Statement.cpp \
  src/Type.cpp \
  src/FunctionDefinition.cpp \
  src/Instruction.cpp \
  src/BytecodeInterpreter.cpp \
  -I src
```

---

## Usage

### Run a script file
```bash
./cvm script.cvm
```

### Run with debug mode (shows tokens, AST, and bytecode)
```bash
./cvm script.cvm --debug
```

### Interactive REPL
```bash
./cvm
```
Type your program line by line. Press Enter on a blank line to execute. Type `exit` to quit, `--debug` to toggle debug mode.

---

## Language Reference

### Data Types

| Type | Description |
|------|-------------|
| `int` | 16-bit signed integer |
| `bool` | Boolean (stored as int: 0 or 1) |
| `let` | Type-inferred integer variable |

### Variable Declaration

```
int x = 10;
let y = 20;
bool flag = true;
```

### Operators

| Operator | Description |
|----------|-------------|
| `+` | Addition |
| `-` | Subtraction |
| `*` | Multiplication |
| `/` | Integer division |
| `<` | Less than |
| `==` | Equality |
| `=` | Assignment |

### Control Flow

```
if (x < 10) {
    print(x);
} else {
    print(0);
};

while (x < 100) {
    x = x + 1;
};
```

### Functions

```
int factorial(int n) {
    int result = 1;
    while (1 < n) {
        result = result * n;
        n = n - 1;
    };
    return(result);
}

int main() {
    print(factorial(5));
    return(0);
}
```

Every program must have a `main` function as the entry point.

### Built-in Keywords

| Keyword | Description |
|---------|-------------|
| `print(x)` | Print an integer value to stdout |
| `input()` | Read an integer from stdin |
| `return(x)` | Return a value from a function |
| `true` | Boolean literal (= 1) |
| `false` | Boolean literal (= 0) |

---

## Example Program

```
int factorial(int n) {
    int result = 1;
    while (1 < n) {
        result = result * n;
        n = n - 1;
    };
    return(result);
}

int main() {
    print(10);
    print(10 - 3);
    print(10 * 3);
    print(10 / 3);

    bool flag = true;
    if (flag == true) {
        print(1);
    } else {
        print(0);
    };

    let x = 1;
    while (x < 6) {
        print(x);
        x = x + 1;
    };

    print(factorial(5));
    print(input());

    return(0);
}
```

Output:
```
10
7
30
3
1
1
2
3
4
5
120
<your input>
```

---

## Instruction Set Architecture (ISA)

The VM uses a fixed 4-byte instruction format: `opcode (1 byte) + p1 (1 byte) + p2 (2 bytes)`

| Opcode | Description |
|--------|-------------|
| `EXIT` | Halt execution |
| `PUSH_INT` | Push integer literal onto stack |
| `POP_INT` | Pop top of stack |
| `ADD_INT` | Pop two values, push sum |
| `SUB_INT` | Pop two values, push difference |
| `MUL_INT` | Pop two values, push product |
| `DIV_INT` | Pop two values, push quotient |
| `COMP_INT_LT` | Push 1 if left < right, else 0 |
| `COMP_INT_EQ` | Push 1 if left == right, else 0 |
| `PRINT_INT` | Pop and print top of stack |
| `READ_INT` | Read integer from stdin, push onto stack |
| `LOAD_INT_BASEPOINTER_RELATIVE` | Load local variable onto stack |
| `STORE_INT_BASEPOINTER_RELATIVE` | Store top of stack into local variable |
| `JUMP_BY` | Unconditional relative jump |
| `JUMP_BY_IF_ZERO` | Jump if top of stack is zero |
| `CALL` | Call a function |
| `RETURN` | Return from a function |

---

## Debug Mode Output

Running with `--debug` shows three stages of compilation:

```
=== TOKENS ===
Token(IDENTIFIER, "int", 1)
Token(IDENTIFIER, "main", 1)
...

=== AST ===
int main() {
  VARIABLE_DECLARATION signed int x (
    LITERAL signed integer 10
  )
  ...
}

=== BYTECODE ===
[0] PUSH_INT p2=10
[1] STORE_INT_BASEPOINTER_RELATIVE p2=0
...
```

---

## Technical Concepts Demonstrated

- **Lexical Analysis** — character-by-character tokenization with state machine
- **Recursive Descent Parsing** — operator precedence, backtracking with `optional<T>`
- **Abstract Syntax Tree** — tree of `Statement` nodes with recursive structure
- **Bytecode Compilation** — single-pass compiler with backpatching for jumps
- **Stack-based VM** — base-pointer-relative addressing for locals and parameters
- **Control Flow** — `if/else` and `while` via conditional/unconditional jumps

---

## Credits

Based on tutorial code by [Uli Stein](https://github.com/uliwitness):
- [simpleparser](https://github.com/uliwitness/simpleparser)
- [bytecodeinterpreter](https://github.com/uliwitness/bytecodeinterpreter)
- [parserandcompiler](https://github.com/uliwitness/parserandcompiler)

Extended with: additional operators (`-`, `*`, `/`, `==`), `if/else` control flow, `bool` type, `let` keyword, `input` built-in, REPL, and debug mode.

Reference: [Crafting Interpreters](https://craftinginterpreters.com/) by Robert Nystrom.