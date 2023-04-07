
/*
  Dave Dempski
  (c) 2023 - Arduino Uno Sketch to develop GitHub user experience
  
    2/6/2023 - Uno board LED to flash on/off for 500ms + echo received serial data back to host
               TX & RX LEDs on board will flash for serial activity
    2/27/23 - Framework for Time to Logo measurement
*/
int incomingByte = 0; // for incoming serial data

/* constants for ASCII characters */
#define RX_s  115 /* s Start the measurement run for Time to Logo */
#define RX_r  114 /* r Resume the run*/
#define RX_p  112 /* p Pause the run */
#define RX_c  99  /* c Clear and stop the run */

#define IOPIN_2    2   /* IO pin #2 controls XBOX rumbler motor */

// Initialization section after power up or reset
void setup()
{
  // digital pin LED_BUILTIN configured as an output & set to LOW to turn LED Off
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  // IO pin #2 is an output, active high
  pinMode(IOPIN_2, OUTPUT);
  digitalWrite(IOPIN_2, LOW);


  // Enable serial port & configure for 9600 BAUD
  Serial.begin(9600); 
  delay(500); 

  // send instructions to control PC
  Serial.print("\n s [ENTER] - Starts Time to Logo measurement\n");
  Serial.print(" p [ENTER] - Pause run\n");
  Serial.print(" r [ENTER] - Resume run\n");
  Serial.print(" c [ENTER] - Clear and stop run\n");
}

// main program loop
void loop()
{
  // strobe the LED to indicate a sketch is running
  digitalWrite(LED_BUILTIN, HIGH);  // LED On
  delay(500);                       // wait 500ms

  digitalWrite(LED_BUILTIN, LOW);   // LED Off
  delay(500);                       // wait 500ms

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
                   Serial.print("Starting Time to Logo measurement\n");
      break;
      case RX_p:  /* Pause measurement run */
                  Serial.print("Time to Logo run paused\n");
       break;
      case RX_r:  /* Resume measurement run */
                  Serial.print("Resume Time to Logo run\n");
      break;
      case RX_c:  /* Stop and clear measurement run */
                  Serial.print("Reset Time to Logo run\n");
      break;
      default: /* fall through processing */
      break;      
    }
  }   

 

}
