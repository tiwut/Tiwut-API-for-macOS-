# cfeel Programming Language

**cfeel** is a minimalistic, extremely fast, natively compiled systems programming language designed for absolute simplicity and powerful interoperability with C. 

Compiled entirely from scratch without massive backend dependencies like LLVM or GCC, the `cfeelc` compiler translates your `cfeel` code directly into optimized ARM64 assembly, yielding highly-performant, standalone native executables.

## Philosophy
- **Simple**: No bloated syntax. Code should be incredibly easy to read and understand.
- **Fast**: Compiles directly to ARM64 assembly instructions.
- **C-Compatible**: The absolute strongest feature of `cfeel` is its Foreign Function Interface (FFI). You can link any C library or OS framework and call its functions directly.

---

## Installation & Building

Since `cfeelc` is entirely self-contained, building it requires only a standard C compiler (like `clang` or `gcc`) and takes less than a second.

```bash
# Clone the repository and build the compiler
make

# You now have the `cfeelc` compiler!
./cfeelc -o myapp mycode.cfeel
```

---

## Language Syntax Guide

`cfeel` has a very clean, straightforward syntax loosely inspired by Rust and Go, but radically simplified.

### Hello World
```feel
extern fn puts(str);

fn main() {
    let msg = "Hello, World!";
    puts(msg);
}
```

### Variables and Math
Variables are declared using the `let` keyword. 

```feel
fn main() {
    let x = 10;
    let y = 20;
    
    // Standard arithmetic operators are supported (+, -, *, /)
    let sum = x + y;
    let complex = sum / 2 * 5;
    
    // Print is a built-in macro for printing integers
    print(complex);
}
```

---

## The FFI Guide (Using C Libraries)

The core tenet of `cfeel` is that **any C library works in cfeel natively**. This is incredibly powerful. You do not need to wait for community wrappers or language bindings. 

To use a C library:
1. Declare the C function using `extern fn`.
2. Pass the library flag to `cfeelc` during compilation (e.g., `-lm` or `-lraylib`).

### Example 1: Standard Math Library (`libm`)

```feel
// Declare the C math functions
extern fn pow(base, exp);
extern fn sqrt(num);
extern fn printf(format, value);

fn main() {
    let result = pow(2, 3); // 2^3 = 8
    
    let format = "Result is: %d\n";
    printf(format, result);
}
```
Compile with:
```bash
./cfeelc -o math_test -lm program.cfeel
```

### Example 2: Graphics with Raylib (`libraylib`)

You can create massive graphical applications by linking C-based GUI frameworks directly:

```feel
// Declare Raylib C functions
extern fn InitWindow(width, height, title);
extern fn WindowShouldClose();
extern fn BeginDrawing();
extern fn EndDrawing();
extern fn ClearBackground(color);

fn main() {
    InitWindow(800, 600, "cfeel Native Window!");
    
    // Standard game loop logic...
    // (Loops are actively being added to the language core)
}
```
Compile with:
```bash
./cfeelc -o game -lraylib program.cfeel
```

---

## Compiler Driver (`cfeelc`)

`cfeelc` operates identically to standard industry compilers like `gcc`. 

**Usage**:
```bash
cfeelc [-c] [-S] [-o output] [linker flags...] <file.cfeel>
```

**Options**:
- `-o <file>` : Specify the output executable name.
- `-c`        : Compile and assemble, but do not link (outputs `.o`).
- `-S`        : Compile to raw ARM64 assembly, do not assemble or link (outputs `.s`).
- `-l<lib>`   : Link an external library (e.g., `-lraylib`, `-lm`).
- `-L<dir>`   : Specify a directory to search for libraries.
- `-framework`: Link a macOS Framework (e.g., `-framework Cocoa`).
