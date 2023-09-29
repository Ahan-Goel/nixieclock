#include <Arduino.h>
#include <Wire.h>
#include <DS3231.h>
#include <header.h>

DS3231 clock;
RTCDateTime dt;

int outputpins[] = {3,4,5,6,7,8,9,10,11,12};
int inputpins[]  = {4,5};
int out_pin_num  = 10;
int in_pin_num   = 2;
int in_loop, out_loop;
int shiftarray[4][10];


void setup() {
  // put your setup code here, to run once:
  for (in_loop = 0; in_loop < in_pin_num; in_loop++){
    pinMode(inputpins[in_loop], INPUT);
  }
  for (out_loop = 0; out_loop < out_pin_num; out_loop++){
    pinMode(outputpins[out_loop], OUTPUT);
  }
  Serial.begin(9600);
  clock.begin();
  // Set sketch compiling time
  clock.setDateTime(__DATE__, __TIME__);
}

void loop() {
  // put your main code here, to run repeatedly:
  int i, hours, minutes, seconds, ledout, hour1, hour2, min1, min2;
  int hour1_digit, hour2_digit, min1_digit, min2_digit;

  dt = clock.getDateTime();
  hours   = dt.hour;
  minutes = dt.minute;
  seconds = dt.second;

  ledout = minutes % 10;

  for (i=0; i<=9; i++){
    if (ledout == i){
      digitalWrite(outputpins[9-i], HIGH);
    }
    else{
      digitalWrite(outputpins[9-i], LOW);
    }
  }

  hour1   = hours / 10;
  hour2   = hours % 10;
  min1    = minutes / 10;
  min2    = minutes % 10;
  
  if ((hour1 == 0) && (hour2 == 0)){
    hour1 = 1;
    hour2 = 2;
  }
  // These two if statements turn 24hr time into 12hr time
  if ((hour1 + hour2) > 3){
    hours   -= 12;
    hour1   = hours / 10;
    hour2   = hours % 10;
  }

  hour1_digit = dec2bin(hour1);
  hour2_digit = dec2bin(hour2);
  min1_digit  = dec2bin(min1);
  min2_digit  = dec2bin(min2);

  Serial.print(hours);
  Serial.print(":");
  Serial.print(minutes);
  Serial.print(":");
  Serial.print(seconds);
  Serial.print(" Digits to be shifted: ");
  Serial.print(hour1_digit);
  Serial.print(":");
  Serial.print(hour2_digit);
  Serial.print(":");
  Serial.print(min1_digit);
  Serial.print(":");
  Serial.println(min2_digit);


  // make the code so that we can serial shift out 4 times and each shift out number
  // is 2^(digit) = digit
  delay(5);
}
int dec2bin(int exp)
{
    int result = 1;
    result <<= exp;
    return result;
}

