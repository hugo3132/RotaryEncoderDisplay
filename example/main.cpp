/**
 * @author    Hugo3132
 * @copyright 2-clause BSD license
 */
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <LiquidCrystal_PCF8574.h>
#include <MenuView.h>
#include <RotaryEncoder.h>
#include <Wire.h>
#include <string>

#define ENCODER_PIN_A  D5 // ky-040 clk pin, interrupt & add 100nF/0.1uF capacitors between pin & ground!!!
#define ENCODER_PIN_B  D6 // ky-040 dt  pin,             add 100nF/0.1uF capacitors between pin & ground!!!
#define ENCODER_SWITCH D7 // ky-040 sw  pin, interrupt & add 100nF/0.1uF capacitors between pin & ground!!!
RotaryEncoder encoder(ENCODER_PIN_A, ENCODER_PIN_B, ENCODER_SWITCH);

#define LCD_ADDR           0x27
#define LCD_NUMBER_OF_COLS 20
#define LCD_NUMBER_OF_ROWS 4
LiquidCrystal_PCF8574 display(LCD_ADDR); // set the LCD address to 0x27 for a 16 chars and 2 line display

//#include <MenuView.h>
lcd::MenuView testMenu(&display, &encoder, "Example Test Menu", LCD_NUMBER_OF_COLS, LCD_NUMBER_OF_ROWS);

/**
 * @brief Callback if one of the encoder's pin are changed
 */
void ICACHE_RAM_ATTR encoderInterrupt(void) {
  encoder.tick();
}

/**
 * @brief Setup of Sketch
 */
void setup() {
  // Initialize Serial
  Serial.begin(115200);
  Serial.println();
  Serial.println("Here we go!");

  // Check if LCD works
  Wire.begin();
  Wire.beginTransmission(LCD_ADDR);

  if (Wire.endTransmission() == 0) {
    Serial.println("LCD found.");
    display.begin(LCD_NUMBER_OF_COLS, LCD_NUMBER_OF_ROWS);
    display.home();
    display.clear();
    display.print("Booting...");
    display.setBacklight(1);

    // attach interrupts of rotary encoder
    attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_A), encoderInterrupt, CHANGE);
    attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_B), encoderInterrupt, CHANGE);
    attachInterrupt(digitalPinToInterrupt(ENCODER_SWITCH), encoderInterrupt, CHANGE);
  }
  else {
    Serial.println("LCD not found.");
  }

  testMenu.createMenuItem("A very long Entry which shouldn't fit");
  for (int i = 0; i < 5; i++) {
    testMenu.createMenuItem("Entry " + String(i), [&i](lcd::MenuView::MenuItem* item) {
      Serial.println(item->getText() + " clicked.");
    });
  }

  testMenu.activate();
}

/**
 * @brief Loop function
 */
void loop() {
  testMenu.tick();
}
