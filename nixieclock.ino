#include <Wire.h>
#include <DS3232RTC.h>
#include <Streaming.h> 

DS3232RTC myRTC;

int outputpins[] = {6,8,7};
int inputpins[]  = {4,5};
int out_pin_num  = 3;
int in_pin_num   = 2;
int in_loop, out_loop;

byte shiftarray[5]; // Shift array

void setup() {
  for (in_loop = 0; in_loop < in_pin_num; in_loop++){
    pinMode(inputpins[in_loop], INPUT);
  }
  for (out_loop = 0; out_loop < out_pin_num; out_loop++){
    pinMode(outputpins[out_loop], OUTPUT);
  }
  Serial.begin(9600);
    myRTC.begin();
    setSyncProvider(myRTC.get);
}
void loop() {
  static time_t tLast;
  time_t t;
  tmElements_t tm;
  int hours, minutes, seconds;
  int hour1, hour2, min1, min2, second1, second2;

  if (Serial.available() >= 12) { 
      // Code to set RTC time via Serial Monitor
      // Format: yy,mm,dd,hh,mm,ss

      tm.Year = Serial.parseInt() + 30;
      tm.Month = Serial.parseInt();
      tm.Day = Serial.parseInt();
      tm.Hour = Serial.parseInt();
      tm.Minute = Serial.parseInt();
      tm.Second = Serial.parseInt();

      t = makeTime(tm);
      myRTC.set(t);
      setTime(t);
  }


  hours   = hour();
  minutes = minute();


  if (hours == 0){
    hours = 12;
  }
  if (hours > 12){
    hours   -= 12;
  }
  
  hour1   = hours / 10;
  hour2   = hours % 10;
  min1    = minutes / 10;
  min2    = minutes % 10;
  


  display(hour1, hour2, min1, min2);
  delay(500);


}
int dec2bin(int exp){
  // Turn the time digits into 2^TIME_DIGIT as that is what
  // The shift registers need to show correct value
  int result = 1;
  result <<= exp;
  return result;
}
void prepshift(int value1, int value2, int value3, int value4){
  // Function takes 4 10-bit values and transforms them into
  // 5 8-bit values that can be shifted to registers

  shiftarray[0] = (value1);
  // Bits 0-7 of shiftarray[0] are bits 0-7 of value1
  shiftarray[1] = (value2 << 2) + ((value1 >> 8) & 3);
  // Bits 0-1 of shiftarray[1] are bits 8-9 of value1
  // Bits 2-7 of shiftarray[1] are bits 0-5 of value2
  shiftarray[2] = (value3 << 4) + ((value2 >> 6) & 15);
  // Bits 0-3 of shiftarray[2] are bits 6-9 of value2
  // Bits 4-7 of shiftarray[2] are bits 0-3 of value3
  shiftarray[3] = (value4 << 6) + ((value3 >> 4) & 63);
  // Bits 0-5 of shiftarray[3] are bits 4-9 of value3
  // Bits 6-7 of shiftarray[3] are bits 0-1 of value4
  shiftarray[4] = (value4 >> 2);
  // Bits 0-7 of shiftarray[0] are bits 2-9 of value4
}
void clear(){
  digitalWrite(outputpins[2], LOW);
  for(int i = 0; i < 5; i++){
    shiftOut(outputpins[0], outputpins[1], MSBFIRST, 0);
  }
  digitalWrite(outputpins[2], HIGH);
}
void display(int digit1, int digit2, int digit3, int digit4){
  clear();

  int adjust[10] = {1,0,9,8,7,6,5,4,3,2};
  // Layout Error with imported schematic, not the worst thing we can just change array
  int hour1_digit, hour2_digit, min1_digit, min2_digit; // Digits of the clock


  hour1_digit = dec2bin(adjust[digit1]);
  hour2_digit = dec2bin(adjust[digit2]);
  min1_digit  = dec2bin(adjust[digit3]);
  min2_digit  = dec2bin(adjust[digit4]);
  
  prepshift(hour1_digit, hour2_digit, min1_digit, min2_digit);

  digitalWrite(outputpins[2],LOW);
  shiftOut(outputpins[0],outputpins[1],MSBFIRST,shiftarray[4]);
  shiftOut(outputpins[0],outputpins[1],MSBFIRST,shiftarray[3]);
  shiftOut(outputpins[0],outputpins[1],MSBFIRST,shiftarray[2]);
  shiftOut(outputpins[0],outputpins[1],MSBFIRST,shiftarray[1]);
  shiftOut(outputpins[0],outputpins[1],MSBFIRST,shiftarray[0]);
  digitalWrite(outputpins[2],HIGH);
}
