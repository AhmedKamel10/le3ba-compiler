# Le3ba (لعبة) 

A stack-based Virtual Machine and Bytecode Compiler built from scratch in C++. 

I built this project to explore the internals of language design, memory allocation, and function-call mechanics. The project handles the full pipeline: from raw text to tokens, from tokens to custom bytecode, and from bytecode to execution inside a virtualized CPU.

## 🚀 Features
- **Stack-based Architecture:** All operations happen on a manual stack, mirroring how low-level VMs (like the JVM or CPython) operate.
- **Function Frames:** Support for local scoping and parameters using a call stack.
- **Control Flow:** Iterative logic via `if` statements and `while` loops implemented with jump offsets (`JZ` and `JMP`).
- **Memory Management:** Basic heap allocation using an `alloc()` instruction that returns pointers to dynamic memory blocks.

## 💻 Code Examples

### 1. Iterative Fibonacci
This script demonstrates the VM's ability to handle loops, variable updates, and function calls.

```text
define fib(n) {
    int a = 0;
    int b = 1;
    int i = 0;
    int temp = 0; 
    while (i < n) {
        temp = a + b; 
        a = b;
        b = temp;
        i = i + 1;
        print a;
    };
};

call fib(7);
```

2. Manual Memory Allocation
```text
int size = 10;
int my_array = alloc(size); // Reserves 10 slots and returns a pointer
print my_array;
```
3. Nested Control Flow
``` text
int m = 0;
while (m < 3) {
    int s = 0;
    while (s < 2) {
        print m;
        print s;
        s = s + 1;
    };
    m = m + 1;
};
```


How It Works

Lexer
Converts source text into tokens (keywords, identifiers, numbers).

Compiler
Parses tokens and generates custom bytecode.
Handles variable-to-ID mapping, jump target calculation, and stack-frame layout.

Virtual Machine
Executes bytecode using a dispatch loop.
Uses a vector<array<int, 256>> to manage local variable frames per function call.


## 🧾 Instruction Set Architecture (ISA)

| Hex  | Mnemonic     | Stack Effect (before → after) | Description |
|------|-------------|-------------------------------|-------------|
| 0x01 | PUSH_INT    | → `value`                     | Pushes a 4-byte immediate integer onto the stack |
| 0x02 | STORE_VAR   | `value` →                     | Stores top of stack into local variable slot |
| 0x03 | PRINT       | `value` →                     | Pops and prints top of stack |
| 0x04 | LOAD_VAR    | → `value`                     | Loads local variable onto stack |
| 0x10 | ADD         | `a b` → `a+b`                 | Integer addition |
| 0x11 | SUB         | `a b` → `a-b`                 | Integer subtraction |
| 0x12 | MUL         | `a b` → `a*b`                 | Integer multiplication |
| 0x14 | OP_EQ       | `a b` → `(a==b)`              | Equality comparison (1 or 0) |
| 0x15 | OP_JZ       | `cond` →                      | Jump to target if `cond == 0` |
| 0x16 | OP_JMP      | →                             | Unconditional jump |
| 0x17 | OP_LT       | `a b` → `(a<b)`               | Less-than comparison |
| 0x18 | OP_GT       | `a b` → `(a>b)`               | Greater-than comparison |
| 0x19 | OP_CALL     | →                             | Calls function (creates new frame) |
| 0x20 | OP_RET      | →                             | Returns from function (destroys frame) |
| 0x30 | OP_ALLOC    | `size` → `ptr`                | Allocates heap block and returns pointer |
| 0x31 | OP_STORE_I  | `ptr idx val` →               | Stores value at heap[ptr][idx] |
| 0x32 | OP_LOAD_I   | `ptr idx` → `value`           | Loads value from heap[ptr][idx] |

### Notes
- All operations are **stack-based**
- Function calls create isolated **local variable frames**
- Heap memory is indexed via pointers returned by `OP_ALLOC`
- Control flow is implemented via absolute jump offsets
###
g++ main.cpp lexer.c++ parser.c++ -o le3ba


Run

(Reads source code from le3ba.txt)

./le3ba




If you want next:
- a **shorter “resume-friendly” README**
- a **diagram section**
- or a **🔥 tagline for recruiters**

Just say the word.
