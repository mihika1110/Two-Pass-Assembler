; Initialize the values
ldc 00 10    ; Load the constant value 10 into the accumulator (A)
ldc 00 4     ; Load the constant value 4 into the accumulator (A) ;previously assigned value replaced

;(a=a-b) 
sub 07       ; Subtract the value in register B from the accumulator (A)

; Store the result
stl 03 0     ;Store the result at SP + 0(offset where the value is stored)

HALT 12      
