/*
   McGill Rocket Team - Ground Station Receiver
   LCD, button, buzzer breakout with Teensy 3.5
   Also added headphone output by using tone()

  Added serial communication with nodeMCU to display
  the RSSI values on the website

   Date:   Feb 26, 2020
   Author: Jasper Yun
*/


#define buzzer     30  // buzzer on analog output
#define pButton    10  // connect one button to arduino pin A3 (break the message input loop)
#define tButton    24  // choose to output on headphones or buzzer
#define headphones 12  // headphones are connected to pin 7

#include <Arduino.h>
#include <LiquidCrystal.h>

boolean pause = false;  // booleans for hardware (button) interrupt functionality
boolean isBuzzer = true;

int previousRSSI; // for the pause functionality, remember last value of RSSI value received
int previousTone; // and the corresponding buzzer tone

String previousMessage; // strings for sending messages over serial
String message;

// variables for sending messages over serial
int arrayPosition = 0;
unsigned int sendPosition = 0;
int stringLength = 0;

// array of test RSSI values to iterate through to simulate receiving RSSI data from xtend
int test_RSSI[] = {-40, -45, -50, -55, -60, -65, -70, -75, -80, -85, -90, -95, -100, -105, -110};

// initialize the library by associating any needed LCD interface pin
// with the Teensy pin number it is connected to
const int rs = 4, en = 5, d4 = 6, d5 = 7, d6 = 8, d7 = 9;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// function prototypes
void pauseLCD();
void chooseHeadphones();
void welcome();
void printReading(int i, int sound);


/*
 * Setup function sets pin modes and welcomes the user with a message on the LCD.
 * Also sets up the interrupt pins and begins hardware serial 1 (RX1, TX1) at 115200 baud.
 */
void setup() {
  Serial1.begin(115200); // match baud rate of nodeMCU
  pinMode(pButton, INPUT);
  pinMode(tButton, INPUT);
  attachInterrupt(digitalPinToInterrupt(pButton), pauseLCD, FALLING);  // pause button interrupt
  attachInterrupt(digitalPinToInterrupt(tButton), chooseHeadphones, FALLING);
  pinMode(buzzer, OUTPUT);
  
  pinMode(headphones, OUTPUT);

  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Initializing...");
  delay(1000);
  lcd.setCursor(0, 1);
  lcd.print("Finished.");
  delay(1000);
  lcd.clear();
  welcome();
}

/*
 *  Main loop. Reads "input" from the test_RSSI[] array and maps it to a buzzer tone and sends
 * the RSSI value over serial to the nodeMCU.
 */
void loop() {
  if (pause) 
  {
    lcd.setCursor(0,1); // alert user that device is paused
    lcd.print("PAUSED");
    
    while (Serial1.availableForWrite() == 0) // write as many bytes as possible through serial
    {  
      if (sendPosition < previousMessage.length()) // if we have not reached the end of the message
      {
        Serial1.write((char) previousMessage[sendPosition]);
        sendPosition++;
      } 
      else 
      {
        Serial1.write('\n');  // end of message, append newline character
        Serial1.println();
        sendPosition = 0; // reset sendPosition for next message
        message = ""; // reset the message string
        break;  // stop the send process
      }
    }
    
    if (isBuzzer) // isBuzzer == true when we want to play the tone through the buzzer
    {
      tone(buzzer, previousTone); // play the previous tone for 1 second
      delay(1000);
      noTone(buzzer);
    } 
    else // otherwise, play the tone through the headphones
    {
      tone(headphones, previousTone);
      delay(1000);
      noTone(headphones);
    }
    
  } 

  else // if not paused
  {
    int reading = test_RSSI[arrayPosition]; // "read" new input
    int sound = map(reading, -110, -40, 125, 3000); // map reading to tone between 125 to 3000 Hz
    previousRSSI = reading; // update previous RSSI and tone values
    previousTone = sound;
    
    printReading(reading, sound); // prints the reading and the tone frequency on LCD
    
    message = (String) reading; // send the RSSI reading to the nodeMCU
    Serial1.print("RSSI value is:  ");
    Serial1.println(message);
    
    sendPosition = 0;
    while (Serial1.availableForWrite() == 0) 
    {
      if (sendPosition < message.length()) 
      {
        Serial1.write((char) message[sendPosition]);
        sendPosition++;
      } 
      
      else // end of message, append newline character
      {
        Serial1.write('\n');
        Serial1.println();
        sendPosition = 0;
        message = "";
        break;  // stop the send process
      }
    }

    if (isBuzzer) // play tone through the buzzer if true
    { 
      tone(buzzer, sound);  // play tone
      delay(500);
      noTone(buzzer);       // stop tone; required for buzzer to sound
      delay(500);
    } 
    else // play tone through headphones otherwise
    {
      tone(headphones, sound);
      delay(500);
      noTone(buzzer);
      delay(500);
      }
  } // end of else case (i.e. if not paused)
  
  // used to iterate through the array of test_RSSI values
  if (arrayPosition > 14) arrayPosition = 0;
  else arrayPosition++;
  
  delay(1000);
}

/*
 * Interrupt service routine. Flips the value of the boolean pause, which
 * is used in the main loop to stop reading from the RSSI receiver.
 */
void pauseLCD() {
  pause = !pause;
}

/* Interrupt service routine changes boolean isBuzzer to contol whether sound output
 * is on headphones or buzzer.
 */
void chooseHeadphones() {
  isBuzzer = !isBuzzer;
}

/*
 * Welcome the user upon startup. Prepares display for showing readings from
 * RSSI.
 */
void welcome() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Welcome!");
  delay(1500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Input:");
  lcd.setCursor(0, 1);
}

/*
 * Prints the reading to the LCD. Takes input parameters
 * of the RSSI value (int) and the buzzer frequency tone (int).
 */ 
void printReading(int RSSIvalue, int sound) {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Input: ");
  lcd.print(RSSIvalue);
  lcd.print(" dB");
  lcd.setCursor(0,1);
  lcd.print(sound);
}
