
; Program Start
start:      ldc n           ; Load the address of n
            ldnl 0          ; Load the value of n into A (A = n)
            stl 1           ; Store n in local variable 1 (n counter)

            ldc 0           ; Initialize sum to 0
            stl 0           ; Store sum in local variable 0

            ldc 0           ; Initialize i = 0 (index)
            stl 2           ; Store i in local variable 2

loop:       ldl 2           ; Load i into A
            ldl 1           ; Load n into B
            sub             ; A = n - i
            brz end         ; If i == n, end the loop

            ldl 2           ; Load i into A
            ldc array_base  ; Load base address of the array
            add             ; A = address of array[i]
            ldnl 0          ; Load array[i] into A

            ldl 0           ; Load current sum
            add             ; A = sum + array[i]
            stl 0           ; Store updated sum

            ldl 2           ; Load i
            adc 1           ; i = i + 1
            stl 2           ; Store updated i

            br loop         ; Repeat loop

end:        ldl 0           ; Load the final sum
            ldc sum         ; Load the address of sum
            stnl 0          ; Store the final sum in memory

            HALT            ; End of the program

; Define the location of array and sum
n:           data 5          ; Number of elements in the array
sum:         data 0          ; Memory location to store the final sum


; Array elements
array_base: data 1          ; array[0] = 1
            data 2          ; array[1] = 2
            data 3          ; array[2] = 3
            data 4          ; array[3] = 4
            data 5          ; array[4] = 5
