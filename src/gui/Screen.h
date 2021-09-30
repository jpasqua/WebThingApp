#ifndef Screen_h
#define Screen_h

#include <map>
#include "Display.h"
#include "Button.h"


class Screen {
public:
  // ----- Constructors
  Screen() = default;

  // ----- Member Functions that must be implemented by subclasses
  virtual void display(bool force = false) = 0;
  virtual void processPeriodicActivity() = 0;

  // ----- Member Functions that may be overriden by subclasses
  virtual void activate() {
    startOfPress = endOfPress = 0;
    display(true);
  }

  // ----- Member Functions
  void processInput(bool pressed, uint16_t tx, uint16_t ty) {
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
          Button::PressType pt;
          uint32_t pressDuration = (millis() - startOfPress);
          if (pressDuration >= Button::VeryLongPressInterval) pt = Button::PressType::VeryLongPress;
          else if (pressDuration >= Button::LongPressInterval) pt = Button::PressType::LongPress;
          else pt = Button::PressType::NormalPress;

          for (int i = 0; i < nButtons; i++) {
            if (buttons[i].processTouch(lastX, lastY, pt)) break;
          }
          endOfPress = 0;
          startOfPress = 0;
        }
      }
    }
  }


  // ----- Data Members
  BaseButton::ButtonCallback physicalButtonHandler = nullptr;
  std::map<uint8_t/*physical*/, uint8_t/*screen*/> screenButtonFromPhysicalButton;

  Button* buttons;
  uint8_t nButtons;


private:
  // ----- Constants
  static constexpr uint32_t DebounceTime = 100;

  // ----- Data Members
  uint32_t startOfPress = 0;
  uint32_t endOfPress = 0;
  uint16_t lastX, lastY;
};

#endif // Screen_h