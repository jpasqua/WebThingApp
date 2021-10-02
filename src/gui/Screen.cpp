#include <ArduinoLog.h>
#include "Screen.h"

void Screen::activate() {
  startOfPress = endOfPress = 0;
  display(true);
}

// ----- Member Functions
void Screen::processTouch(bool pressed, uint16_t tx, uint16_t ty) {
  if (pressed) {
    if (startOfPress == 0) {
      startOfPress = millis();
      endOfPress = 0;
    }
    lastX = tx; lastY = ty;
  } else {
    if (startOfPress != 0) {
      if (endOfPress == 0) {
        endOfPress = millis();
      } else if (millis() - endOfPress > DebounceTime) {
        // Process the press
        // Ok, we got a press/release, see which button (if any) is associated
        PressType pt;
        uint32_t pressDuration = (millis() - startOfPress);
        if (pressDuration >= WTButton::VeryLongInterval) pt = PressType::VeryLong;
        else if (pressDuration >= WTButton::LongInterval) pt = PressType::Long;
        else pt = PressType::Normal;

        for (int i = 0; i < nLabels; i++) {
          if (labels[i].region.contains(lastX, lastY)) {
            buttonHandler(labels[i]._id, pt);
            break;
          }
        }

        endOfPress = 0;
        startOfPress = 0;
      }
    }
  }
}

bool Screen::physicalButtonPress(uint8_t pin, PressType pt) {
  for (int i = 0; i < nButtonMappings; i++) {
    if (pin == buttonMappings[i].pin) {
      buttonHandler(buttonMappings[i].id, pt);
      return true;
    }
  }
  return false;
}
