#include <FastLED.h>

#define PIN 6     // пин ленты
#define NUM_LEDS 144     // кол-во светодиодов

CRGB leds[NUM_LEDS];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(3, OUTPUT);
  pinMode(2, INPUT);
  pinMode(4, INPUT);
  digitalWrite(3, LOW);

  FastLED.addLeds<WS2811, PIN, GRB>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(80);
  for (int i = 0; i < NUM_LEDS; i++ ) {
    leds[i] = CRGB::White;
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  int num = analogRead(A0);
  Serial.write(num>>2);
  if(digitalRead(2) == HIGH)
    digitalWrite(3, HIGH);
  else
    digitalWrite(3, LOW);
  byte bright = 0;
  if(digitalRead(4) == HIGH) bright = 80;
  FastLED.setBrightness(bright);
  FastLED.show();
}
