#ifndef Screen_h
#define Screen_h

#include "Display.h"
#include "Label.h"
#include <WTButton.h>

class BaseScreen {
public:
  // ----- Types
  using ButtonHandler = std::function<void(uint8_t, PressType)>;

  // ----- Constructors
  BaseScreen() = default;
  ~BaseScreen() = default;

  // ----- Member Functions
  virtual void display(bool force = false) = 0;
  virtual void processPeriodicActivity() = 0;

  void activate();

  // ----- ScreenMgr uses these functions to pass in input activity
  void processTouch(bool pressed, uint16_t tx, uint16_t ty);
  bool physicalButtonPress(uint8_t pin, PressType pt);

protected:
  // ----- Data Members
  ButtonHandler buttonHandler;

  Label* labels;
  uint8_t nLabels;

  WTButton::Mapping* buttonMappings;
  uint8_t nButtonMappings;

private:
  // ----- Constants
  static constexpr uint32_t DebounceTime = 100;

  // ----- Data Members
  uint32_t startOfPress = 0;
  uint32_t endOfPress = 0;
  uint16_t lastX, lastY;

};

#include "devices/DeviceSelect.h"
#include DeviceImplFor(Screen)

#endif // Screen_h