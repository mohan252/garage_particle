#include "blynk/blynk.h"
#include "SparkCorePolledTimer/SparkCorePolledTimer.h"

char auth[] = "e3ac4c915633434dbcb322fd5587f501";
const int relay = D3;
const int reedSwitch = D0;
SparkCorePolledTimer updateTimer(20000); //Create a timer object and set it's timeout in milliseconds
int timerCount = 0;
int highCount = 0;
int howLongCheckToGetStableReadingInMin = 5 * 3; // 15 20ms interval, // this will make door to open for 15 * 20 = 300 secs = 5 min
void (*action)(String) = NULL;
void (*nextAction)(String) = NULL;

void setup() {
    Serial.begin(9600);
    Blynk.begin(auth);
    pinMode(relay, OUTPUT);
    pinMode(reedSwitch, INPUT); 
    
    Particle.function("newaction", activateDoor);
    Particle.function("doorStatus", doorStatus);
    Particle.function("openClose", openClose);
}

void triggerActionAfterIntervalElapsed(int noOfIntervalsToWait, void (*actionToExecute)(String)){
    action = actionToExecute;
    howLongCheckToGetStableReadingInMin = noOfIntervalsToWait; // each interval is 20 ms
    triggerTimerToTakeAction(); 
}

int doorStatus(String args){
    int minToWait = args.toInt();
    triggerActionAfterIntervalElapsed(minToWait * 3,&publishStatus);
}

int activateDoor(String args){
  Particle.publish("door activated",args);
  digitalWrite(relay, HIGH);
  delay(2000); // delay for 2 sec
  digitalWrite(relay, LOW);  
}

int openClose(String args){
    int minToWait = args.toInt();
    activateDoor(args);
    triggerActionAfterIntervalElapsed(minToWait * 3,&closeDoorIfOpen);
}

void closeDoorIfOpen(String args){
    if(highCount > 0){
        activateDoor(args);
        delay(60000);
    }
    nextAction = &publishStatus;
    // triggerActionAfterIntervalElapsed(6,&publishStatus);
}

void publishStatus(String args){
    action = NULL;
    if(highCount > 0){
        Particle.publish("ifttt", "open");
    }
    else{
        Particle.publish("ifttt", "close");
    }
    nextAction = NULL;
}

void triggerTimerToTakeAction(){
    updateTimer.SetCallback(timerCheck);
}

void timerCheck(){
    timerCount = timerCount + 1;
    if(digitalRead(reedSwitch) == HIGH)
    {
        highCount = highCount + 1;    
    }
    if(timerCount == howLongCheckToGetStableReadingInMin){ 
        takeActionAndResetTimer("timer");
    }
}

void takeActionAndResetTimer(String args){
    updateTimer.SetCallback(NULL); // reset timer asap once the timer expired
    action(args);
    action = NULL;
    timerCount = 0;
    highCount = 0;
    if(nextAction != NULL){
        triggerActionAfterIntervalElapsed(6,nextAction);
    }
}


BLYNK_WRITE(V1) //Button Widget is writing to pin V1
{
  activateDoor("blynk");
}

BLYNK_WRITE(V2) //Button Widget is writing to pin V2
{
  openClose("2");
}

BLYNK_WRITE(V3) //Button Widget is writing to pin V2
{
  openClose("5");
}


void loop() {
    Blynk.run();
    updateTimer.Update();
}