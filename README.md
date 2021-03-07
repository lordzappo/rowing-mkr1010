# rowing-mkr1010
Arduino Rowing Computer with MKR1010

I broke the rowing computer that ships with this cheap Amazon rower I bought secondhand. 

I'm writing a new one in Arduino that will drive an LED screen with the inputs of a Polar H10 HR Monitor and the original stroke sensor that ships with the machine.

* The Arduino board is the [MKR1010](https://store.arduino.cc/usa/mkr-wifi-1010), note that it's 3.3V unlike most Arduino
* HR Monitor: https://www.polar.com/us-en/products/accessories/h10_heart_rate_sensor
* LCD https://www.buydisplay.com/small-size-arduino-code-lcd-20x4-i2c-character-display-wide-view-angle
    * This uses the I2C connector that ships on the board to reduce number of digital pins needed 

So far I only have the Polar and the Arduino, this code will connect to it (and many BLE heartrate sensors with minor modifications)

very rough draft
