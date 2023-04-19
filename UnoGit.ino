
/*
  Dave Dempski
  (c) 2023 - Arduino Uno Sketch to develop GitHub user experience
  
    2/6/2023 - Uno board LED to flash on/off for 500ms + echo received serial data back to host
               TX & RX LEDs on board will flash for serial activity
    2/27/23 - Framework for Time to Logo measurement
    4/17/23 - Restored lost sections


    The circuit:
    * LCD RS pin to digital pin 12
    * LCD Enable pin to digital pin 10
    * LCD D4 pin to digital pin 5
    * LCD D5 pin to digital pin 4
    * LCD D6 pin to digital pin 3
    * LCD D7 pin to digital pin 2
    * LCD R/W pin to ground
    * LCD VSS pin to ground
    * LCD VCC pin to 5V
    * 10K potentialmeter - used to set LCD contrast connected to +5V and ground, wiper to LCD VO pin (pin 3) 
    * Relay control - pin 6
 
*/

#include <LiquidCrystal.h>

#define PWRRELAYCONTROL_OUT 6  // Power button control relay Arduino Digital Pin 6, used to fire the solenoid
#define PHOTODETECTOR_IN 8
#define LCDRS_IO 12
#define LCDENABLE_IO 10
#define LCDD4_IO 5
#define LCDD5_IO 4
#define LCDD6_IO 3
#define LCDD7_IO 2

#define LCDCOLS 16
#define LCDROWS 2

int incomingByte = 0; // for incoming serial data

/* constants for ASCII command characters */
#define RX_s  115 /* s Start the measurement run for Time to Logo */
#define RX_r  114 /* r Resume the run*/
#define RX_p  112 /* p Pause the run */
#define RX_c  99  /* c Clear and stop the run */

#define BAUDRATE 57600

#define TIMER_10MS 10
#define MAX_100MILLISECONDS 10
#define MAX_10MILLISECONDS 10
#define MAX_SECONDS 60
#define MAX_MINUTES 60
#define PWRBUTTONDELAY 80 /* time need to actuate power button in ms */

#define BOOT_TIMER_WAIT_CMD 0
#define BOOT_TIMER_START_CMD 1
#define BOOT_TIMER_TIME_CMD 2
#define BOOT_TIMER_PAUSE_CMD 3
#define BOOT_TIMER_STOP_CMD 4

byte boot_minutes;
byte boot_seconds;
byte boot_100milliseconds;
byte boot_10milliseconds;

int BootTimerCommandVal;

String TimerString = String("00:00:00");
String SOLTimerString = String("XX:XX");

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(LCDRS_IO, LCDENABLE_IO, LCDD4_IO, LCDD5_IO, LCDD6_IO, LCDD7_IO);

// Initialization section after power up or reset
void setup()
{
  // Enable serial port & configure for 57600 BAUD
  Serial.begin(BAUDRATE); 
  delay(500); 

  // send instructions to control PC
  Serial.print("\n\n");
  Serial.print(" s [ENTER] - Starts Time to Logo measurement\n");
  Serial.print(" p [ENTER] - Pause run\n");
  Serial.print(" r [ENTER] - Resume run\n");
  Serial.print(" c [ENTER] - Clear and stop run\n");

  // digital pin LED_BUILTIN configured as an output & set to LOW to turn LED Off
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  // Photodetector is an input
  pinMode(PHOTODETECTOR_IN, INPUT);

  // Power Control relay - Output active high
  pinMode(PWRRELAYCONTROL_OUT, OUTPUT);
  digitalWrite(PWRRELAYCONTROL_OUT, LOW);

  // reset the timers
  boot_minutes = 0;
  boot_seconds = 0;
  boot_100milliseconds = 0;
  boot_10milliseconds = 0; 

  // configure LCD columns & rows:
  lcd.begin(LCDCOLS, LCDROWS);

  // print start message
  lcd.setCursor(0, 0);
  lcd.print("Boot Timer v2.1");

  BootTimerCommandVal = BOOT_TIMER_WAIT_CMD;

  // print out starting timer value 00:00:00
  lcd.setCursor(0, 1);
  lcd.print(TimerString);

  // clear the Sign of life timer
  lcd.setCursor(10, 1);
  lcd.print(SOLTimerString);

  Serial.println("\nBoot Timer - Waiting...");

}

// main program loop
void loop()
{

  // wait for data to be received from the host
  if (Serial.available() > 0)
  {
    // read the incoming byte:
    incomingByte = Serial.read();

    // echo information back to host
    //Serial.print("RX: ");
    //Serial.println(incomingByte, DEC);

    // add a case statement to properly process 
    switch (incomingByte)
    {
      case RX_s:  /* Start Time to Logo measurement run */
                  BootTimerCommandVal = BOOT_TIMER_START_CMD;
                  Serial.print("Starting Time to Logo measurement\n");
      break;
      case RX_p:  /* Pause measurement run */
                  digitalWrite(LED_BUILTIN, LOW);   // LED Off
                  BootTimerCommandVal = BOOT_TIMER_PAUSE_CMD;
                  Serial.print("Time to Logo run paused\n");
       break;
      case RX_r:  /* Resume measurement run */
                  digitalWrite(LED_BUILTIN, HIGH);  // LED On 
                  BootTimerCommandVal = BOOT_TIMER_TIME_CMD;
                  Serial.print("Resume Time to Logo run\n");
      break;
      case RX_c:  /* Stop and clear measurement run */
                  BootTimerCommandVal = BOOT_TIMER_STOP_CMD;
                  Serial.print("Resetting Time to Logo run\n");
      break;
      default: /* fall through processing */
      break;      
    }
  }   

  switch (BootTimerCommandVal)
  {
    case BOOT_TIMER_WAIT_CMD:
                               break;
    case BOOT_TIMER_START_CMD:
                               digitalWrite(LED_BUILTIN, HIGH); // turn indicator LED on

                               // "press" the Power Button once
                               digitalWrite(PWRRELAYCONTROL_OUT, HIGH);
                               delay(PWRBUTTONDELAY);
                               digitalWrite(PWRRELAYCONTROL_OUT, LOW);

                               // account for delay for relay to activate Power Button
                               boot_10milliseconds = 8;

                               // start to dislay time
                               BootTimerCommandVal = BOOT_TIMER_TIME_CMD;
                               break;
    case BOOT_TIMER_TIME_CMD:
                               BootTimerProcessing(); 
                               break;
    case BOOT_TIMER_PAUSE_CMD:                               
                               break;
    case BOOT_TIMER_STOP_CMD:
                               digitalWrite(LED_BUILTIN, LOW); // turn indicator LED off
                               
                               // reset the boot timer
                               boot_minutes = 0;
                               boot_seconds = 0;
                               boot_100milliseconds = 0;
                               boot_10milliseconds = 0;     

                               // reset the timer string
                               TimerString = "00:00:00";
                               lcd.setCursor(0, 1);
                               lcd.print(TimerString);

                               // reset the Sign of Life timer string
                               SOLTimerString = "XX:XX";
                               lcd.setCursor(10, 1);
                               lcd.print(SOLTimerString);                               

                               BootTimerCommandVal = BOOT_TIMER_WAIT_CMD;
                               break;
    default: ;
  }

}

void BootTimerProcessing()
{

  delay(TIMER_10MS);  // wait for 10ms

  // increment how many 10ms we have delayed
  boot_10milliseconds = (boot_10milliseconds + 1) % MAX_10MILLISECONDS;

  // if we have counted 10 10ms delays, increment 100ms counter
  if (boot_10milliseconds == 0 )
  {
     boot_100milliseconds = (boot_100milliseconds + 1) % MAX_100MILLISECONDS;
  
     // if we have counted 10 100ms delays, increment seconds counter
     if ( boot_100milliseconds == 0 )
     {
        boot_seconds = (boot_seconds + 1) % MAX_SECONDS;

        // check to see if 1 minute has been counted
        if ( boot_seconds == 0 ) 
        {
           // if we have counted 60 seconds, increment minutes counter
           boot_minutes = (boot_minutes + 1) % MAX_MINUTES;
        }
     }
  }
   
  // print the timer:
  TimerString = "";
  if( boot_minutes < 10 ) 
  {
    // pad a 0
    TimerString = "0";
  }

  // add minutes to timer string
  TimerString = TimerString + boot_minutes + ":";

  if ( boot_seconds < 10 )
  {
    // pad a 0
    TimerString = TimerString + "0";
  }
  
  // add seconds to timer string
  TimerString = TimerString + boot_seconds + ":";

  // add decimal 100ths and 10ths
  TimerString = TimerString + boot_100milliseconds + boot_10milliseconds;

  // set the cursor to column 0, line 1
  lcd.setCursor(0, 1);
    
  // print timer to the second line of the LCD
  lcd.print(TimerString);

  // only save & print the Sign of Life timer once
  if ( digitalRead(PHOTODETECTOR_IN) == 1 )
  {
          
     // photodector ON, check for first time
     if ( SOLTimerString == "XX:XX" )
     {
       // save the XX:XX decimal times for Sign of Life Timer
       SOLTimerString = "";

       if ( boot_seconds < 10 )
       {
         SOLTimerString = "0";
       }
       
       SOLTimerString = SOLTimerString + String(boot_seconds, DEC) + ".";
       SOLTimerString = SOLTimerString + String(boot_100milliseconds, DEC) + String(boot_10milliseconds, DEC);           
       
       lcd.setCursor(10, 1);
       lcd.print(SOLTimerString);

       Serial.println("Sign of Life Captured!");
     }
  }
   
}
