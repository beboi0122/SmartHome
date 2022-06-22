symbol bout = b0
symbol cout = b6
symbol mask = b1
symbol ain = b2
symbol totoggle = b3
symbol ctoggle = b4
symbol en_c = b5

init:
    let dirsa = %00010000
    let dirsb = %11111111
    let dirsc = %11111111
    let mask = 0
    let bout = 0
    goto main

main:
    goto wait_for_all_zeros

wait_for_all_zeros:
    if pinA.3 = 0 and pinA.2 = 0 and pinA.1 = 0 and pinA.0 = 0 then
        goto wait_for_interrupt
    endif
    goto wait_for_all_zeros

wait_for_interrupt:
    if pinA.3 = 1 or pinA.2 = 1 or pinA.1 = 1 or pinA.0 = 1 then
        pause 25
        goto decode
    endif
    goto wait_for_interrupt

decode:
    let totoggle = pinsa & %00000111
    if totoggle = %00000000 then
        let mask = %00000001
    else if totoggle = %00000001 then
        let mask = %00000010
    else if totoggle = %00000010 then
        let mask = %00000100
    else if totoggle = %00000011 then
        let mask = %00001000
    else if totoggle = %00000100 then
        let mask = %00010000
    else if totoggle = %00000101 then
        let mask = %00100000
    else if totoggle = %00000110 then
        let mask = %01000000
    else if totoggle = %00000111 then
        let mask = %10000000
    endif


    let en_c = pinsa & %00001000
    if en_c > 0 then
        let cout = cout ^ mask
    else
        let bout = bout ^ mask
    endif
    
    goto set_output

set_output:
    let pinsb = bout
    let pinsc = cout
    goto main

;/home/zsombi/picaxe/compiler/picaxe20m2 -c/dev/ttyUSB0 ./src/pic/test.bas
