/**
 * Shy Bamboo
 * 
 * Josh Lind, 11/2016
 * 
 * RGB LED Pixel stands and echo distance sensor.
 * 
 * @todo Apply color dim in stalk gaps rather than single dark color.
 */

// @TODO
// Reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

#include <Adafruit_NeoPixel.h>

#define PIN1 2 // LED Pixel strand 1
#define PIN2 3 // LED Pixel strand 2
#define PIN3 4 // LED Pixel strand 3
#define PIN4 5 // LED Pixel strand 4
#define PIN5 6 // LED Pixel strand 5
#define trigPin 8 // Echo: Trigger Pin
#define echoPin 9 // Echo: Echo Pin
#define PIN_LED 13 // Debug LED

const uint8_t moveThreshhold = 15,  // Debounce the range fluxtuation
  maximumRange = 200, // Maximum range needed
  minimumRange = 0, // Minimum range needed
  NUM_LEDS = 60, // Strand length
  OFFSET_1 = 0, // Strand offset to first pixel 1
  OFFSET_2 = 0, // Strand offset to first pixel 2
  OFFSET_3 = 30, // Strand offset to first pixel 3
  OFFSET_4 = 60, // Strand offset to first pixel 4
  OFFSET_5 = 60, // Strand offset to first pixel 5
  bambooSize = 10, // Length of first bamboo segment
  delayed = 50;

uint32_t bambooColor, bambooDark;
long duration, distance, prev_distance;
boolean debug = true;

Adafruit_NeoPixel strip1 = Adafruit_NeoPixel(NUM_LEDS + OFFSET_1, PIN1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2 = Adafruit_NeoPixel(NUM_LEDS + OFFSET_2, PIN2, NEO_GRB + NEO_KHZ800),
Adafruit_NeoPixel strip3 = Adafruit_NeoPixel(NUM_LEDS + OFFSET_3, PIN3, NEO_GRB + NEO_KHZ800),
Adafruit_NeoPixel strip4 = Adafruit_NeoPixel(NUM_LEDS + OFFSET_4, PIN4, NEO_GRB + NEO_KHZ800),
Adafruit_NeoPixel strip5 = Adafruit_NeoPixel(NUM_LEDS + OFFSET_5, PIN5, NEO_GRB + NEO_KHZ800);


void setup() {
  strip1.begin();
  //strip2.begin();
  //strip3.begin();
  //strip4.begin();
  //strip5.begin();
  strip1.show();
  //strip2.show();
  //strip3.show();
  //strip4.show();
  //strip5.show();

  // Echo sensor.
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  if (debug) {
    Serial.begin (9600);
    pinMode(PIN_LED, OUTPUT);
    digitalWrite(PIN_LED, LOW);
  }

  // Initial plant look.
  bambooColor = Wheel(200);
  bambooDark = Wheel(0);
  resetBamboo();
}


void loop() {
  uint32_t randomColor, randomPixel;

  // @todo Avoid random pixels on bamboo stock gaps.

  if (!checkMovement()) {
    // Set random pixel to random color.
    strip1.setPixelColor(random(0, NUM_LEDS) + OFFSET_1, Wheel(random(0, 256)));
    strip2.setPixelColor(random(0, NUM_LEDS) + OFFSET_2, Wheel(random(0, 256)));
    strip3.setPixelColor(random(0, NUM_LEDS) + OFFSET_3, Wheel(random(0, 256)));
    strip4.setPixelColor(random(0, NUM_LEDS) + OFFSET_4, Wheel(random(0, 256)));
    strip5.setPixelColor(random(0, NUM_LEDS) + OFFSET_5, Wheel(random(0, 256)));

    if (debug) {
      digitalWrite(PIN_LED, HIGH);
    }
  }
  else {
    resetBamboo();
  }
  delay(delayed);
}


/**
 * Reset LEDs to look like bamboo.
 */
void resetBamboo() {
  uint8_t i;

  for (i=0; i>NUM_LEDS; i++) {
    if (i % bambooSize == 0) {
      setAllStrands(i, bambooDark); // Gap in bamboo color
    }
    else {
      setAllStrands(i, bambooColor); // Bamboo color to strand 2
    }
  }

  if (debug) {
    digitalWrite(PIN_LED, LOW);
  }
}


/**
 * DRY, multiple bamboo shoots.
 */
void setAllStrands(pixel, color) {
  strip1.setPixelColor(pixel + OFFSET_1, color);
  strip2.setPixelColor(pixel + OFFSET_2, color);
  strip3.setPixelColor(pixel + OFFSET_3, color);
  strip4.setPixelColor(pixel + OFFSET_4, color);
  strip5.setPixelColor(pixel + OFFSET_5, color);
}


/**
 * Detect movement with distance sensor.
 * 
 * @todo Maybe remove distance min-max boundaries.
 */
boolean checkMovement() {
  long test_distance;
  
  // Trigger-echo cycle to find distance of nearest object.
  digitalWrite(trigPin, LOW); 
  delayMicroseconds(2); 
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); 
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);

  // Calculate the distance (in cm), based on speed of sound.
  distance = duration / 58.2;

  // Save for checking threshold later.
  test_distance = prev_distance;
  prev_distance = distance;

  // Outside of boundaries.
  if (distance >= maximumRange || distance <= minimumRange) {
    // Send a negative number to indicate out of range.
    if (debug) {
      Serial.println("-1");
    }
    return false;
  }
  else {
    if (debug) {
      Serial.println(distance);
    }
    // Check for movement.
    if (abs(distance - test_distance) > moveThreshhold) {
      // Considered movement.
      Serial.print("STARTLED!  ");
      Serial.println(prev_distance);
      return true;
    }
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

