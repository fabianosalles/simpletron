[![CMake Build & Test](https://github.com/fabianosalles/simpletron/actions/workflows/cmake-tests.yml/badge.svg?branch=master)](https://github.com/fabianosalles/simpletron/actions/workflows/cmake-tests.yml)

# Simpletron Machine Language Interpreter

A Simpletron Machine Language (SML) interpreter implemented in C++, based on the exercise from Deitel's book *C++ How to Program, 3rd edition*.

The project supports both the original numeric machine-language format and a human-readable assembly language. The interpreter parses assembly source into Simpletron machine words and then executes those words in the virtual machine.

The following example reads a number and prints its multiplication table from 1 to 10. The complete source is available at [samples/multiplication-table.asm](samples/multiplication-table.asm).

```asm
; Print the multiplication table for the value entered by the user.
.code
READ number             ; Read the number whose table will be printed.
LOAD one                ; Initialize the counter to one.
STORE counter

loop: LOAD number       ; Calculate number * counter.
	  MULTIPLY counter
	  STORE product
	  WRITE product      ; Print the current table row.

	  LOAD counter       ; Advance the counter.
	  ADD one
	  STORE counter
	  LOAD counter
	  SUBTRACT limit     ; Continue while counter is less than eleven.
	  BRANCHNEG loop
	  HALT

.data
number:  .word 0        ; User-provided number.
one:     .word 1        ; First multiplier and counter increment.
counter: .word 0        ; Current multiplier.
product: .word 0        ; Current multiplication result.
limit:   .word 11       ; One past the last multiplier to print.
```

For an input value of `7`, the program prints `7`, `14`, `21`, through `70`.


## The Simpletron Machine Language

The complete opcode and mnemonic reference is available in [docs/instruction-set.md](docs/instruction-set.md).

## Assembly language

The interpreter accepts both the original numeric format and Simpletron assembly source. Assembly instructions are case-insensitive, labels end with a colon, and comments start with `#`, `;`, or `//`.

The complete assembly-language reference is available in [docs/assembly-language.md](docs/assembly-language.md).

## Goals

This program was originally written as an academic exercise, so please be kind with my student's code style.
I've discovered this code in one old backup disk, and I decided to make some improvements:

  - [x] Add runtime error checks
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


