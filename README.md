# Okosotthon

### ESP32-S pins

```
                      ____________________
               3.3V -|1                 20|- GND
                 EN -|2    __________   21|- 
          Light sen -|3   |          |  22|- Servo en btn
               Poti -|4   |  ESP32-S |  23|-
           Soil sen -|5   |          |  24|-
          Water sen -|6   |__________|  25|- Servo
     DHT11 bathroom -|7                 26|- GND
       DHT22 garden -|8                 27|- Irrigation en btn
         DHT11 room -|9                 28|- Kitchen light btn
                    -|10                29|- WC light
                    -|11                30|- Door open sen
                    -|12                31|- Bathroom light
  Garden motion sen -|13                32|- LED[3]
                GND -|14                33|- LED[2]
                    -|15                34|- LED[1]
                  ! -|16                35|- LED[0]
                  ! -|17      ____      36|- !
                  ! -|18 [o] |    | [o] 37|- !
                 5V -|19_____|____|_____38|- !
                     
                              
```
