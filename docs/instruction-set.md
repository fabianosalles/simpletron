# Simpletron Instruction Set

Simpletron instructions are stored as four-digit decimal words. The first two digits identify the opcode and the last two digits identify a memory address.

| Code | Mnemonic | Operand | Description |
| ---: | --- | --- | --- |
| 00 | `NOOP` | none | Consume one CPU cycle without changing machine state. |
| 10 | `READ` | address | Read a value from the input provider into memory. |
| 11 | `WRITE` | address | Write a memory value to the output. |
| 20 | `LOAD` | address | Copy a memory value into the accumulator. |
| 21 | `STORE` | address | Copy the accumulator into memory. |
| 30 | `ADD` | address | Add a memory value to the accumulator. |
| 31 | `SUBTRACT` | address | Subtract a memory value from the accumulator. |
| 32 | `DIVIDE` | address | Divide the accumulator by a memory value. |
| 33 | `MULTIPLY` | address | Multiply the accumulator by a memory value. |
| 40 | `BRANCH` | address | Branch unconditionally to an address. |
| 41 | `BRANCHNEG` | address | Branch if the accumulator is negative. |
| 42 | `BRANCHZERO` | address | Branch if the accumulator is zero. |
| 43 | `BRANCHPOS` | address | Branch if the accumulator is positive. |
| 50 | `HALT` | none | Stop execution. |

The assembler accepts these mnemonics without requiring the numeric opcode. Operands must be memory addresses from `0` to `99`, or labels that resolve to an address in that range.
