# LED output extender 4x16
### multiplexer pins:
```
       ---_---
     -|       |-out7
  in0-|       |-out6
  in1-|  PIC  |-out5
  in2-|  28x2 |-out4
  in3-|       |-out3
     -|       |-out2
     -|       |-out1
  gnd-|       |-out0
     -|       |-vcc
     -|       |-gnd
 out8-|       |-out15
 out9-|       |-out14
out10-|       |-out13
out11-|       |-out12
       -------
```

```
                     ---_---
                   -|       |-out7 bathroom leakage
                in0-|       |-out6
                in1-|  PIC  |-out5 kitchen small light
                in2-|  28x2 |-out4 kitchen main light
                in3-|       |-out3 irrigation en
                   -|       |-out2 garden light
                   -|       |-out1 irrigation
                gnd-|       |-out0 __Not_available__
                   -|       |-vcc
                   -|       |-gnd
room light 1   out8-|       |-out15 bathroom light
room light 2   out9-|       |-out14 bathroom fan
heating       out10-|       |-out13 wc light
cooling       out11-|       |-out12 wc fan
                     -------
```

1. pull low all inputs for 10ms
2. set desired inputs for high for 30ms
3. the selected output will flip 
(e.g. in0 = 1, in1 = 1, in2 = 0, in3 = 0) out3 will be 1
the same will flip it back later
4. repeat 

- output defaults are all 0
- in0 is LSB
- in3 is MSB
