; Read two values, multiply them, and print the product.
.code
READ multiplicand     ; Read the first factor.
READ multiplier       ; Read the second factor.
LOAD multiplicand     ; Load the first factor.
MULTIPLY multiplier   ; Multiply by the second factor.
STORE result
WRITE result          ; Print the product.
HALT

.data
multiplicand: .word 0  ; First factor.
multiplier:   .word 0  ; Second factor.
result:       .word 0  ; Product of both factors.
