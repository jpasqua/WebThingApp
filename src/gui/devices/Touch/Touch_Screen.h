#ifndef Touch_Screen_h
#define Touch_Screen_h

#include "../../Label.h"

class Screen : public BaseScreen {
public:
  // ----- Constructors
  Screen() = default;
  ~Screen() = default;

  void processTouch(bool pressed, uint16_t tx, uint16_t ty) {
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

protected:
  Label* labels;
  uint8_t nLabels;

private:
  // ----- Constants
  static constexpr uint32_t DebounceTime = 100;

  // ----- Data Members
  uint32_t startOfPress = 0;
  uint32_t endOfPress = 0;
  uint16_t lastX, lastY;
};

#endif // Touch_Screen_h
