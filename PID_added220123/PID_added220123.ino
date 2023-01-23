#define PWM_PIN 5
#define BUTTON_1 8
#define BUTTON_2 9
#define SENSOR_PIN A0
#define CURRENT_LIMIT 8

#include <ACS712.h>

ACS712 sensor(ACS712_30A, SENSOR_PIN); // Sensor decleration

int a = 0; // A dummy variable declared for the soft start to happen
float b = 0.0;
int curDuty = 0; // A variable that holds the current duty cycle (hold a value in the range of 0-255 for analogWrite)
double AmpThrough = 0.0; // The current passing through the motor
double RefAmp = 0.1; // Reference for control

// variables for PI control
unsigned long StartTime;
unsigned long EndTime;
float dt = 0.01;
double error = 0;
double integral = 0; 
int output = 0;
#define Kp 30  // Proportional gain
#define Ki 2  // Integral gain

void soft_start(){ // It increases the duty cycle each seconds by a small amount up to 60
  int ss_duty = 0; // A variable holding the soft start duty cycle (0-255)
  while(ss_duty < 60){ // The code below runs while the duty cycle is under 60
    delay(1000);
    ss_duty = ss_duty + 3; // Slightly increase the duty cycle at each loop
    ss_duty = constrain(ss_duty, 0, 61);  // Limit the duty cycle to be under %25
    
    analogWrite(PWM_PIN, ss_duty);
    curDuty = ss_duty; // The current duty cycle variable is updated
    a = curDuty;
  }
}

void on_off_control(){
  AmpThrough = sensor.getCurrentAC(980) - 0.1; // Read the rms value of the current

  if (RefAmp == 1.70 && AmpThrough < CURRENT_LIMIT)  // 
  {
    if (AmpThrough > (RefAmp + 0.08)){
      curDuty = constrain(curDuty - 2, 1, 191);  // Max %75 duty
    } 
    else if (AmpThrough < (RefAmp - 0.08)){
      curDuty = constrain(curDuty + 2, 1, 191);  // Max %75 duty
    }
    analogWrite(PWM_PIN, curDuty);
    print_variables();
    delay(400);
  } 
  else if (RefAmp == 1.30 && AmpThrough < CURRENT_LIMIT)  // 1 A
  {
    if (AmpThrough > (RefAmp + 0.05)){
      curDuty = constrain(curDuty - 2, 1, 191);  // Max %75 duty
    } 
    else if (AmpThrough < (RefAmp - 0.05)){
      curDuty = constrain(curDuty + 2, 1, 191);  // Max %75 duty
    }
    analogWrite(PWM_PIN, curDuty);
    print_variables();
    delay(300);
  } 
  else if (AmpThrough > CURRENT_LIMIT) {
    analogWrite(PWM_PIN, 0);  // Resets the duty cycle to zero
    delay(50000); // Wait for 10 seconds
    soft_start(); // Soft start again
  }
}

void pi_control(){
  StartTime = micros();
  
  AmpThrough = sensor.getCurrentAC(980) - 0.1; // Read the rms value of the current
  if (AmpThrough > CURRENT_LIMIT) {
    analogWrite(PWM_PIN, 0);  // Resets the duty cycle to zero
    delay(50000); // Wait for 10 seconds
    soft_start(); // Soft start again
  }
  
  error = RefAmp - AmpThrough; // Error is the difference between the reference and output current
  integral = integral + error*dt; // The definition of the integral term
  b = 60+Kp*error + Ki*integral; // PI control
  b = constrain(b, 1, 191);
  curDuty = int(round(b));

  delay(50);
  analogWrite(PWM_PIN, curDuty);
  print_variables();
  
  EndTime = micros();
  dt = abs(EndTime - StartTime)/1000000.00; // Find the step time
  
}

void print_variables(){
  Serial.print(RefAmp); // Print RefAmp
  Serial.print('\t');
  Serial.print(AmpThrough); // Print AmpThrough
  Serial.print('\t');
  Serial.print(curDuty / 255.0); // Print curDuty
  Serial.print('\n');
}

void setup() {
  pinMode(PWM_PIN, OUTPUT); //PWM pin
  pinMode(BUTTON_1, INPUT); //BUTON 1
  pinMode(BUTTON_2, INPUT); //BUTON 2
  Serial.begin(9600);
  analogWrite(PWM_PIN, 0);
  // If you are not sure that the current through the sensor will not leak during calibration - comment out this method
  sensor.calibrate();
  delay(100);
}

void loop() {
  if (a<60){
    delay(20000);
    soft_start();
    print_variables();
    AmpThrough = sensor.getCurrentAC(980) - 0.1; // Read the rms value of the current
    RefAmp = AmpThrough;
  }
  else{
    if (digitalRead(BUTTON_1) == 1){  // Button 1 is pressed
      RefAmp = 1.70;
    }
    if (digitalRead(BUTTON_2) == 1){  // Button 2 is pressed
      RefAmp = 1.30;
    }
    /* Comment out the control method that you don't want to use */
    
    //on_off_control();
    pi_control();
  }
}
