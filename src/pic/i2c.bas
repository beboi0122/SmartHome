init: hi2csetup i2cslave, %10100000

main:
    if hi2cflag = 0 then main ; poll flag, else loop
    hi2cflag = 0 ; reset flag
    ;get hi2clast,b1 ; get last byte written
    HIGH A.0 ; set output pins
    goto main