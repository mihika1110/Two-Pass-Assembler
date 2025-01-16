variable_1:  data 5          ; Initial value = 5
variable_2:  data 10         ; Initial value = 10

start:      ldc variable_1   ; Load the address of variable_1 into the accumulator
            ldnl 0           ; Load the value of variable_1 into A (A = 5)
            stl 0            ; Store the value of variable_1 in local variable 0

            ldc variable_2   ; Load the address of variable_2 into the accumulator
            ldnl 0           ; Load the value of variable_2 into A (A = 10)
            ldc variable_1   ; Load the address of variable_1 into the accumulator
            stnl 0           ; Store the value of variable_2 into variable_1

            ldl 0            ; Load the original value of variable_1 (from local 0)
            ldc variable_2   ; Load the address of variable_2 into the accumulator
            stnl 0           ; Store the original value of variable_1 into variable_2

HALT
