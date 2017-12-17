//≤≥≤≥≤≥≤≥≤≥≤≥≤≥≤≥≤≥≤≥≤≥≤≥≤≥≤≥≤≥≤≥≤≥≤≥≤≥≤≥≤≥≤≥≤≥≤≥≤≥≤≥
//Auto Curtains                                         
//Created by Mert Türel on 28/04/2017.              
//≤≥≤≥≤≥≤≥≤≥≤≥≤≥≤≥≤≥≤≥≤≥≤≥≤≥≤≥≤≥≤≥≤≥≤≥≤≥≤≥≤≥≤≥≤≥≤≥≤≥≤≥

#include <ESP8266WiFi.h>
#include <Firebase.h>
#include <FirebaseArduino.h>
#include <FirebaseCloudMessaging.h>
#include <FirebaseError.h>
#include <FirebaseHttpClient.h>
#include <FirebaseObject.h>

#define FIREBASE_HOST "Your Firebase Host URL"
#define FIREBASE_AUTH "Your Firebase Auth Key"

#define wifiName "Your WiFi SSID"
#define wifiPass "Your WiFi Pass"

//Pins On NodeMcu 
#define step_pin D1
#define dir_pin D5
#define MS1 D3
#define MS2 D2
#define slp_pin D4

int steps;
int firstValue = 0;

void setupFirebase() {

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  if (Firebase.failed()) {
    Serial.println("FAIL begin");
    Serial.println(Firebase.error());
  }
 
}

void setupWifi() {

  WiFi.begin(wifiName, wifiPass);

  Serial.println("Connecting...");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.println(".");
    delay(500);
  }

  Serial.println();
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


void setup() {

  Serial.begin(9600);
  
  pinMode(MS1, OUTPUT);
  pinMode(MS2, OUTPUT);
  pinMode(dir_pin, OUTPUT);
  pinMode(step_pin, OUTPUT);
  pinMode(slp_pin, OUTPUT);

  digitalWrite(slp_pin, LOW);
  
  setupWifi();
  setupFirebase();
  
  digitalWrite(slp_pin, HIGH);
  delay(5);
  
  digitalWrite(MS1, LOW);
  digitalWrite(MS2, LOW);

}

void getData() {

  //Sleep
  digitalWrite(slp_pin, LOW);
  String path = "";
  FirebaseObject object = Firebase.get(path);
  if (Firebase.failed()) {
    Serial.println("FAIL obj");
    Serial.println(Firebase.error());
  }
  
  int curtainValue = object.getInt("step value on firebase");
  bool isPowerSaveMode = object.getBool("power save mode value on firebase");
  
  if (Firebase.failed()) {
    Serial.println("FAIL curtain");
    Serial.println(Firebase.error());
  }
    
  steps = abs(curtainValue - firstValue);

  if(curtainValue > firstValue){
    digitalWrite(slp_pin, HIGH);
    for(int i = 0; i < steps; i++){
      digitalWrite(dir_pin, LOW); //Turn Clockwise...
      digitalWrite(step_pin, HIGH);
      delay(1);
      digitalWrite(step_pin, LOW);
      delay(1);
    }
  }else if(curtainValue < firstValue){
    digitalWrite(slp_pin, HIGH);
    for(int i = 0; i < steps; i++){
      digitalWrite(dir_pin, HIGH); //Turn Anti-Clockwise...
      digitalWrite(step_pin, HIGH); 
      delay(1);
      digitalWrite(step_pin, LOW);
      delay(1); 
    } 
  }else{
    
      Serial.println("NOTR");
      delay(300);
      
      if(isPowerSaveMode)
      {
        delay(3000);
      }
  }
  
  firstValue = curtainValue;
}
void loop() {

  getData();

}
