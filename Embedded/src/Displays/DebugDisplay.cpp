#include "DebugDisplay.h"

DebugDisplay::DebugDisplay() : Display() {}

boolean DebugDisplay::setup() {
  return true;
}

// notifies the display to do another frame if it wants to do that kind of thing
void DebugDisplay::frameLoop() {

}

// It's a clock of some sort... you have to implement this
// Time is passed by reference - the display should update on the next frame loop
void DebugDisplay::setTime(DateTime time) {
  Serial.println((String)"*** TIME: " + time.hour() + ":" + time.minute() + ":" + time.second());
}

// Implementation is optional
void DebugDisplay::setWeather(Weather weather) {
  Serial.println((String)"*** WEATHER: " + weather.synopsis);
}

// Show a message - but what kind of message?
void DebugDisplay::displayMessage(const char *stringy, MessageType messageType) {
  Serial.println((String)"*** MESSAGE: " + stringy);
}

// Brightness is a float from 0 (barely visible) to 1 (really bright)
void DebugDisplay::setBrightness(float brightness) {

}

void DebugDisplay::setBatteryVoltage(float newVoltage){

}
