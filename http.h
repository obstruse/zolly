#ifndef HTTP_H
#define HTTP_H
#include <ESP8266WebServer.h>

ESP8266WebServer server ( 80 );
#endif

//--------------------------------------------
void handleRoot() {

  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;
        
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.sendHeader("Content-Type","text/html",true);
  server.sendHeader("Cache-Control","no-cache");
  server.send(200);
   
//--------------------------------------------
  sprintf ( temp,
"<html>\
  <head>\
    <title>Zolly</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>Zolly</h1>"
  );
  server.sendContent ( temp );

//--------------------------------------------
  sprintf ( temp,
"<form action='/setPosition'>\
<table border='1' cellpadding='5'>\
<tr><th colspan='2'>Location</th></tr>\
<tr><td>Camera</td><td><input type='number' min='4' max='44' step='0.0001' name='locationInches' value='%s'></td></tr>\
<tr><td>Object</td><td><input type='number' min='48'         step='0.0001' name='objectInches' value='%s'></td></tr>\
<tr><td>Zoom</td><td><input type='number' min='0' max='100'                name='zoomStep' value='%d'></td></tr>\
<tr><td colspan='2'><button type='submit' name='submit'>Submit</button></td></tr>\
</form>\
</table><br>\
",
  String(horizStep/stepsInch,4).c_str(),
  String(objectStep/stepsInch,4).c_str(),
  zoomStep
  );
  server.sendContent ( temp );
  
//--------------------------------------------
  sprintf ( temp,
"<form action='/autoMove'>\
<table border='1' cellpadding='5'>\
<tr><td><button type='submit' name='zoom'  value='-1' >%s</button></td></tr>\
</table>\
</form>",
trackerRun ? "Stop" : "Zoom In");
  server.sendContent ( temp );

//--------------------------------------------
  sprintf ( temp,
"<form action='/releaseStepper'>\
<table border='1' cellpadding='5'>\
<tr><td><button type='submit' name='release'>Release</button></td></tr>\
</table>\
</form>"
  );
  server.sendContent ( temp );

//--------------------------------------------
/*  
sprintf ( temp,
"<table border='1' cellpadding='5'>\
<tr><td>Starting Zoom</td><td>%d</td><td>%smm</td></tr>\
<tr><td>Ending Zoom</td><td>%d</td><td>%smm</td></tr>\
<tr><td>Starting Pos</td><td>%d</td><td>%sin</td></tr>\
<tr><td>Ending Pos</td><td>%d</td><td>%sin</td></tr>\
</table>",
startingZoomStep,String(zoomFocal[startingZoomStep]/1000.0,2).c_str(),
zoomStep,String(zoomFocal[zoomStep]/1000.0,2).c_str(),
startingStep,String(startingStep/stepsInch,2).c_str(),
horizMove,String(horizMove/stepsInch,2).c_str() );
  server.sendContent ( temp );
*/
//--------------------------------------------
/*  
sprintf ( temp,
"<pre>horizStep=%s\nzoomStep=%s\nzoomFocal=%s\nobjectStep=%s\nhorizMove=%s\nstartingZoomStep=%s\nzoomConstant=%s\n</pre>",
    String(horizStep).c_str(),
    String(zoomStep).c_str(),
    String(zoomFocal[zoomStep]).c_str(),
    String(objectStep).c_str(),
    String(horizMove).c_str(),
    String(startingZoomStep).c_str(),
    String(zoomConstant,4).c_str() );
  server.sendContent ( temp );
*/
  sprintf ( temp,
"<br><p>Uptime: %02d:%02d:%02d</p>\
    <p>%s/%s %s %s</p>\
    </body></html>",
    hr, min % 60, sec % 60,
    pathName, fileName, __DATE__, __TIME__);
  server.sendContent ( temp );

  server.sendContent (" ");

  }

//--------------------------------------------
void setPosition() {
  int inArgs = server.args();
  int numer, denom = 0;
  float locationInches = 0;

  
  for ( int i = 0; i < inArgs; i++ ) {
    if ( server.argName(i) == "locationInches" ) {
      horizStep = server.arg(i).toFloat() * stepsInch;
    }
    if ( server.argName(i) == "objectInches" ) {
      objectStep = server.arg(i).toFloat() * stepsInch;
    }
    if ( server.argName(i) == "zoomStep" ) {
      zoomStep = server.arg(i).toInt();
    }
  }

  horizMove = horizStep;

  // refresh root
  server.sendHeader("Location", String("/"), true);
  server.send ( 302, "text/plain", "");
  
}


//--------------------------------------------
void releaseStepper() {

  stepperRelease();

  // 204: No Content
  //      The server successfully processed the request and is not returning any content
  server.send ( 204, "text/plain", "");

}

//--------------------------------------------
void autoMove() {

  //trackerRun = true;
  trackerRun = !trackerRun;

  int inArgs = server.args();

  for ( int i = 0; i < inArgs; i++ ) {
    if ( server.argName(i) == "zoom" ) {
      zoomInOut = server.arg(i).toInt();
    }
  }


  
  horizMove = horizStep;
  startingStep = horizStep;
  startingZoomStep = zoomStep;
  
  // 204: No Content
  //      The server successfully processed the request and is not returning any content
  //server.send ( 204, "text/plain", "");
  
  // refresh root
  server.sendHeader("Location", String("/"), true);
  server.send ( 302, "text/plain", "");

}



//--------------------------------------------
void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }

  server.send ( 404, "text/plain", message );
}


//--------------------------------------------
void httpSetup() {
/******************** HTTP ***********************/
  server.on ( "/", handleRoot );
  server.on ( "/setPosition", setPosition );
  server.on ( "/releaseStepper", releaseStepper );
  server.on ( "/autoMove", autoMove );
  server.onNotFound ( handleNotFound );
  server.begin();
/*************************************************/
Serial.println("http started");
}
