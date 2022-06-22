symbol _0 = b0
symbol _1 = b1
symbol _2 = b2
symbol _3 = b3
symbol _4 = b4
symbol _5 = b5
symbol _6 = b6
symbol _7 = b7
let _0 = 0
let _1 = 0
let _2 = 0
let _3 = 0
let _4 = 0
let _5 = 0
let _6 = 0
let _7 = 0

main:
    goto wait_for_all_zeros

wait_for_all_zeros:
    if pinC.4 = 0 and pinC.3 = 0 and pinC.2 = 0 then
        goto wait_for_interrupt
    endif
    goto wait_for_all_zeros

wait_for_interrupt:
    if pinC.4 = 1 or pinC.3 = 1 or pinC.2 = 1 then
        pause 25
        goto decode
    endif
    goto wait_for_interrupt

decode:
    if pinC.4 = 0 and pinC.3 = 0 and pinC.2 = 1 then
        if _1 = 0 then let _1 = 1
        else let _1 = 0
        endif
    elseif pinC.4 = 0 and pinC.3 = 1 and pinC.2 = 0 then
        if _2 = 0 then let _2 = 1
        else let _2 = 0
        endif
    elseif pinC.4 = 0 and pinC.3 = 1 and pinC.2 = 1 then
        if _3 = 0 then let _3 = 1
        else let _3 = 0
        endif
    elseif pinC.4 = 1 and pinC.3 = 0 and pinC.2 = 0 then
        if _4 = 0 then let _4 = 1
        else let _4 = 0
        endif
    elseif pinC.4 = 1 and pinC.3 = 0 and pinC.2 = 1 then
        if _5 = 0 then let _5 = 1
        else let _5 = 0
        endif
    elseif pinC.4 = 1 and pinC.3 = 1 and pinC.2 = 0 then
        if _6 = 0 then let _6 = 1
        else let _6 = 0
        endif
    elseif pinC.4 = 1 and pinC.3 = 1 and pinC.2 = 1 then
        if _7 = 0 then let _7 = 1
        else let _7 = 0
        endif
    endif
    goto set_output

set_output:
    if _1 = 1 then HIGH B.1
    else LOW B.1
    endif
    if _2 = 1 then HIGH B.2
    else LOW B.2
    endif
    if _3 = 1 then HIGH B.3
    else LOW B.3
    endif
    if _4 = 1 then HIGH B.4
    else LOW B.4
    endif
    if _5 = 1 then HIGH B.5
    else LOW B.5
    endif
    if _6 = 1 then HIGH C.0
    else LOW C.0
    endif
    if _7 = 1 then HIGH C.1
    else LOW C.1
    endif
    goto main

;/home/zsombi/picaxe/compiler/picaxe20m2 -c/dev/ttyUSB0 ./src/pic/test.bas
