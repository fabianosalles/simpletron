[![CMake Build & Test](https://github.com/fabianosalles/simpletron/actions/workflows/cmake-tests.yml/badge.svg?branch=master)](https://github.com/fabianosalles/simpletron/actions/workflows/cmake-tests.yml)

# Simpletron Machine Language Interpreter

A Simpletron Machine Language (SML) interpreter implementation in C++ as proposed by Deitel's book "C++ How to Program, 3rd edition".


![Alt text](screenshot/running.png "Running a program in the simpletron")


## The Simpletron Machine Language

The complete opcode and mnemonic reference is available in [docs/instruction-set.md](docs/instruction-set.md).

## Assembly language

The interpreter accepts both the original numeric format and Simpletron assembly source. Assembly instructions are case-insensitive, labels end with a colon, and comments start with `#`, `;`, or `//`.

The complete assembly-language reference is available in [docs/assembly-language.md](docs/assembly-language.md).

```asm
.code
load value
add one
store result
halt

.data
value:  .word 40
one:    .word 2
result: .word 0
```

Instruction operands may be decimal memory addresses or labels. The `.code` and `.data` sections may appear in either order, but the assembler always places code before data. The `.word` directive reserves one data memory word, and the assembler reports syntax errors with their source line number.

Assembly programs use one statement per line. Labels are resolved automatically, and the assembler supports `NOOP`, `READ`, `WRITE`, `LOAD`, `STORE`, arithmetic operations, conditional branches, `BRANCHPOS`, and `HALT`. Existing numeric `.txt` programs remain supported for compatibility.

## Goals

This program was originally written as an academic exercise, so please be kind with my student's code style.
I've discovered this code in one old backup disk, and I decided to make some improvements:

 - [ ] Add runtime error checks
  - [x] Add mnemonic support to the language 	
 - [ ] Add debugging support (breakpoints, step-by-step execution, etc)	
  - [x] Implement a proper parser
 - [x] Add support to g++ and linux ( acheived via CMake )
 - [ ] Create a UI mode
 
 
## Building

Configure and compile the project using CMake. Make sure you have CMake installed on your system.

```bash
cmake -S . -B build
cmake --build build --config Release
```

## Running
Execute the interpreter binary with a sample file as an argument. The sample files are located in the `samples` directory. Both numeric `.txt` files and assembly `.asm` files are supported.


|OS |Command|
|--|-------|
|Linux  |```./build/bin/simpletron samples/add.txt```|
|Windows|```.\build\bin\simpletron.exe samples\add.txt```|

For example, to run the assembly version of the add sample:

|OS |Command|
|--|-------|
|Linux  |```./build/bin/simpletron samples/add.asm```|
|Windows|```.\build\bin\simpletron.exe samples\add.asm```|

## Running Tests
To execute the unit tests, first build the project with the `-DBUILD_TESTS=ON` option:


```bash
ctest --test-dir build --output-on-failure -C Release
```
Alternatively, run the tests binary directly:

|OS |Command|
|--|-------|
|Linux|```./build/bin/simpletron_tests```|
|Windows|```.\build\bin\simpletron_tests.exe```|


