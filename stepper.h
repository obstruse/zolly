#include <Wire.h>
#include <Adafruit_MotorShield.h>

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_StepperMotor *stepper[2];

//float stepsInch = 66.19;
const float stepsInch = 66.676;             // 0.0149979 inchesStep
const float stepsUM   = stepsInch / 25.4 / 1000.0;     // zoomFocal in micrometers.  0.3809 mmStep, 380.9 umStep
//float stepsInch = 132.38;                 // if INTERLEAVED (high resolution, but slow)
int horizMinimum = stepsInch * 4.0;
int horizMaximum = stepsInch * 44.0;
const int rotateMaximum = 200 * 27;         // 200 steps per revolution, 27:1 worm gear

const float stepsDegree = 15;
//float stepsDegree = 30;

int horizStep = 0;
int horizMove = 0;
int rotateStep = -1;
int rotateMove = -1;

const int stepperHoriz = 1;
const int stepperRotate = 0;

//--------------------------------------------
void stepperMove(int stepperNumber, int Steps, int Type=SINGLE) {

  // MICROSTEP doesn't jolt the shuttle as much as DOUBLE
  // SINGLE sometimes has a stutter at start,drops some steps?
  stepper[stepperNumber]->step(abs(Steps), Steps > 0 ? FORWARD : BACKWARD, Type);
  //stepper[stepperNumber]->onestep(Steps > 0 ? FORWARD : BACKWARD, Type);
  
}
  
//--------------------------------------------
void stepperRelease() {
  stepper[stepperHoriz]->release();
  stepper[stepperRotate]->release();  
}

//--------------------------------------------
void stepperSetup() {
  
  AFMS.begin();  // create with the default frequency 1.6KHz
  //AFMS.begin(1000);  // OR with a different frequency, say 1KHz
  
  //TWBR = ((F_CPU /400000l) - 16) / 2; // Change the i2c clock to 400KHz

  // stepper[0] by the power connection (M1/M2)
  
  stepper[stepperRotate] = AFMS.getStepper(200, 1);
  stepper[stepperRotate]->setSpeed(20);
  stepper[stepperHoriz] = AFMS.getStepper(200, 2);
  stepper[stepperHoriz]->setSpeed(20);
  
  
}
