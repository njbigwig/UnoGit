# UnoGit
Learning how to use GitHub for my Uno project.

The project is based on the Arduino Uno R3 (ELEGOO clone):
![Image of Uno R3](/images/UnoR3.png)

The Sketch will be used to determine the Time-to-Logo for a Microsoft Surface PC - time between when the power button is pressed and the Windows logo is displayed on the screen:

![Image of Boot Logo](/images/SurfaceBootLogo.png)

* Control PC connects to Uno R3 board via USB-A and allows for commands to be sent (BAUD 57600)
* Light sensor (photodiode) is placed on the target unit display screen where the logo will appear
* Relay controls a solenoid to tuurn the target unit on
* LCD display provides timing info


![Image of Relay](/images/Relay.png)
[Order Part](https://www.amazon.com/gp/product/B0899L3CC3/ref=ppx_yo_dt_b_asin_title_o06_s00?ie=UTF8&th=1)



![Image of Solenoid](/images/Solenoid.png)
[Order Part](https://www.amazon.com/dp/B08Z7SK3HD?psc=1&ref=ppx_yo2ov_dt_b_product_details)



![Image of Photodiode](/images/Photodiode.png)


![Image of LCD](/images/LCD.png)

Note: There is a white backlight on the right side of the LCD which will turn on when the LCD is initialized
  
  
  
    
Make sure that the Arduino IDE Serial Monitor window has the following settings:
![Image of LCD](/images/SerialMonitorSettings.png)

