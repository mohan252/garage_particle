#include "blynk/blynk.h"
#include "SparkCorePolledTimer/SparkCorePolledTimer.h"

char auth[] = "e3ac4c915633434dbcb322fd5587f501";
const int relay = D3;
SparkCorePolledTimer updateTimer(20000); //Create a timer object and set it's timeout in milliseconds
int timerCount = 0;
int howLongCheckToGetStableReadingInMin = 5 * 3; // 15 20ms interval, // this will make door to open for 15 * 20 = 300 secs = 5 min
int (*action)(String) = NULL;

void setup() {
    Serial.begin(9600);
    Blynk.begin(auth);
    pinMode(relay, OUTPUT);
    
    Particle.function("activateDoor", activateDoor);
    Particle.function("openClose", openClose);
}

void triggerActionAfterIntervalElapsed(int noOfIntervalsToWait, int (*actionToExecute)(String)){
    action = actionToExecute;
    howLongCheckToGetStableReadingInMin = noOfIntervalsToWait; // each interval is 20 ms
    triggerTimerToTakeAction(); 
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
    triggerActionAfterIntervalElapsed(minToWait * 3,&activateDoor);
}

void triggerTimerToTakeAction(){
    updateTimer.SetCallback(timerCheck);
}

void timerCheck(){
    timerCount = timerCount + 1;
    if(timerCount == howLongCheckToGetStableReadingInMin){ 
        takeActionAndResetTimer("timer");
    }
}

void takeActionAndResetTimer(String args){
    updateTimer.SetCallback(NULL); // reset timer asap once the timer expired
    action(args);
    action = NULL;
    timerCount = 0;
}


BLYNK_WRITE(V1) //Button Widget is writing to pin V1
{
  activateDoor("blynk");
}

BLYNK_WRITE(V2) //Button Widget is writing to pin V2
{
  openClose("2");
}

BLYNK_WRITE(V5) //Button Widget is writing to pin V2
{
  openClose("5");
}


void loop() {
    Blynk.run();
    updateTimer.Update();
}