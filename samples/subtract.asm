; Read two values and subtract the subtrahend from the minuend.
READ minuend        ; Read the value to subtract from.
READ subtrahend     ; Read the value to subtract.
LOAD minuend        ; Load the minuend into the accumulator.
SUBTRACT subtrahend ; Subtract the subtrahend.
STORE result
WRITE result        ; Print the difference.
HALT

minuend:    .word 0  ; Minuend input.
subtrahend: .word 0  ; Subtrahend input.
result:     .word 0  ; Difference between the inputs.
