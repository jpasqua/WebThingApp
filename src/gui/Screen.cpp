#include <ArduinoLog.h>
#include "Screen.h"

void BaseScreen::activate() {
  startOfPress = endOfPress = 0;
  display(true);
}

bool BaseScreen::physicalButtonPress(uint8_t pin, PressType pt) {
  for (int i = 0; i < nButtonMappings; i++) {
    if (pin == buttonMappings[i].pin) {
      buttonHandler(buttonMappings[i].id, pt);
      return true;
    }
  }
  return false;
}
