; Read one value and echo it to the output.
.code
READ value       ; Wait for one input value.
WRITE value      ; Print the value that was read.
HALT             ; Finish the program.

.data
value: .word 0   ; Storage for the input value.
