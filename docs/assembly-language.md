# Simpletron Assembly Language

The Simpletron interpreter accepts assembly source files in addition to the original numeric machine-language files. Assembly source is translated into the four-digit words stored in Simpletron memory before execution.

## Source format

A source file contains one statement per line. Blank lines are ignored. A source file may contain `.code` and `.data` sections in either order. The assembler always emits the complete code section before the complete data section.

Each statement is one of the following forms:

```asm
MNEMONIC operand
label: MNEMONIC operand
.word value
```

Mnemonics and labels are case-insensitive. An instruction operand is either a decimal memory address or a label that resolves to a memory address.

The assembler does not require commas between an instruction and its operand.

## Comments

Comments can occupy a complete line or follow a statement. The following comment markers are supported:

- `;`
- `#`
- `//`

For example:

```asm
; Read the input value.
READ value       # Store input in memory.
LOAD value       // Copy it to the accumulator.
```

Comments are removed before the line is parsed.

## Labels

A label identifies the memory address of the statement on the same line. Labels must end with a colon and must be unique within a source file.

```asm
		BRANCH loop
		HALT

loop:   LOAD value
		WRITE value
		BRANCH loop
value:  .word 7
```

The assembler performs two passes. It first assigns addresses to labels, then resolves label operands. A label can therefore refer to a statement that appears later in the file.

## Sections and data directives

`.code` selects the code section and `.data` selects the data section. Section markers do not emit memory words and do not accept operands. The sections may appear multiple times and in either order.

The data section accepts `.word`, which reserves one memory word for a signed 16-bit integer value. `.word` statements must be placed after `.data`.

```asm
.data
value:   .word 42
counter: .word -1

.code
LOAD value
HALT
```

The assembler lays out the example as code first (`LOAD`, `HALT`) followed by `value` and `counter`, even though the data section appears first in the source.

Simpletron has 100 memory positions, numbered from `0` to `99`. A complete assembled program must fit within those positions.

## Instruction set

See the complete [Simpletron instruction set reference](instruction-set.md) for opcodes, mnemonics, operands, and descriptions.

`NOOP` and `HALT` do not accept operands. Every other instruction requires an operand between `0` and `99`, or a label resolving to that range.

## Complete example

The following program reads two values, adds them, and writes the result:

```asm
; Add two input values.
.code
READ first          ; Read the first value.
READ second         ; Read the second value.
LOAD first          ; Load the first value.
ADD second          ; Add the second value.
STORE result        ; Save the sum.
WRITE result        ; Display the sum.
HALT                ; Finish execution.

.data
first:  .word 0
second: .word 0
result: .word 0
```

The equivalent numeric instructions are available in `samples/add.txt`. The assembly version is available in `samples/add.asm`.

## Running an assembly program

Build the project and run the executable with the assembly file as its argument:

```powershell
cmake -S . -B build
cmake --build build --config Release
.\build\bin\simpletron.exe samples\add.asm
```

The same executable also accepts the existing numeric `.txt` samples. The input provider supplies values when a `READ` instruction is executed.

## Errors

Syntax errors are reported as `AssemblyError` exceptions by the assembler. The error message includes the source line number. Examples of invalid input include:

- Unknown mnemonics.
- Missing operands.
- Extra tokens after an operand.
- Duplicate labels.
- Undefined or invalid numeric operands.
- A `.data` section marker with an operand.
- Statements other than `.word` inside the data section.
- Data values outside the signed 16-bit range.
- Programs larger than 100 memory words.
