; Program Start
start:      ldc n           ; Load the address of n
            ldnl 0          ; Load the value of n into A (A = n)
            stl 1           ; Store n in local variable 1 (n counter)

            ldc 0           ;sum=0
            stl 0           ; Store sum in local variable 0

            ldc 0           ;i=0
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

            br loop         

end:        ldl 0           ; Load the final sum
            ldc sum         ; Load the address of sum
            stnl 0          ; Store the final sum in memory

            HALT

; Define the location of array and sum
n:           data 5          ;n
sum:         data 0          ; Memory location to store the final sum

array_base: data 1          
            data 2  
            data 3 
            data 4  
            data 5   
