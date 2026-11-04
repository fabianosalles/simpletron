; Read two values and add them together.
.code
READ first       ; Read the first value.
READ second      # Read the second value.
LOAD first       // Start with the first value.
ADD second       ; Add the second value.
STORE result
WRITE result
HALT             ; Stop after printing the result.

first:  .word 0
second: .word 0
result: .word 0
