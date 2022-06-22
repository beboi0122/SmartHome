# LED output extender 3x7
### multiplexer pins:
```
      ---_---
 vcc-|       |-gnd
    -|       |-
 in2-|  PIC  |-out1
 in1-|  14m2 |-out2
 in0-|       |-out3
out7-|       |-out4
out6-|       |-out5
      -------
```

1. pull low all inputs
2. set desired inputs for high for 25-50ms (50 recommended)
3. the selected output will flip 
(e.g. in0 = 1, in1 = 1, in2 = 0) out3 will be 1
the same will flip it back later
4. repeat 

- output defaults are all 0
- in0 is LSB
- in2 is MSB
