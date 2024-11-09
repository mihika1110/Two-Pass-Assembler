; Program Start
start:      ldc n           ; Load the address of n
            ldnl 0          ; Load the value of n into A (A = n)
            stl 1           ; Store n in local variable 1 (n counter)

            ldc array_base  ; Load the base address of the array
            ldnl 0          ; Load the first element of the array as the initial value
            stl 0           ; Store initial value in local variable 0 (result)

            ldc 1           ; Initialize i = 1 (index starting from 1, since the first element is used)
            stl 2           ; Store i in local variable 2

loop:       ldl 2           ; Load i into A
            ldl 1           ; Load n into B
            sub             ; A = n - i
            brz end         ; If i == n, end the loop

            ldl 2           ; Load i into A
            ldc array_base  ; Load base address of the array
            add             ; A = address of array[i]
            ldnl 0          ; Load array[i] into A

            ldl 0           ; Load current result
            sub             ; A = result - array[i]
            stl 0           ; Store updated result

            ldl 2           ; Load i
            adc 1           ; i = i + 1
            stl 2           ; Store updated i

            br loop         ; Repeat loop

end:        ldl 0           ; Load the final result
            ldc result      ; Load the address of result
            stnl 0          ; Store the final result in memory

            HALT            ; End of the program

; Define the location of array and result
n:           data 5          ; Number of elements in the array
result:      data 0          ; Memory location to store the final result


; Array elements
array_base: data 1          ; array[0] = 1
            data 2          ; array[1] = 2
            data 3          ; array[2] = 3
            data 4          ; array[3] = 4
            data 5          ; array[4] = 5