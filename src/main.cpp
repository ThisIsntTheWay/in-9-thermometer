#include <Arduino.h>
#include <Bounce2.h>

// Pins
#define PWM_PIN     1
#define POT_PIN     4
#define LED_PIN     3
#define BUT_PIN     0
#define SENSOR_PIN  2

// Base values
#define MVOFFSET    0.5     // mV offset for ADC 0 readings
#define PRESS_INT   5000    // Time to hold button to enter calibration mode (ms)
#define BOOTLED_T   5000    // Boot LED timeout (ms)

// Ranges
#define TEMP_MIN    0       // Min temperature range value
#define TEMP_MAX    40      // Max temperature range value
#define CALIB_MIN   -5      // Min calibration range value
#define CALIB_MAX   5       // Max calibration range value

// Various
#define DEBUG               // if defined, the MCU will produce serial output
//#define VREF_EXT          // if defined, AREF will be used

#ifdef VREF_EXT
  #define VREF 3.3          // if using AREF
#else
  #define VREF 5.0          // if using VREF
#endif

/* ----------------------
    VARIABLES
  ----------------------- */
int pwm;
int sensVal;
int potVal;
int calib;

int currMillis;
int startMillis;
int heldMillis;

float TMP36volt;
float temperature;

bool isCalibrating = false;
bool isHeld = false;

/* ----------------------
    INSTANCES
  ----------------------- */
Bounce bounce = Bounce();

/* ----------------------
    MAIN
  ----------------------- */
void setup() {
  pinMode(PWM_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(POT_PIN, INPUT);
  pinMode(SENSOR_PIN, INPUT);

  bounce.attach(BUT_PIN, INPUT);
  bounce.interval(5);

  startMillis = millis();

  #ifdef DEBUG
    Serial.begin(9600);
  #endif

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
  bounce.update();
  currMillis = millis();

  // Check if status LED should still light up
  if ((currMillis - startMillis) > BOOTLED_T)
    digitalWrite(LED_PIN, 0);

  // ADC readings
  sensVal = analogRead(SENSOR_PIN);
  potVal = analogRead(POT_PIN);

  // Parse TMP36 data
  TMP36volt = sensVal * VREF;
  temperature = (TMP36volt - MVOFFSET) * 100;

  // Map temperature to PWM signal and push said signal
  pwm = map(temperature, TEMP_MIN, TEMP_MAX, 0, 255);
  analogWrite(PWM_PIN, pwm);

  #ifdef DEBUG
    Serial.println(F("--------------"));
    Serial.printf("PWM value: %d\n", temperature);
    Serial.printf("ADC (Pin: %d) readings:\n", SENSOR_PIN);
    Serial.printf(" > Raw: %d\n", sensVal);
    Serial.printf(" > Raw with VREF %f\n", TMP36volt);
    Serial.printf(" > Temperature (C): %d\n", temperature);
    Serial.printf("ADC (Pin: %d) readings:\n", POT_PIN);
    Serial.printf(" > Raw: %d\n", potVal);
  #endif

  // Check if calibration mode should be entered
  if (bounce.changed()) {
    if (bounce.read() == 0) {
      if (!isHeld) {              // Init timer
        heldMillis = millis();
        isHeld = true;
      }

      if ((currMillis - heldMillis) > PRESS_INT) {
        isCalibrating = true;
      } else {
        isCalibrating = false;
      }
    } else {
      isHeld = false;             // Nullify timer
    }
  }

  // Check for calibration mode
  if (isCalibrating) {
    digitalWrite(LED_PIN, 1);
    delay (1000);
    digitalWrite(LED_PIN, 0);
    
    calib = map(potVal, 0, 1024, CALIB_MIN, CALIB_MAX);
  }

  delay(1000);
}