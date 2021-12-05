#include <ArduinoLog.h>
#include "Screen.h"

void BaseScreen::activate() {
  display(true);
}

bool BaseScreen::physicalButtonPress(uint8_t pin, PressType pt) {
  if (nButtonMappings == PassAllRawButtons) {
    // We want every raw button press passed through...
    buttonHandler(pin, pt);
    return true;
  }

  for (int i = 0; i < nButtonMappings; i++) {
    if (pin == buttonMappings[i].pin) {
      buttonHandler(buttonMappings[i].id, pt);
      return true;
    }
  }
  return false;
}
