# stm8s-hcsr04

## HC-SR04
* An ultrasonic distance meter

## Wires
```
STM8S003F3P3     HC-SR04
+----------+     +-----+
|          |     |     |
|        C4|<--->|Trig |
|          |     |     |
|        D4|<--->|Echo |
|          |     |     |
|       GND|<--->|GND  |
|          |     |     |
+----------+     +-----+
```
C4 GPIOC_PIN_4
D4 TIM2_CH1
