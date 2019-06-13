#include <Arduino.h>
#ifdef ESP32
    #include <WiFi.h>
#else
    #include <ESP8266WiFi.h>
#endif
#include "fauxmoESP.h"
#include "Adafruit_Crickit.h"
#include "seesaw_servo.h"
 
#define WIFI_SSID "****"  		// put in your WiFi SSID 
#define WIFI_PASS "****"  		// Put in your WiFi Password
#define SERIAL_BAUDRATE 115200

bool resetMe=false;
 
fauxmoESP fauxmo;
Adafruit_Crickit crickit;
seesaw_Servo myservo(&crickit);  // create servo object to control a servo
 
// -----------------------------------------------------------------------------
// Wifi
// -----------------------------------------------------------------------------
 
void wifiSetup() {
 
    // Set WIFI module to STA mode
    WiFi.mode(WIFI_STA);
 
    // Connect
    Serial.printf("[WIFI] Connecting to %s ", WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
 
    // Wait
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(100);
    }
    Serial.println();
 
    // Connected!
    Serial.printf("[WIFI] STATION Mode, SSID: %s, IP address: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
}
 
void setup() {
  // Init serial port and clean garbage
  Serial.begin(115200);
  
  if(!crickit.begin()){
    Serial.println("ERROR!");
    while(1);
  }
  else Serial.println("Crickit started");
  
  myservo.attach(CRICKIT_SERVO1);  // attaches the servo to CRICKIT_SERVO1 pin
   
  Serial.begin(SERIAL_BAUDRATE);
  Serial.println("FauxMo demo sketch");
  Serial.println("After connection, ask Alexa/Echo to 'turn <devicename> on' or 'off'");

  
 
  // Wifi
  wifiSetup();
 
  // Fauxmo
  fauxmo.addDevice("Switch");
    // Gen3 Devices or above
  fauxmo.setPort(80);
 
  // Allow the FauxMo to be discovered
  fauxmo.enable(true);
  
  fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state, unsigned char value) {
    Serial.print("Device: ");Serial.print(device_name);
    Serial.print(" state");
    if(state) {
      Serial.println(" ON!");
      myservo.write(160);
      resetMe=true;
       }
    else {
      Serial.println(" off");
      myservo.write(60);
      resetMe=true;
    }
   });
 
}
 
/* ---------------------------------------------------------------------------
 Device Callback
 ----------------------------------------------------------------------------*/
void callback(uint8_t device_id, const char * device_name, bool state) 
{
  Serial.print("Device "); Serial.print(device_name); 
  Serial.print(" state: ");
  if (state) 
  {
    Serial.println("ON");
  } 
  else 
  {
    Serial.println("OFF");
  }
}
  //Switching action on detection of device name, useful for adding
  //multiple "devices" to a single ESP unit.
  
void loop() 
{
  if(resetMe){
    delay(1000);
    Serial.println("reset the servo");
    myservo.write(105);
    delay(1000);
    resetMe=false;
    Serial.println("Servo Reset");
  }
  fauxmo.handle();
}
