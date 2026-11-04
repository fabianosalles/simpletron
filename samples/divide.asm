; Read a dividend and divisor, then print the integer quotient.
.code
READ dividend     ; Read the dividend.
READ divisor      ; Read the divisor.
LOAD dividend     ; Load the dividend into the accumulator.
DIVIDE divisor    ; Divide by the value in memory.
STORE result
WRITE result      ; Print the quotient.
HALT

.data
dividend: .word 0  ; Dividend input.
divisor:  .word 0  ; Divisor input.
result:   .word 0  ; Integer quotient.
