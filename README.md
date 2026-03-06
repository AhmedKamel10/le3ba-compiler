
# Le3ba (لعبة)
A stack-based Virtual Machine and Bytecode Compiler built from scratch in C++.

Le3ba explores language internals, memory allocation, and function-call mechanics, implementing the full pipeline from raw text → tokens → bytecode → execution.


## Features

tack-based Architecture: All operations happen on a manual stack, similar to low-level VMs like JVM or CPython.

Function Frames: Local variables and parameter scoping using a call stack.

Control Flow: if statements and while loops via jump instructions (JZ and JMP).

Dynamic Arrays & Heap: alloc() creates heap blocks for arrays and dynamic memory access.

Arithmetic & Comparison: Standard math (+, -, *) and comparisons (<, >, ==).
## Examples
1. Iterative Fibonacci
```text
define fib(n) {
    int a = 0;
    int b = 1;
    int i = 0;
    int temp = 0 ;
    while (i < n) {
        temp = a + b; 
        a = b;
        b = temp;
        i = i + 1;
        print a;
    }
}

call fib(7);
```

2. Array Search Example
```text
int n = 4;
int arr = alloc(n);
set(arr, 0, 10);
set(arr, 1, 55);
set(arr, 2, 22);
set(arr, 3, 40);

int target = 55;
int found = 0;
int k = 0;
int current = 0;
while (k < 4) {
    current = get(arr, k);
    if (current == target) {
        found = 1;
    };
    k = k + 1;
}
int result = found;
print result;
```
## Instruction Set Architecture (ISA)
| Hex  | Mnemonic   | Stack Effect        | Description                              |
| ---- | ---------- | ------------------- | ---------------------------------------- |
| 0x01 | PUSH_INT   | → `value`           | Push a 4-byte integer onto the stack     |
| 0x02 | STORE_VAR  | `value` →           | Store top of stack into a local variable |
| 0x03 | PRINT      | `value` →           | Pop and print top of stack               |
| 0x04 | LOAD_VAR   | → `value`           | Load local variable onto stack           |
| 0x10 | ADD        | `a b` → `a+b`       | Integer addition                         |
| 0x11 | SUB        | `a b` → `a-b`       | Integer subtraction                      |
| 0x12 | MUL        | `a b` → `a*b`       | Integer multiplication                   |
| 0x14 | OP_EQ      | `a b` → `(a==b)`    | Equality check                           |
| 0x15 | OP_JZ      | `cond` →            | Jump to target if `cond == 0`            |
| 0x16 | OP_JMP     | →                   | Unconditional jump                       |
| 0x17 | OP_LT      | `a b` → `(a<b)`     | Less-than check                          |
| 0x18 | OP_GT      | `a b` → `(a>b)`     | Greater-than check                       |
| 0x19 | OP_CALL    | →                   | Call function (new frame)                |
| 0x20 | OP_RET     | →                   | Return from function (pop frame)         |
| 0x30 | OP_ALLOC   | `size` → `ptr`      | Allocate heap block and return pointer   |
| 0x31 | OP_STORE_I | `ptr idx val` →     | Store value at heap[ptr][idx]            |
| 0x32 | OP_LOAD_I  | `ptr idx` → `value` | Load value from heap[ptr][idx]           |
| 0x33 | OP_SIZE    | `ptr` → `size`      | Push size of heap block onto stack       |


## Notes
All operations are stack-based

Function calls create isolated local frames

Heap memory supports dynamic arrays

Control flow uses absolute jump offsets for loops and conditionals

## Limitaiton
No recursion yet: Function calls work, but recursive calls are not fully supported.

No comments: The language does not support inline or block comments.

No standard library: Only integer arithmetic, arrays, and control flow are implemented.

Limited data types: Only integers are supported; no floats, strings, or booleans natively.

Static array bounds: alloc() creates fixed-size arrays; resizing is not supported.

Jump offsets: Control flow uses absolute jumps, which can be error-prone in complex programs.

### Compilation 
g++ src/lexer.c++ src/parser.c++ -o le3ba
### Run

./le3ba tests/le3ba.txt
