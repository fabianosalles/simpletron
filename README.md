[![CMake Build & Test](https://github.com/fabianosalles/simpletron/actions/workflows/cmake-tests.yml/badge.svg?branch=master)](https://github.com/fabianosalles/simpletron/actions/workflows/cmake-tests.yml)

# Simpletron Machine Language Interpreter

A Simpletron Machine Language (SML) interpreter implementation in C++ as proposed by Deitel's book "C++ How to Program, 3rd edition".


![Alt text](screenshot/running.png "Running a program in the simpletron")


## The Simpletron Machine Language 

The simpletron machine language contains a few instructions

| CODE | NAME       | DESCRIPTION                                | 
------ |------------|------------------------------------------- |
| 00   | noop       | consumes 1 CPU cycle but doesn't do anything 
| 10   | read       | read a word from stdin into a specific location in memory       
| 11   | write      | write a word from a specific location in memory to the terminal    
| 20   | load       | load a word from a specific location in memory into the accumulator 
| 21   | store      | store a word from the accumulator into a specific location in memory
| 30   | add        | add a word from a specific location in memory to the word in the accumulator (leaving the result in the accumulator)
| 31   | subtract   | subtract a word from a specific location in memory from the word in the accumulator (leave result in the accumulator)
| 32   | divide     | divide a word from a specific location in memory into the word in the accumulator (leaving result in accumulator)
| 33   | multiply   | multiply a word from a specific location in memory into the word in the accumulator (leaving result in accumulator)
| 40   | branch     | branch to a specific location in memory
| 41   | branchneg  | branch to a specific location in memory if the accumulator is negative
| 42   | branchzero | Branch to a specific location in memory if the accumulator is zero
| 43   | branchpos  | Branch to a specific location in memory if the accumulator is positive
| 50   | halt       | called when the program is done with its task

## Assembly language

The interpreter accepts both the original numeric format and Simpletron assembly source. Assembly instructions are case-insensitive, labels end with a colon, and comments start with `#`, `;`, or `//`.

The complete assembly-language reference is available in [docs/assembly-language.md](docs/assembly-language.md).

```asm
		load value
		add one
		store result
		halt

value:  .word 40
one:    .data 2
result: .word 0
```

Instruction operands may be decimal memory addresses or labels. The `.word` and `.data` directives reserve one memory word for a signed 16-bit value. The assembler reports syntax errors with their source line number.

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


