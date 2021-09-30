/*
 * PhysicalButtonMgr
 *     Handle input from physical buttons
 *
 */

#ifndef PhysicalButtonMgr_h
#define PhysicalButtonMgr_h

#include "BaseButton.h"


//
// PhysicalButton manages the input from a single physical button that is associated
// with an input pin. That pin is the unique identifier for this button.
//
struct PhysicalButton : public BaseButton {
  // ----- Constructors
  PhysicalButton(uint8_t pinNum): pin(pinNum)  {
    pinMode(pin, INPUT_PULLUP);
  }

  // ----- Member Functions

  // Determine whether the physical button has been pressed, and if so, what
  // type of press it was
  // @param pressType   [OUT] when the function returns true, pressType will
  //                    have been set to the type of press. If there was no
  //                    press, then it is unchanged.
  // @return true  -> The button was pressed
  //         false -> There was no press
  bool wasPressed(BaseButton::PressType& pressType) {
    return false;
    bool pressed = false;//digitalRead(pin);

    if (pressed) {
      if (startOfPress == 0) {
        startOfPress = millis();
        endOfPress = 0;
      }
    } else {
      if (startOfPress != 0) {
        if (endOfPress == 0) {
          endOfPress = millis();
        } else if (millis() - endOfPress > DebounceTime) {
          // Process the press
          uint32_t pressDuration = (millis() - startOfPress);
          if (pressDuration >= BaseButton::VeryLongPressInterval) pressType = BaseButton::PressType::VeryLongPress;
          else if (pressDuration >= BaseButton::LongPressInterval) pressType = BaseButton::PressType::LongPress;
          else pressType = BaseButton::PressType::NormalPress;

          endOfPress = 0;
          startOfPress = 0;
          return true;
        }
      }
    }
    return false;
  }

  // ----- Data Members
  uint8_t  pin;   // The physical pin associated with this button

private:
  // ----- Constants
  static constexpr uint32_t DebounceTime = 50;

  // ----- Data Members
  uint8_t  state = 1;
  uint32_t lastBounce = 0;
  uint32_t startOfPress = 0;
  uint32_t endOfPress = 0;
};


//
// Manages a collection os PhysicalButton's
//
class PhysicalButtonMgr  {
public:
  // ----- Types
  using Dispatcher = std::function<void(uint8_t, BaseButton::PressType)>;

  // ----- Constructors
  PhysicalButtonMgr() = default;

  // ----- Member Functions
  void addButton(PhysicalButton& pb) { buttons.push_back(pb); }
  void addButton(PhysicalButton&& pb) {  buttons.push_back(pb); }

  void setDispatcher(Dispatcher d) { dispatcher = d; }

  void processInput() {
    for (PhysicalButton& b : buttons) {
      BaseButton::PressType pt;
      if (b.wasPressed(pt)) {
        if (dispatcher) dispatcher(b.pin, pt);
      }
    }
  }

private:
  // ----- Data Members
  std::vector<PhysicalButton> buttons;
  Dispatcher dispatcher;
};

#endif  // PhysicalButtonMgr_h
