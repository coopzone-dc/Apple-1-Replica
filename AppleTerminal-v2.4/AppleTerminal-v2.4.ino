/* ---------------------------------------------------------------------------------- 
 Apple 1 Replica CZ6502 firmware                                                    
                                                                                    
 By Derek Cooper                                                                    
 39-07-2024 Version 2.1 Moved serial output to be part of the ISR
 29-07-2024 Version 2.0 re-written to use pin change interupts on DA line .         
 26-07-2024 Version 1.1 clean up code                                               
 20-07-2024 Version 1.0 Modified code, removed unused features                      
                                                                                    
 Based on Version 1.0, Apple 1 Mini firmware for Arduino. By Ruud van Falier         
                                                                                    
 Enables serial communication between an Arduino and the Apple 1 PIA.               
                                                                                    
 Heavily based on Propeller source code from Briel Computers' Apple 1 replica.      
 ---------------------------------------------------------------------------------- */
#include <avr/interrupt.h>

// Port definitions (Arduino pins connected to the PIA)
#define PIN_DA  2  //was A0
#define PIN_RDA A1
#define PIN_PB0 A0   //was 2
#define PIN_PB1 3
#define PIN_PB2 4
#define PIN_PB3 A5
#define PIN_PB4 A4
#define PIN_PB5 A3
#define PIN_PB6 A2
#define PIN_PA0 5
#define PIN_PA1 6
#define PIN_PA2 7
#define PIN_PA3 8
#define PIN_PA4 9
#define PIN_PA5 10
#define PIN_PA6 11
#define PIN_STROBE 12

#define VERSION "2.4"

#define DELAY_ASCII 500 // 0.5ms
#define DELAY_VIDEO 10

// Global variables
static uint8_t video_data_pins[] = { PIN_PB0, PIN_PB1, PIN_PB2, PIN_PB3, PIN_PB4, PIN_PB5, PIN_PB6 };
static uint8_t kbd_data_pins[] = { PIN_PA0, PIN_PA1, PIN_PA2, PIN_PA3, PIN_PA4, PIN_PA5, PIN_PA6 };

volatile uint8_t video_data, vi;

/*
 * Program initialization
 */
void setup() 
{
  // Setup video data pins (output from the PIA)
  pinMode(PIN_RDA, OUTPUT);
  pinMode(PIN_DA, INPUT_PULLUP);
  for (uint8_t i = 0; i < 7; i++) pinMode(video_data_pins[i],INPUT);

  // Setup ASCII data pins (input for the PIA)
  pinMode(PIN_STROBE, OUTPUT);
  for (uint8_t i = 0; i < 7; i++) pinMode(kbd_data_pins[i],OUTPUT);

  Serial.begin(115200); //standard, sort of, speed.
  
  display_boot_message();
  digitalWrite(PIN_RDA, HIGH); //Set RDA and kbd_strobe to start values
  digitalWrite(PIN_STROBE, LOW);
  attachInterrupt(digitalPinToInterrupt(PIN_DA),process_video_int, FALLING); 
}

/*
 * Retrieves video (character) data from the PIA and sends it to the user over the serial connection.
 */
//process_video_data is now in the ISR
void process_video_int()
{  
     cli();
     video_data = 0;
     // Decode the data bits to a uint8_t.
     for (vi = 0; vi < 7; vi++) video_data |= (digitalRead(video_data_pins[vi]) << vi);  
     sei();    
      switch(video_data) {
      case 13:
        Serial.print((char)10);
        Serial.print((char)video_data);
        break;
      case 32 ... 127:
        Serial.print((char)video_data);
        break;
    }
   delayMicroseconds(DELAY_VIDEO);
   digitalWrite(PIN_RDA, LOW); //ok send 10usec pulse to clear video busy D7 on 6821
   delayMicroseconds(10);
   digitalWrite(PIN_RDA, HIGH);
}
/*
 * Program loop
 */
void loop() 
{
  process_serial_data(); 
}

/*
 * Displays the Apple 1 Replica firmware boot message and enables the blinking cursor
 */
void display_boot_message()
{
 
  Serial.print(F("CZ6502, Apple 1 Replica ["));
  Serial.print(VERSION);
  Serial.println(F("]"));
  Serial.println(F("READY..."));
}

/*
 * Processes data received from the serial connection.
 * Puts the received data on to the PIA where the CPU will read it from.
 */
 
void process_serial_data()
{
  uint8_t serial_data;  
  // Wait for serial data coming in.
  while (Serial.available() > 0)
  {
    serial_data = Serial.read();
    
    if (serial_data == 203) 
    {
      // Translate uppercase ESC to normal ESC.
      serial_data = 27;
    }

    if (serial_data >= 97 && serial_data <= 122)
    {
      // Translate lowercase characters to uppercase as the Apple 1 only supports uppercase.
      serial_data -= 32;
    }

    if (serial_data < 96)   
    {
      // Encode any Apple 1 compatible character to data bits.      
      for (uint8_t i = 0; i < 7; i++) digitalWrite(kbd_data_pins[i],bitRead(serial_data, i));
     
      // Pulse the STROBE bit so the PIA will process the input.
      digitalWrite(PIN_STROBE, HIGH);
      delayMicroseconds(DELAY_ASCII);
      digitalWrite(PIN_STROBE, LOW);
    }
   }
}
