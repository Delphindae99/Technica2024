#include <Wire.h>
#include <LiquidCrystal.h>
const int potPin = A5;
const int redLedPin = 9;
const int greenLedPin = 8;
const int blueLedPin = 7;

int currentMillis,prevMillis;

byte colorVal;
int potValue;

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

#define SLAVE_ADDRESS 0x04

volatile boolean receiveFlag = false;
char temp[32];
String command;

void setup(){
  Wire.begin(SLAVE_ADDRESS);

  Wire.onReceive(receiveEvent);

  Serial.begin(9600);
  Serial.println("Ready!");

  lcd.begin(16, 2);

  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(blueLedPin, OUTPUT);
  analogWrite (redLedPin, 0);
  analogWrite(greenLedPin, 0);
  analogWrite(blueLedPin, 0);
}

void loop() {
  if (receiveFlag == true) {
    //Serial.println(temp);
    /*if(strcmp(temp, "END!")==0){
      lcd.clear();
      lcd.print("Ending");
      delay(1000);
      lcd.clear();
    analogWrite (redLedPin, 0);
    analogWrite(greenLedPin, 0);
    analogWrite(blueLedPin, 0);
    while(true){} //kill program
    }*/
     
    
    
    int month = (temp[0]-'0')*10 + (temp[1]-'0');
    Serial.println(month);
    Serial.println(temp[0]);
    int day = (temp[3]-'0')*10 + (temp[4]-'0');
    int year = (temp[6]-'0')*1000+(temp[7]-'0')*100+(temp[8]-'0')*10+(temp[9]-'0');

    int hour = (temp[11]-'0')*10 + (temp[12]-'0');
    if (hour == 0){
      hour = 12;
    } else if (hour > 12){
      hour -= 12;
    }
    int minute = (temp[14]-'0')*10 + (temp[15]-'0');

    lcd.clear();
    lcd.print(month);
    lcd.print("/");
    lcd.print(day);
    lcd.print("/");
    lcd.print(year);
    lcd.setCursor(0,1);
    lcd.print(hour);
    lcd.print(":");
    lcd.println(minute);

    
    
    receiveFlag = false;
  }

  writeLED();
}

void receiveEvent(int howMany) {
  for (int i = 0; i < howMany; i++){
    temp[i] = Wire.read();
    temp[i+1] = '\0';
  }

  for (int i = 0; i < howMany; ++i) temp[i] = temp[i+1];
  //Wire.write(colorVal);

  receiveFlag = true;
}

void writeLED (){
  int rawValue = analogRead(potPin);
  Serial.println(rawValue);
  colorVal = map(rawValue, 0, 1023, 0, 255);
  int wheelPos = 255 - colorVal;
  Serial.println(wheelPos);
  if (wheelPos < 85){
    analogWrite(redLedPin, 255 - wheelPos *3);
    analogWrite(greenLedPin, 0);
    analogWrite(blueLedPin, wheelPos * 3);
  } else if (wheelPos < 170){
    wheelPos -= 85;
    analogWrite(redLedPin, 0);
    analogWrite(greenLedPin, wheelPos *3);
    analogWrite(blueLedPin, 255 - wheelPos *3);
  } else {
    wheelPos -= 170;
    analogWrite(redLedPin, wheelPos * 3);
    analogWrite(greenLedPin, 255 - wheelPos * 3);
    analogWrite(blueLedPin, 0);
  }
}
