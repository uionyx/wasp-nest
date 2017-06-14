#include <Arduino.h>
#include <Wire.h>

#define SOLENOID_PIN 4
#define PIR_PIN 13
#define BUZZER_PIN 2


#define BOOT_SEQUENCE 1
#define CALIBRATE_PIR 2
#define SEEKING_STATE 3
#define INIT_CO2 4
#define RESET_UNIT 5

#define SOLENOD_INTERVAL 100

unsigned long previous_time = millis();
unsigned long current_time = millis();

unsigned long calibration_timer = 3000; // duration of calibration
int sensor_reading = 0;

unsigned long solenoid_timer = 9000; // duration of solenoid operation
unsigned long previous_solenoid = 0;
unsigned int solenoid_active = 0;

unsigned int buzzer_active = 0;
unsigned long previous_tone_timer = 0;

int current_state = 1;
unsigned long boot_timer = 1000;

void buzzer_play_tone(unsigned long duration, long frequency){

  unsigned long current_tone_timer = millis();

  if(current_tone_timer - previous_tone_timer > duration) {
    previous_tone_timer = current_tone_timer;

    if (buzzer_active == 0){
      buzzer_active = 1;
      tone(BUZZER_PIN, frequency);
    } else {
      buzzer_active = 0;
      noTone(BUZZER_PIN);
    }

  }

}

void solenoid_control(unsigned long interval){

  unsigned long current_solenoid = millis();

  if(current_solenoid - previous_solenoid > interval) {
    previous_solenoid = current_solenoid;

    if (solenoid_active == 0){
      solenoid_active = 1;
      analogWrite(SOLENOID_PIN, 250);
    } else {
      solenoid_active = 0;
      analogWrite(SOLENOID_PIN, 0);
    }

  }

}

bool sense_motion(){

  sensor_reading = digitalRead(PIR_PIN);
  bool motion_detected;

  if(sensor_reading == HIGH){
    motion_detected = true;

  } else {
    motion_detected = false;
  }

  return motion_detected;
}

void transition_state(int state) {

  current_state = state;
  Serial.print(current_state);
}

void setup() {

  Serial.begin(9600);
  pinMode(PIR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(SOLENOID_PIN, OUTPUT);

}

void loop() {

  switch(current_state){

  case BOOT_SEQUENCE:

    buzzer_play_tone(250, 5000);
    current_time = millis();

    if (((current_time - previous_time) > boot_timer)) {
        previous_time = current_time;
        noTone(BUZZER_PIN);
        transition_state(CALIBRATE_PIR);
      }

  break;

  case CALIBRATE_PIR:

    current_time = millis();

    if (((current_time - previous_time) > calibration_timer)) {
        previous_time = current_time;
        transition_state(SEEKING_STATE);
      }

  break;

  case SEEKING_STATE:

    if(sense_motion()){
      transition_state(INIT_CO2);
      Serial.print("Motion detected - transitioning to INIT_CO2");
    }

  break;

  case INIT_CO2:

    solenoid_control(SOLENOD_INTERVAL);

    current_time = millis();

    if (((current_time - previous_time) > solenoid_timer)) {
        analogWrite(SOLENOID_PIN, 0);
        noTone(BUZZER_PIN);
        previous_time = current_time;
        transition_state(RESET_UNIT);
      }

  break;

  case RESET_UNIT:

  break;


  }




}
