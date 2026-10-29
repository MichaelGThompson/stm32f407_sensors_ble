## STM32F407 Black Board Project

# Project description

This project includes drivers for the MPU6050 accelerometer/gyroscope & BH1750 light sensor and uses the RYB2340_Lite BLE module. 
This simple bare metal app's main loop reads values from both sensors and send the accelerometer values (x, y and z) to any subscribed 
BLE devices (like the STM32 Toolbox app, LightBlue, etc) via the notify characteristic. The BLE module is connected to the STM32 via 
UART and a GPIO pin is toggled with every reading (to facilitate testing with a logic analyzer).

In addition to sending the accelerometer data, the light sensor provides lux readings (unit of illuminance and luminous emittance).
An onboard LED is turned on or off depending on if the lux value is below/over a somewhat arbitrary value of 250; that is, if the BH1750 
is covered/uncovered then the LED is off/on. A GPIO pin is turned on/off for use with the logic analyzer.

The SWV ITM Data Console was configured so printf statements could be monitored & saved to a log file.

The drivers were created using the object pattern with the implementation separate from the interface (via function pointers). 

To do: Create a simple driver for the BLE module to clean up the main function. Refactor one of my iOS BLE apps to connect to the RYB2340
module and display the notification data.

# Results

Logic analyzer results shown below:
<img width="1350" height="322" alt="analyzer output - mpu6050 and bh1750" src="https://github.com/user-attachments/assets/5a4d6ab0-8ec8-4006-8342-a864a7a523a1" />

Channel 0 represents the toggled port that occurs when each MPU6050 reading occurs. As expected, it is a simple square wave with a 50% duty cycle. 
Channel 1 describes the port (and onboard led state) when the sensor is covered & uncovered repeatedly.

Serial output (subset of the data) is show below. it shows that the sensors are initialized, lux values from the BH1750, the accelerometer data 
from the MPU6050 and notes when the LED is turned on & off.

--- accelerometer & gyroscope sensor initialized.
--- light sensor initialized.
--- mpu6050 accelerometer data: -1344 -420  16784
Ax:-1344, Ay: -420, Ay: 16784
--- BH1750 Lux value: 532.500000
--- turn led on!
--- mpu6050 accelerometer data: -1248 -388  16816
Ax:-1248, Ay: -388, Ay: 16816
--- BH1750 Lux value: 532.500000
--- turn led on!
--- mpu6050 accelerometer data: -1300 -460  16780
Ax:-1300, Ay: -460, Ay: 16780
--- BH1750 Lux value: 531.666626
--- turn led on!

...

--- mpu6050 accelerometer data: -1164 -316  16732
Ax:-1164, Ay: -316, Ay: 16732
--- BH1750 Lux value: 40.833332
--- turn led off!
--- mpu6050 accelerometer data: -1180 -876  16716
Ax:-1180, Ay: -876, Ay: 16716
--- BH1750 Lux value: 3.333333
--- turn led off!

... 

--- mpu6050 accelerometer data: -1308 -340  16936
Ax:-1308, Ay: -340, Ay: 16936
--- BH1750 Lux value: 525.000000
--- turn led on!

