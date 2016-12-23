#include "blynk/blynk.h"

char auth[] = "e3ac4c915633434dbcb322fd5587f501";
const int reedSwitch = D0;
const int relay = D3;

void setup() {
    Serial.begin(9600);
    Blynk.begin(auth);
    
    pinMode(reedSwitch, INPUT);    
    pinMode(relay, OUTPUT);
    
    Particle.function("activateDoor", activateDoor);
    Particle.function("publishDoorStatus", publishDoorStatus);
}

int activateDoor(String args){
  Particle.publish("door activated by",args);
  completeActionAndPublishStatus();
  waitForFiveMin();
  if(isDoorOpen()){
      completeActionAndPublishStatus();
  }
}

int publishDoorStatus(String args){
    if(isDoorOpen()){
        Particle.publish("door status", "open");
    }
    else{
        Particle.publish("door status", "close");
    }
    return 0;
}

void completeActionAndPublishStatus(){
    activateRelay();
    waitForOneMin();
    publishDoorStatus("");    
}

void activateRelay(){
    digitalWrite(relay, HIGH);
    delay(2000); // delay for 2 sec
    digitalWrite(relay, LOW);
}

void waitForOneMin(){
    delay(60000);    
}

void waitForFiveMin(){
    delay(300000);    
}

bool isDoorOpen(){
    int reedStatus = 0;
    reedStatus = digitalRead(reedSwitch);
    if (reedStatus == HIGH) {
        return true;
    }
    else {
        return false;
    }
}

BLYNK_WRITE(V1) //Button Widget is writing to pin V1
{
  activateDoor("blynk");
}

void loop() {
    Blynk.run();
}