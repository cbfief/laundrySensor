#include <Homie.h>


const int sensorPin = 14;
const int washerDonePin = 12;
const int washerStartPin = 13;
const int threshold = 100000;
const int dryerStartThreshold = 30000;
const int dryerStopThreshold = 120000;
bool vibDetected = 0;
int count = 0;
int dryerOn = 0;
int washerCount = 0;
bool washerOn = 0;
bool washerOff = 0;
long startTime = 0;
long stopTime = 0;

HomieNode dryerNode("Dryer", "sensor");
HomieNode washerNode("Washer", "sensor");

void setup() {
  // put your setup code here, to run once:
  Homie.setFirmware("washer-dryer", "1.0.0");
  Homie.setSetupFunction(setupHandler);
  Homie.setLoopFunction(loopHandler);
  Homie.setup();
  //Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  Homie.loop();
}

void setupHandler() {
  // Do what you want to prepare your sensor
  //pinMode(ledPin, OUTPUT);

}

void loopHandler() {
  vibDetected = checkVibration();
  //digitalWrite(ledPin,vibDetected);
  checkDryer();
  checkWasher();  
}


void checkDryer() {
  long currentTime = millis();

  if (vibDetected == 1 && startTime == 0) startTime = currentTime;

  if (vibDetected == 1 && dryerOn == 0 && (currentTime - startTime) > dryerStartThreshold){
      dryerOn = 1;
      stopTime = 0;
      //Serial.println("Cycle Started");
      Homie.setNodeProperty(dryerNode, "status", String("ON"));
  }

  if (vibDetected == 0 && dryerOn ==0) startTime = 0;

  if (vibDetected == 0 && dryerOn == 1 && stopTime == 0) stopTime = currentTime;

  if (vibDetected == 0 && dryerOn == 1 && (currentTime - stopTime) > dryerStopThreshold){
    dryerOn = 0;
    startTime = 0;
    //Serial.println("End of Cycle");
    Homie.setNodeProperty(dryerNode, "status", String("OFF"));
  }
  
}

bool checkVibration() {
    int reading = digitalRead(sensorPin);
    int state;

  if (reading == HIGH) {
    state = 1;
    count = 0;
  }
  else if (reading == LOW){
    if (count > threshold){
      state = 0;
      count = 0;
    }
    else {
      count = (count + 1);
      state = vibDetected;
    }
  }

  return state;
}

void checkWasher() {
  int washerStart = digitalRead(washerStartPin);
  int washerDone = digitalRead(washerDonePin);
  
  if (washerOn == 0 && washerStart == 1){
    washerCount = (washerCount + 1);
    if (washerCount == 100){
      washerOn = 1;
      Homie.setNodeProperty(washerNode, "status", String("ON"));
    }
  }

  if (washerOn == 1 && washerDone == 1){
    washerOn = 0;
    Homie.setNodeProperty(washerNode, "status", String("OFF"));
  }

  //if (washerStart == 0 && washerDone == 0 && washerOn ==1) washerOn = 0;
}
