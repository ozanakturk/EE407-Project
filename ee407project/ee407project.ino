#define PWM_PIN 5
#define BUTTON_1 8
#define BUTTON_2 9
#define SENSOR_PIN A0
#define CURRENT_LIMIT 15

#include <ACS712.h>

ACS712 sensor(ACS712_30A, A0); // Sensor decleration

int curDuty = 0; // A variable that holds the current duty cycle (hold a value in the range of 0-255 for analogWrite)
double AmpThrough = 0.0; // The current passing through the motor
double RefAmp = 0.0; // Reference for control

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

  // If you are not sure that the current through the sensor will not leak during calibration - comment out this method
  Serial.println("Calibrating... Ensure that no current flows through the sensor at this moment");
  sensor.calibrate();
  Serial.println("Done!");

  soft_start();
}

void loop() {
  AmpThrough = sensor.getCurrentAC(); // Read the rms value of the current

  // ON_OFF_control
  if (digitalRead(BUTTON_1) == 1){  // Button 1 is pressed
    RefAmp = 1.5;
    Serial.println(String("New Reference Value: ") + RefAmp); // Print RefAmp
  }
  if (digitalRead(BUTTON_2) == 1){  // Button 2 is pressed
    RefAmp = 1;
    Serial.println(String("New Reference Value: ") + RefAmp); // Print RefAmp
  }

  if (RefAmp == 1.5 && AmpThrough < CURRENT_LIMIT)  // 
  {
    if (AmpThrough > (RefAmp + 0.08)){
      curDuty = constrain(curDuty - 2, 1, 191);  // Max %75 duty
    } 
    else if (AmpThrough < (RefAmp - 0.08)){
      curDuty = constrain(curDuty + 2, 1, 191);  // Max %75 duty
    }
    analogWrite(PWM_PIN, curDuty);
    Serial.print(String("Reference Value: ") + RefAmp); // Print RefAmp
    Serial.println(String("\nCurrent Value: ") + AmpThrough); // Print AmpThrough
    Serial.println(String("\nDuty Cycle: ") + curDuty / 255); // Print curDuty (0-1)
    delay(400);
  } 
  else if (RefAmp == 1 && AmpThrough < CURRENT_LIMIT)  // 1 A
  {
    if (AmpThrough > (RefAmp + 0.05)){
      curDuty = constrain(curDuty - 2, 1, 191);  // Max %75 duty
    } 
    else if (AmpThrough < (RefAmp - 0.05)){
      curDuty = constrain(curDuty + 2, 1, 191);  // Max %75 duty
    }
    analogWrite(PWM_PIN, curDuty);
    Serial.print(String("Reference Value: ") + RefAmp); // Print RefAmp
    Serial.println(String("\nCurrent Value: ") + AmpThrough); // Print AmpThrough
    Serial.println(String("\nDuty Cycle: ") + curDuty / 255); // Print curDuty (0-1)
    delay(300);
  } 
  else if (AmpThrough > CURRENT_LIMIT) {
    analogWrite(PWM_PIN, 0);  // Resets the duty cycle to zero
    delay(10000); // Wait for 10 seconds
    soft_start(); // Soft start again
  }
}