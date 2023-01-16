#define PWM_PIN 5
#define BUTTON_1 8
#define BUTTON_2 9
#define SENSOR_PIN A0
#define CURRENT_LIMIT 15

#include <ACS712.h>

int b;
int curDuty; // A variable that holds the current duty cycle (hold a value in the range of 0-255 for analogWrite)
int mVperAmp = 66;  // The sensing accuracy of the 30A module
int SensorVout; // The raw sensor output (analogRead's output is between 0-1023)
double AmpThrough; // The current passing through the motor
double RefAmp; // Reference for control

void soft_start(){ // It increases the duty cycle each seconds by a small amount up to 60
  int ss_duty = 0; // A variable holding the soft start duty cycle (0-255)
  while(ss_duty < 60){ // The code below runs while the duty cycle is under 60
    delay(1000);
    ss_duty = ss_duty + 3; // Slightly increase the duty cycle at each loop
    ss_duty = constrain(ss_duty, 0, 61);  // Limit the duty cycle to be under %25
    Serial.println(ss_duty);
    analogWrite(PWM_PIN, ss_duty);
    curDuty = ss_duty; // The current duty cycle variable is updated
  }
}

void setup() {
  pinMode(PWM_PIN, OUTPUT);  //PWM pin
  pinMode(BUTTON_1, INPUT);   //BUTON 1
  pinMode(BUTTON_2, INPUT);   //BUTON 2
  Serial.begin(9600);

  soft_start();  
}

void current_rms_read(){ // Measures the current and calculates its rms value
  SensorVout = analogRead(SENSOR_PIN); // The sensor output is read (0-1023)
  AmpThrough = abs((((SensorVout / 1023.0) * 5000) - 2500) / mVperAmp) - 0.05; // The calculation for the current is done according to the datasheet of ACS712
}

void loop() {
  current_rms_read();

  // ON_OFF_control
  current_rms_read();

  if (digitalRead(BUTTON_1) == 1)  // Button 1 is pressed
  {
    RefAmp = 1.5;
  }
  if (digitalRead(BUTTON_2) == 1)  // Button 2 is pressed
  {
    RefAmp = 1;
  }
  if (RefAmp == 1.5 && AmpThrough < CURRENT_LIMIT)  // 
  {
    if (AmpThrough > (RefAmp + 0.08)) {
      b = curDuty;
      b = b - 2;
      b = constrain(b, 1, 191);  // Max %75 duty
      curDuty = b;
      Serial.println(b);
      Serial.println(RefAmp);
      Serial.println(AmpThrough);
      analogWrite(PWM_PIN, b);
      delay(400);
    } else if (AmpThrough < (RefAmp - 0.08)) {
      b = curDuty;
      b = b + 2;
      b = constrain(b, 1, 191);  // Max %75 duty
      curDuty = b;
      Serial.println(b);
      Serial.println(RefAmp);
      Serial.println(AmpThrough);
      analogWrite(PWM_PIN, b);
      delay(400);
    }
  } 
  else if (RefAmp == 1 && AmpThrough < CURRENT_LIMIT)  // 1 A
  {
    if (AmpThrough > (RefAmp + 0.05)) {
      b = curDuty;
      b = b - 2;
      b = constrain(b, 1, 191);  // Max %75 duty
      curDuty = b;
      Serial.println(b);
      Serial.println(RefAmp);
      Serial.println(AmpThrough);
      analogWrite(PWM_PIN, b);
      delay(300);
    } else if (AmpThrough < (RefAmp - 0.05)) {
      b = curDuty;
      b = b - 2;
      b = constrain(b, 1, 191);  // Max %75 duty
      curDuty = b;
      Serial.println(b);
      Serial.println(RefAmp);
      Serial.println(AmpThrough);
      analogWrite(PWM_PIN, b);
      delay(300);
    }
  } 
  else if (AmpThrough > CURRENT_LIMIT) {
    analogWrite(PWM_PIN, 0);  // Resets the duty cycle to zero
    delay(10000); // Wait for 10 seconds
    soft_start(); // Soft start again
}