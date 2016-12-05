/**
 * Shy Bamboo
 * 
 * Josh Lind, 11/2016
 * 
 * RGB LED Pixel stands and echo distance sensors.
 * 
 * 
 * Reduce NeoPixel burnout risk by adding a 1000 uF capacitor across pixel
 * power leads, add 300 - 500 Ohm resistor on first pixel's data input.
 * Avoid connecting on a live circuit... if you must, connect GND first.
 * 
 * @todo Random bamboo height assignment.
 */

#include <Adafruit_NeoPixel.h>

#define PIN1 2 // LED Pixel strand 1
#define PIN2 3 // LED Pixel strand 2
#define PIN3 4 // LED Pixel strand 3
#define PIN4 5 // LED Pixel strand 4
#define PIN5 6 // LED Pixel strand 5
#define trigPin1 10  // Sensor: Trigger Pin 1
#define echoPin1 11  // Sensor: Echo Pin 1
//#define trigPin2 12  // Sensor: Trigger Pin 2
//#define echoPin2 13  // Sensor: Echo Pin 3
//#define PIN_LED 13 // Debug LED

const uint8_t moveThreshhold = 10, // Debounce the range fluxtuation
  maximumRange = 120,              // Maximum range needed
  minimumRange = 10,               // Minimum range needed
  shyCycles = 180,
  delayed = 10;

const int8_t bambooSizeSegmentDiff = -2; // Decrease in bamboo segment length per piece

const uint16_t NUM_LEDS = 83, // Strand length
  bambooSizeInit = 20,         // Length of first bamboo segment
  OFFSET_1 = 16,               // Strand E
  OFFSET_2 = 20, //216,         // Strand B
  OFFSET_3 = 0, //34,         // Strand A
  OFFSET_4 = 0,                // Strand C
  OFFSET_5 = 15;               // Strand D

uint32_t bambooColor, bambooDark;
uint8_t shyCounter;
uint8_t prevDistance = 0;

boolean debug = true;

Adafruit_NeoPixel strip1 = Adafruit_NeoPixel(100, PIN1, NEO_GRB + NEO_KHZ800),
  strip2 = Adafruit_NeoPixel(130, PIN2, NEO_GRB + NEO_KHZ800),
  strip3 = Adafruit_NeoPixel(100, PIN3, NEO_GRB + NEO_KHZ800),
  strip4 = Adafruit_NeoPixel(100, PIN4, NEO_GRB + NEO_KHZ800),
  strip5 = Adafruit_NeoPixel(100, PIN5, NEO_GRB + NEO_KHZ800);


void setup() {
  uint8_t i;

  randomSeed(analogRead(0));

  // Used for plants and off.
  bambooColor = strip1.Color(0, 80, 0);
  bambooDark = strip1.Color(0, 0, 0);

  strip1.begin();
  strip2.begin();
  strip3.begin();
  strip4.begin();
  strip5.begin();

  // Reset everything to off.
//  for (i = 0; i < OFFSET_1; i++) {
//    strip1.setPixelColor(i, bambooDark);
//  }
//  for (i = 0; i < OFFSET_2; i++) {
//    strip2.setPixelColor(i, bambooDark);
//  }
//  for (i = 0; i < OFFSET_3; i++) {
//    strip3.setPixelColor(i, bambooDark);
//  }
//  for (i = 0; i < OFFSET_4; i++) {
//    strip4.setPixelColor(i, bambooDark);
//  }
//  for (i = 0; i < OFFSET_5; i++) {
//    strip5.setPixelColor(i, bambooDark);
//  }

  strip1.show();
  strip2.show();
  strip3.show();
  strip4.show();
  strip5.show();

  // Echo sensor.
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);

  if (debug) {
    Serial.begin (9600);
    //pinMode(PIN_LED, OUTPUT);
    //digitalWrite(PIN_LED, LOW);
  }

  // Initial plant look.
  resetBamboo();
}


void loop() {
  uint32_t randomColor = Wheel(random(256));
  uint8_t randomPixel1 = random(NUM_LEDS),
    randomPixel2 = random(NUM_LEDS),
    randomPixel3 = random(NUM_LEDS),
    randomPixel4 = random(NUM_LEDS),
    randomPixel5 = random(NUM_LEDS);

  // @todo Avoid same random pixel on all stocks.

  // Carry on sparkling.
  if (!checkMovement()) {
    // Shy period.
    if (shyCounter < shyCycles) {
      shyCounter++;
    }
    else {
      // Set random pixel to random color.
      if (strip1.getPixelColor(randomPixel1 + OFFSET_1) != bambooDark) {
        strip1.setPixelColor(randomPixel1 + OFFSET_1, randomColor);
        strip1.show();
      }
      if (strip2.getPixelColor(randomPixel2 + OFFSET_2) != bambooDark) {
        strip2.setPixelColor(randomPixel2 + OFFSET_2, randomColor);
        strip2.show();
      }
      if (strip3.getPixelColor(randomPixel3 + OFFSET_3) != bambooDark) {
        strip3.setPixelColor(randomPixel3 + OFFSET_3, randomColor);
        strip3.show();
      }
      if (strip4.getPixelColor(randomPixel4 + OFFSET_4) != bambooDark) {
        strip4.setPixelColor(randomPixel4 + OFFSET_4, randomColor);
        strip4.show();
      }
      if (strip5.getPixelColor(randomPixel5 + OFFSET_5) != bambooDark) {
        strip5.setPixelColor(randomPixel5 + OFFSET_5, randomColor);
        strip5.show();
      }
    }
  }
  else {
    resetBamboo();
    shyCounter = 0;
  }
  delay(delayed);
}


/**
 * Reset LEDs to look like bamboo.
 */
void resetBamboo() {
  uint8_t i,
    bambooSize = bambooSizeInit;

  for (i = 0; i < NUM_LEDS; i++) {
    setAllStrands(i, bambooColor); // Bamboo color to strand 2
  }

  // Bamboo segments.
//  for (i = 0; i < NUM_LEDS; i += bambooSize) {
//    setAllStrands(i, bambooDark); // Gap in bamboo color
//    bambooSize += bambooSizeSegmentDiff;
//  }

  strip1.show();
  strip2.show();
  strip3.show();
  strip4.show();
  strip5.show();

//  if (debug) {
//    digitalWrite(PIN_LED, LOW);
//  }
}


/**
 * DRY, multiple bamboo shoots.
 */
void setAllStrands(uint16_t pixel, uint32_t color) {
  strip1.setPixelColor(pixel + OFFSET_1, color);
  strip2.setPixelColor(pixel + OFFSET_2, color);
  strip3.setPixelColor(pixel + OFFSET_3, color);
  strip4.setPixelColor(pixel + OFFSET_4, color);
  strip5.setPixelColor(pixel + OFFSET_5, color);
}


/**
 * Detect movement with distance sensor.
 * 
 * Accept a range of valid distance to trigger reaction.
 * 
 * Out of range is considered movement (the desired effect is only display with an audience).
 * 
 * @todo Rolling average rather than prevDistance.
 */
boolean checkMovement() {
  long duration;
  uint16_t distance;
  
  // Trigger-echo cycle to find distance of nearest object.
  digitalWrite(trigPin1, LOW); 
  delayMicroseconds(2); 
  digitalWrite(trigPin1, HIGH);
  delayMicroseconds(10); 
  digitalWrite(trigPin1, LOW);
  duration = pulseIn(echoPin1, HIGH);

  // Calculate the distance (in cm), based on speed of sound.
  distance = (duration/2) / 29.1;

  // Outside of boundaries.
  if (distance >= maximumRange || distance <= minimumRange) {
    // Send a negative number to indicate out of range.
    if (debug) {
      Serial.print("OUT OF RANGE  ");
      Serial.println(distance);
    }
    // Save to ignore subsequent bad sensor readings.
    prevDistance = 0;
    return true;
  }
  else {
    if (debug) {
      Serial.println(distance);
    }
    // Check for movement.
    if (distance > 0 && prevDistance > 0 && (abs(prevDistance - distance) > moveThreshhold)) {
      // Considered movement.
      Serial.print("STARTLED!  D:");
      Serial.print(distance);
      Serial.print(" - P:");
      Serial.println(prevDistance);
      // Track past.
      prevDistance = distance;
      return true;
    }
    prevDistance = distance;

    // Considered noise by threshhold.
    return false;
  }
}


/**
 * Input a value 0 to 255 to get a color value.
 * The colours are a transition r - g - b - back to r.
 */
uint32_t Wheel(byte WheelPos) {
  if (WheelPos < 85) {
    return strip1.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
  else if (WheelPos < 170) {
    WheelPos -= 85;
    return strip1.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  else {
    WheelPos -= 170;
    return strip1.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

