#include <Arduino.h>
#include <Wire.h>

#define SOLENOID_PIN 4
#define PIR_PIN 13
#define BUZZER_PIN 12


#define BOOT_SEQUENCE 1
#define CALIBRATE_PIR 2
#define SEEKING_STATE 3
#define INIT_CO2 4
#define RESET_UNIT 5

#define SOLENOD_INTERVAL 100
#define SEEK_INTERVAL 200
#define STRIKE_MAX 8

unsigned long previous_time = millis();
unsigned long current_time = millis();

unsigned long calibration_timer = 3000; // duration of calibration
unsigned long currentSeekTime = 0;
unsigned long previousSeekTime = 0;
int sensorState = 0;
int lastSensorState = 1;
bool motionActive = false;
int pirVal = 0;



unsigned long strikeInterval = 200; // duration of solenoid operation
unsigned long previous_solenoid = 0;
unsigned int solenoid_active = 0;
unsigned long current_solenoid_time = 0;
int strikeCount = 0;

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

  pirVal = digitalRead(PIR_PIN);

  if(pirVal == HIGH){
    transition_state(INIT_CO2);
    digitalWrite(PIR_PIN,LOW);
    Serial.print("Motion detected - transitioning to INIT_CO2");
  }

  break;

  case INIT_CO2:

  current_time = millis();

  if(current_time - previous_time > strikeInterval) {
    previous_time = current_time;
    if (solenoid_active == 0){
      solenoid_active = 1;
      strikeCount++;
      digitalWrite(SOLENOID_PIN, HIGH);

    } else {
      solenoid_active = 0;
      digitalWrite(SOLENOID_PIN, LOW);
      if(strikeCount >= STRIKE_MAX){
        strikeCount = 0;
        transition_state(RESET_UNIT);
      }
    }
  }

  break;

  case RESET_UNIT:

  transition_state(BOOT_SEQUENCE);

  break;


  }




}
