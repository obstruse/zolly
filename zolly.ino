#include <Time.h>
#include <math.h>

char fileName[] = __FILE__;

// in the hardware subfolder of the Arduino IDE installation folder, create platform.txt:
//       compiler.cpp.extra_flags=-D__PATH__="{build.source.path}"
char pathName[] = __PATH__;

// pins 
#define APDS9960_INT    16  // can I poll the interrupt pin? instead of generating an interrupt?   // gesture int  // not 16       // not 15
#define SDA             2   // I2C          // not 16       // not 15
#define LED_PIN         4  // DotStar      // pulls high?  // not 15  // not 7==TX  // not 1==TX
#define CLOCK_PIN       5   // Dot Star     // pulls high?  // not 15 
#define MISO            12  // SPI          // not moveable
#define MOSI            13  // SPI          // not moveable
#define SCK             14  // SPI          // not moveable
#define SS              15  // chip select ADC  // should work on 15
#define SCL             0   // I2C          // not 16       // not 15 (pullup)




char temp[1000];
boolean trackerRun = false;
boolean constantAngular = false;

time_t frameTime = 1;
time_t frameInc = 1;

/********************************************************************************/

#include "stepper.h"
#include "zoomFocal.h"

const double pi = 3.14159265;
int totalFrames = 125;  
float totalMinutes = 1;
int stepsPerFrame;  // will be calculated later autoMove(in http.h)
int rotsPerFrame;    // will be calculated later autoMove(in http.h)
float targetHeight = 10.0;
const double targetOrigin = 24.0;
double targetDistance;
double targetBase;
double targetAngle;

int objectStep = 0;
int startingStep = 0;
int startingZoomStep = 0;

#include "wifi.h"
#include "http.h"
#include "httpWifi.h"

//-----------------------------------------
void setup()  
{
  Serial.begin(9600);
  
  wifiSetup();
  
  httpSetup();
  httpWifiSetup();

  stepperSetup();

  // shutter pin
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW); 
    
//  Serial.print ("Server started.");

//  Serial.println("setup finished");

}

//----------------------------------------------
void loop() 
{
  
  server.handleClient();
  //ArduinoOTA.handle();
  wifiOTA();
  wifiDNS();

  // don't do anything unless horizStep and rotateStep have been set
  if (horizStep <= 0) {
    return;   // loop will restart
  }
  
  // make sure position fits within margins?
  // margins
  if ( horizMove < horizMinimum ) horizMove = horizMinimum; 
  if ( horizMove > horizMaximum ) horizMove = horizMaximum;
      

  //---------------------------------------------auto---------------------------------------
  if (trackerRun) {                                                       // automatic mode
    // check if horiz move
    if (horizMove != horizStep) {                                              // There's a horiz move pending
      
      if ( horizMove > horizStep ) {
        stepperMove(stepperHoriz, 1, MICROSTEP);
        horizStep++;
      } else {
        stepperMove(stepperHoriz, -1, MICROSTEP);
        horizStep--;
      }
      
    } else {
      // horiz and rotate done, check if time (in milliseconds) for frame
      //... or not
              
      // settling time.  
      delay(100);
      // shoot, and step camera to next zoom
      digitalWrite(2, HIGH);
      delay(100);
      digitalWrite(2, LOW);
      // give it plenty of time to complete the shot
      delay(3000);

      // camera has already stepped to next zoom, so setup camera position to match
      // don't mess with zoomStep unless the calculated move can proceed
      int newZoomStep = zoomStep + zoomInOut;

      if ( newZoomStep >= 0 && zoomFocal[newZoomStep] > 0 ) {
        // camera starts at startingPosition, horizontal steps from origin (input as inches from origin)
        //                  startingZoomStep
        // object is at objectPosition, horizontal steps from origin (input as inches from origin)

        // zoomConstant = ratio of initial camera-object distance / initial focal length
        //zoomConstant = double((objectStep - startingStep)) / double(zoomFocal[startingZoomStep]);
        // well, actually lens-object distance / initial focal length
        int lensStep = startingStep + (zoomFocal[startingZoomStep] * stepsUM);    // location of lens at start, focal length ahead of camera
        zoomConstant = double( ( objectStep - lensStep ) ) / double(zoomFocal[startingZoomStep]);
        
        // new camera-object distance = new focal length * zoomConstant
        // since the object doesn't move, new camera position is object location - camera-object distance
        // OK, lens/object distance = new focal length * zoomConstant
        // 
        lensStep = objectStep - (zoomFocal[newZoomStep] * zoomConstant);
        horizMove = lensStep - (zoomFocal[newZoomStep] * stepsUM);                // camera located focal length behind lens
        
      } else {
        horizMove = -1;
      }

      if ( horizMove > horizMaximum || horizMove < horizMinimum ) {
        // reached the end, shut it down
        horizMove = horizStep;
        trackerRun = false;
      } else {
        zoomStep = newZoomStep;
      }
    }        
  }
  
}



       
