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
one:    .word 1        ; First multiplier and counter increment.
counter: .word 0        ; Current multiplier.
product: .word 0        ; Current multiplication result.
limit:   .word 11       ; One past the last multiplier to print.
