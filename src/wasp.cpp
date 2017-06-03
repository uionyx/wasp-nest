#include <Arduino.h>
#include <Wire.h>

#define SOLENOID_PIN 4
#define SOLENOD_INTERVAL 90

unsigned long solenoid_timer = millis();
unsigned long solenoid_timer_interval = 4000;

unsigned long previous_solenoid = 0;
unsigned int solenoid_active = 0;

int current_state = 1;

void solenoid_control(unsigned long interval){

  unsigned long current_solenoid = millis();

  if(current_solenoid - previous_solenoid > interval) {
    previous_solenoid = current_solenoid;

    if (solenoid_active == 0){
      solenoid_active = 1;
      digitalWrite(SOLENOID_PIN, HIGH);
    } else {
      solenoid_active = 0;
      digitalWrite(SOLENOID_PIN, LOW);
    }

  }

}
void transition_state(int state) {

  current_state = state;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(SOLENOID_PIN, OUTPUT);           //Sets the pin as an output

}

void loop() {
  // put your main code here, to run repeatedly:
  switch(current_state){

  case 1:

  solenoid_control(SOLENOD_INTERVAL);

  if (((millis() - solenoid_timer) > solenoid_timer_interval)) {
      digitalWrite(SOLENOID_PIN, LOW);
      transition_state(2);
    }


  break;

  case 2:

  Serial.print("case2");
  break;

  }



            //Wait 1 Second
}
