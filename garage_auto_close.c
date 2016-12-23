// This #include statement was automatically added by the Particle IDE.
#include "SparkCorePolledTimer/SparkCorePolledTimer.h"

// This #include statement was automatically added by the Particle IDE.
#include "blynk/blynk.h"

char auth[] = "e3ac4c915633434dbcb322fd5587f501";

const int reedSwitch = D0;
const int relay = D3;
int reedStatus = 0;
int ledStatus = 0;
bool doorOpen = false;
bool doorActivated = false;
int minToKeepDoorOpen = 10;

SparkCorePolledTimer updateTimer(1000); //Create a timer object and set it's timeout in milliseconds
SparkCorePolledTimer actionTimer(minToKeepDoorOpen * 60000);


void setup() {
    Serial.begin(9600);
    Blynk.begin(auth);
    
    pinMode(reedSwitch, INPUT);    
    pinMode(relay, OUTPUT);
    updateTimer.SetCallback(garageDoorStatus);
    
    reedStatus = digitalRead(reedSwitch);
    
    Spark.function("activateDoor", activateDoor);
}

int activateDoor(String args){
  Spark.publish("door activated",args);
  digitalWrite(relay, HIGH);
  delay(2 * 1000); // delay for 2 sec
  digitalWrite(relay, LOW);
  actionTimer.Reset();
  actionTimer.SetCallback(closeDoorIfOpen);
}

BLYNK_WRITE(V1) //Button Widget is writing to pin V1
{
  activateDoor("blynk open");
}

void garageDoorStatus(void)
{
    //add LED in Blynk app corresponding to V3
    reedStatus = digitalRead(reedSwitch);
    if (reedStatus == HIGH) {
        ledStatus = 1023;
        doorOpen = true;
        Blynk.virtualWrite(3, ledStatus);
    }
    else {
        ledStatus = 0;
        doorOpen = false;
        Blynk.virtualWrite(3, ledStatus);
    }
}

void closeDoorIfOpen(){
    actionTimer.SetCallback(NULL);
    if(doorOpen){
        activateDoor("timer close");
    }
    else{
        Spark.publish("door status", "closed");
    }
}

void loop() {
    Blynk.run();
    updateTimer.Update();
    actionTimer.Update();
}