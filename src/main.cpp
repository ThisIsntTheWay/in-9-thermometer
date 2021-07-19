#include <Arduino.h>

#define PWM_PIN     1
#define LED_PIN     3
#define SENSOR_PIN  2
#define MVOFFSET    0.5

#define DEBUG

// Uncomment to use external VREF
//#define VREF_EXT
#ifdef VREF_EXT
  #define VREF 3.3
#else
  #define VREF 5.0
#endif

/* ----------------------
    VARIABLES
  ----------------------- */
int sensVal;
int currMillis;
int startMillis;

float TMP36volt;
float temperature;

/* ----------------------
    MAIN
  ----------------------- */
void setup() {
  pinMode(PWM_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(SENSOR_PIN, INPUT);

  #ifdef DEBUG
    Serial.begin(9600);
  #endif

  startMillis = millis();

  // Light up status LED
  digitalWrite(LED_PIN, 1);

  #ifdef VREF_EXT
    // AREF is on pin 5
    analogReference(EXTERNAL);

    #ifdef DEBUG
      Serial.println(F("[i] Using AREF!");
    #endif
  #endif

  #ifdef DEBUG
    Serial.printf("VREF set to: %d", VREF);
  #endif
}

void loop() {
  #ifdef DEBUG
    Serial.println(F("--------------"));
  #endif

  // Check if status LED should still light up
  currMillis = millis();
  if ((currMillis - startMillis) > 5000)
    digitalWrite(LED_PIN, 0);

  // Parse TMP36 data
  sensVal = analogRead(SENSOR_PIN);
  TMP36volt = sensVal * VREF;

  temperature = (TMP36volt - MVOFFSET) * 100;

  #ifdef DEBUG
    Serial.printf("ADC (Pin: %d) readings:\n", SENSOR_PIN);
    Serial.printf(" > Raw: %d\n", sensVal);
    Serial.printf(" > Raw with VREF %f\n", TMP36volt);
    Serial.printf(" > Temperatuer (C): %d\n", temperature);
  #endif

  delay(500);
}