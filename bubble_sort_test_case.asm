; Program Start
start:      ldc 0x1000         ; Set up stack pointer
            a2sp
            adj -1
            ldc 11             ; Number of elements to sort
            stl 0              ; Store count
            ldc array          ; Load base address of array
            call sort          ; Call sort function
            HALT               ; End of program

sort:       adj -1             ; Adjust stack for local variables
            stl 3              ; Save return address in local 3
            stl 4              ; Save base address of elements in local 4

            ldc 0
            stl 2              ; x = 0

loop_out:   ldl 0              ; Load count
            ldl 2              ; Load x
            sub
            ldc 1
            sub
            brz done           ; If x >= count-1, exit outer loop

            ldc 1
            stl 1              ; Initialize y = 1

loop_in:    ldl 0              ; Load count
            ldl 2              ; Load x
            sub
            ldl 1              ; Load y
            sub
            brz addx           ; If y >= count - x, exit inner loop

            ; Load addresses for comparison
            ldl 4              ; Load base address of elements
            ldl 1              ; Load y
            add
            stl 5              ; Store address of array[y] in local 5

            ldl 4              ; Load base address of elements
            ldl 1
            ldc 1
            sub                ; Calculate address of array[y-1]
            add
            stl 6              ; Store address of array[y-1] in local 6

            ; Compare array[y-1] and array[y]
            ldl 6              ; Load address of array[y-1]
            ldnl 0             ; Load array[y-1]
            ldl 5              ; Load address of array[y]
            ldnl 0             ; Load array[y]
            sub
            brlz addy          ; If array[y-1] <= array[y], skip swap

            ; Swap logic
swap:       ldl 6              ; Load address of array[y-1]
            ldnl 0             ; Load array[y-1] into A
            stl 7              ; Store it in temp (local 7)

            ldl 5              ; Load address of array[y]
            ldnl 0             ; Load array[y]
            ldl 6              ; Load address of array[y-1]
            stnl 0             ; array[y-1] = array[y]

            ldl 7              ; Load temp (array[y-1])
            ldl 5              ; Load address of array[y]
            stnl 0             ; array[y] = temp

addy:       ldc 1
            ldl 1              ; Load y
            add
            stl 1              ; y++

            br loop_in         ; Continue inner loop

addx:       ldc 1
            ldl 2              ; Load x
            add
            stl 2              ; x++

            br loop_out        ; Continue outer loop

done:       ldl 3              ; Load return address
            adj 5              ; Restore stack pointer and return
            return

; Data array for sorting
array:      data 9
            data 8
            data 7
            data 6
            data 5
            data 4
            data 3
            data 2
            data 1
            data 0
            data 0
