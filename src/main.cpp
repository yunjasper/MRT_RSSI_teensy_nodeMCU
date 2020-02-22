/*
   McGill Rocket Team - Ground Station Receiver
   LCD, button, buzzer breakout with Teensy 3.5
   Also added headphone output by using tone()

  Added serial communication with nodeMCU to display
  the RSSI values on the website

   Date:   Feb 20, 2020
   Author: Jasper Yun
*/


#define buzzer     30  // buzzer on analog output
#define pButton    10  // connect one button to arduino pin A3 (break the message input loop)
#define tButton    24  // choose to output on headphones or buzzer
#define headphones 12  // headphones are connected to pin 7

#include <Arduino.h>
#include <LiquidCrystal.h>
#include <math.h> // for exponentials


boolean pause = false;
boolean isBuzzer = true;

int previousRSSI;
int previousTone;
String previousMessage;
String message;

int arrayPosition = 0;
unsigned int sendPosition = 0;
int stringLength = 0;

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
 *  Main loop. Reads potentiometer and adjusts that output to the buzzer frequency.
 */
void loop() {
  if (pause) {
    lcd.setCursor(0,1);
    lcd.print("PAUSED");
    
    while (Serial1.availableForWrite() == 0) {
      if (sendPosition < previousMessage.length()) {
        Serial1.write((char) previousMessage[sendPosition]);
        //Serial.print(previousMessage[sendPosition]);
        sendPosition++;
      } else {
        Serial1.write('\n');
        Serial1.println();
        sendPosition = 0;
        message = "";
        break;  // stop the send process
      }
     
      
    }
    
    if (isBuzzer) {
      tone(buzzer, previousTone);
      delay(1000);
      noTone(buzzer);
    } else {
      tone(headphones, previousTone);
      delay(1000);
      noTone(headphones);
    }
    
  } else {
    int reading = test_RSSI[arrayPosition];
    int sound = map(reading, -110, -40, 125, 3000);
    previousRSSI = reading; // update previous RSSI and tone values
    previousTone = sound;
    
    printReading(reading, sound);
    
    message = (String) reading;
    Serial1.print("Message is :  ");
    Serial1.println(message);
    sendPosition = 0;
    while (Serial1.availableForWrite() == 0) {
      if (sendPosition < message.length()) {
        Serial1.write((char) message[sendPosition]);
        //Serial.print(message[sendPosition]);
        sendPosition++;
      } else {
        Serial1.write('\n');
        Serial1.println();
        sendPosition = 0;
        message = "";
        break;  // stop the send process
      }
     
      
    }

    if (isBuzzer) {
      tone(buzzer, sound);  // play tone
      delay(500);
      noTone(buzzer);       // stop tone; required for buzzer to sound
      delay(500);
    } else {
      tone(headphones, sound);
      delay(500);
      noTone(buzzer);
      delay(500);
      }
  }
  
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

/* Interrupt service routine changes boolean to contol whether sound output
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
  lcd.setCursor(0, 1);
  lcd.print("Turn 10k pot");
  delay(1000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Input read:");
  lcd.setCursor(0, 1);
  return;
}

void printReading(int i, int sound) {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Input read: ");
  lcd.print(i);
  lcd.print(" dB");
  lcd.setCursor(0,1);
  lcd.print(sound);
}
