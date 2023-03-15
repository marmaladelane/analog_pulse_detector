#include "limits.h"

// Configuration:
const bool DEBUG = true;                                          // enable debug logging of current values and messages
const int  NUM_CHANNELS = 2;                                      // number of ADC pins to sample
const int  CHANNEL_PINS[NUM_CHANNELS] = {A0, A1};                 // which ADC pin to sample for each channel
const bool ENABLE_INTERNAL_PULLUP[NUM_CHANNELS] = {false, true};  // true to enable the internal pull-up to 5V on each channel
const int  LOG_CURRENT_VALUES_INTERVAL = 5000;                    // how often to debug log the current values, in milliseconds
const int  MIN_PULSE_LENGTH = 10;                                 // number of milliseconds that new state must be held to record it
const int  PULSE_VALUE_THRESHOLD = 100;
const bool COUNT_FALLING_EDGE = true;

// Internal state:
int LAST_VALUE[NUM_CHANNELS] = {}; // Last value read from ADC
bool LAST_STATE[NUM_CHANNELS] = {}; // Last state (high or low), determined from value and PULSE_VALUE_THRESHOLD
unsigned long LAST_PULSE_RECORDED_TIME[NUM_CHANNELS] = {}; // time when the last pulse was counted, for pulse interval output
unsigned long LAST_STATE_RECORDED_TIME[NUM_CHANNELS] = {}; // time when we were last in LAST_STATE, for debouncing
unsigned long PULSE_COUNT[NUM_CHANNELS] = {}; // number of pulses counted in total
unsigned long LOG_CURRENT_VALUES_LAST = 0; // time of last debug log of current values

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  for (int i = 0; i < NUM_CHANNELS; i++)
  {
    digitalWrite(CHANNEL_PINS[i], ENABLE_INTERNAL_PULLUP[i] ? HIGH : LOW); // maybe disable internal pullup
    pinMode(CHANNEL_PINS[i], INPUT);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  
  bool log_current_values = false;
  unsigned long current_time = millis();

  if (current_time < LOG_CURRENT_VALUES_LAST || // clock wrap
      current_time > LOG_CURRENT_VALUES_LAST + LOG_CURRENT_VALUES_INTERVAL)
  {
    log_current_values = true;
  }
  
  for (int i = 0; i < NUM_CHANNELS; i++)
  {
    int current_value = analogRead(CHANNEL_PINS[i]);
    LAST_VALUE[i] = current_value;
    
    bool current_state = (current_value > PULSE_VALUE_THRESHOLD) ? true : false;
    bool previous_state = LAST_STATE[i];
    
    if (current_state != previous_state)
    {
      log_current_values = true;
      
      unsigned long previous_time = LAST_STATE_RECORDED_TIME[i];
      unsigned long time_since_state_change;

      if (current_time < previous_time)
      {
        time_since_state_change = (ULONG_MAX - previous_time) + current_time;
      }
      else
      {
        time_since_state_change = current_time - previous_time;
      }
      
      if (time_since_state_change >= MIN_PULSE_LENGTH)
      {
        Serial.print("pulse.");
        Serial.print(i);
        Serial.print(" N:");
        Serial.print(current_state ? "1" : "0");
        Serial.println("");
        
        LAST_STATE[i] = current_state;
        LAST_STATE_RECORDED_TIME[i] = current_time;

        if ((COUNT_FALLING_EDGE && current_state == false) ||
            (!COUNT_FALLING_EDGE && current_state == true))
        {
          PULSE_COUNT[i]++;

          Serial.print("count.");
          Serial.print(i);
          Serial.print(" N:");
          Serial.print(PULSE_COUNT[i]);
          Serial.println("");

          unsigned long last_pulse_recorded_time = LAST_PULSE_RECORDED_TIME[i];
          unsigned long time_between_pulses;

          if (current_time < last_pulse_recorded_time)
          {
            time_between_pulses = (ULONG_MAX - last_pulse_recorded_time) + current_time;
          }
          else
          {
            time_between_pulses = current_time - last_pulse_recorded_time;
          }

          Serial.print("pulse_interval.");
          Serial.print(i);
          Serial.print(" N:");
          Serial.print(time_between_pulses);
          Serial.println("");

          LAST_PULSE_RECORDED_TIME[i] = current_time;
        }
      }
      else
      {
        Serial.print("# Ignoring transition of ");
        Serial.print(i);
        Serial.print(" from ");
        Serial.print(previous_state ? "1" : "0");
        Serial.print(" to ");      
        Serial.print(current_state ? "1" : "0");
        Serial.print(" as elapsed time ");
        Serial.print(time_since_state_change);
        Serial.print(" ms is less than the minimum");
        Serial.println("");
      }
    }
    else // current_state == previous_state
    {
      LAST_STATE_RECORDED_TIME[i] = current_time;
    }
  }

  if (DEBUG && log_current_values)
  {
    Serial.print("# ");
    for (int i = 0; i < NUM_CHANNELS; i++)
    {
      Serial.print(i);
      Serial.print("=");
      Serial.print(LAST_VALUE[i]);
      Serial.print(" ");
    }
    Serial.println("");
    LOG_CURRENT_VALUES_LAST = current_time;
  }
  
  delay(MIN_PULSE_LENGTH / 2);
}
