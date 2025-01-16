start:
        ldc loopindex
        ldnl 0
        ldc fibarraysize
        ldnl 0
        sub
        brz end

        ldc loopindex
        ldnl 0
        adc -1
        ldnl fibarray

        ldc loopindex
        ldnl 0
        adc -2
        ldnl fibarray

        add

        ldc loopindex
        ldnl 0
        stnl fibarray

        ldc loopindex
        ldnl 0
        adc 1
        ldc loopindex
        stnl 0
        br start
end:
        HALT



fibarraysize: data 6
fibarray: data 0
        data 1
        data 0
        data 0
        data 0
        data 0
loopindex: data 2
