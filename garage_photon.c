#include "SparkCorePolledTimer/SparkCorePolledTimer.h"

#include "blynk/blynk.h"

char auth[] = "e3ac4c915633434dbcb322fd5587f501";
SparkCorePolledTimer updateTimer(1000); //Create a timer object and set it's timeout in milliseconds

const int reedSwitch = D0;
const int relay = D3;
int reedStatus = 0;
int ledStatus = 0;

void setup() {
    Serial.begin(9600);
    Blynk.begin(auth);
    pinMode(reedSwitch, INPUT);    
    pinMode(relay, OUTPUT);
    
    Spark.function("activateDoor", activateDoor);
    
    updateTimer.SetCallback(garageDoorStatus);
    reedStatus = digitalRead(reedSwitch);
}

int activateDoor(String args){
  Spark.publish("door activated",args);
  digitalWrite(relay, HIGH);
  delay(2 * 1000); // delay for 2 sec
  digitalWrite(relay, LOW);  
}

BLYNK_WRITE(V1) //Button Widget is writing to pin V1
{
  activateDoor("blynk");
}

void garageDoorStatus(void)
{
    //add LED in Blynk app corresponding to V3
    reedStatus = digitalRead(reedSwitch);
    if (reedStatus == HIGH) {
        ledStatus = 1023;
        Spark.publish("reedStatus","high");
        Blynk.virtualWrite(3, ledStatus);
    }
    else {
        ledStatus = 0;
        Spark.publish("reedStatus","low");
        Blynk.virtualWrite(3, ledStatus);
    }
}

void loop() {
    Blynk.run();
    updateTimer.Update();
}