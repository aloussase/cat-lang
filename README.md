# mmt

mmt is an arithmetic expression transpiler to MIPS assembly.

## Description

mmt is a simple language for educational purposes that transpiles to
MIPS assembly. The objective is to help students who are learning MIPS
to see how common operations are performed at a lower level.

The mmt language supports the following operations:

* `+`: Takes two operands and adds them together
* `-`: Takes two operands and substracts the second from the first one
* `*`: Takes two operands and multiplies them

Other features that will be implemented are:

- [ ] `print` statements: print the numeric value of an expression
- [ ] arrays: memory allocation and access
- [ ] loops
- [ ] variables

## Build locally

```
cmake -S . -B ./build
cmake --build ./build

# Spawn a REPL
./build/src/mmt-exe
```

## License

MIT
