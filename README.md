# Simpletron Machile Language Interpreter

A Simpletron Machine Language (SML) interpreter implementation in C++ as proposed by Deitel's book "C++ How to Program 3th edittion".

![Alt text](screenshot/running.png "Running a program in the simpletron")


## The Simpletron Machine Language 

The simpletron machine language contains a few instructions

| CODE | NAME       | DESCRIPTION                                | 
------ |------------|------------------------------------------- |
| 10   | read       | read a word from the stdin into a specific location in memory       
| 11   | write      | write a word from a specific location in memorry to the terminal    
| 20   | load       | load a word from a specific location in memory into the accumulator 
| 21   | store      | store a word from the accumulator into a speficif location in memory
| 30   | add        | add a word from a spefici location in memory to the word in the accumulator (leaving the result in the accumulator)
| 31   | subtract   | subtract a word from a specifc location in memory from the word in the accumulator (leave result in accumulator)
| 32   | divide     | divide a word from a specific location in memory into the word in the accumulator (leaving result in accumulator)
| 33   | multiply   | divide a word from a specific location in memory into the word in the accumulator (leaving result in accumulator)
| 40   | branch     | branch to a specific location in memory
| 41   | branchneg  | branch to a specific location in memory if the accumulator is negative
| 42   | branchzero | Branch to a specific location in memory if the accumulator is zero
| 43   | branchpos  | Branch to a specific location in memory if the accumulator is positive
| 50   | halt       | called when the program is done with its task

## Goals

This program was originally written as an academic exercise so, please, be kind with my student's code style.
I've discovered this code in one old backup disk and I decided to make some improvements:

 - [ ] Add runtime error checks
 - [ ] Add mnenomic support to the language
 - [ ] Implement a proper parser
 - [ ] Add support to g++ and linux
 
 ## Building

The project uses CMake and requires CMake 3.20 or newer, in addition to a C++17-compatible compiler.

From the project root, configure and build the project in a separate `build` directory:

```bash
cmake -S . -B build
cmake --build build
```

The executable is generated in `build/bin/`. On Windows with a multi-configuration generator, use:

```powershell
cmake -S . -B build
cmake --build build --config Release
```

The resulting executable is `build/bin/simpletron.exe` on Windows or `build/bin/simpletron` on Linux and macOS.

To run a program saved in a file:

```bash
./build/bin/simpletron samples/add.txt
```

On Windows PowerShell, use:

```powershell
.\build\bin\simpletron.exe samples\add.txt
```

If no input file is provided, the interpreter starts in interactive mode.
