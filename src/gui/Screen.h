#ifndef Screen_h
#define Screen_h

#include "Display.h"
#include "Label.h"
#include <WTButton.h>

class BaseScreen {
public:
  // ----- Types
  using ButtonHandler = std::function<void(uint8_t, PressType)>;

  // ----- Constants
  static constexpr uint8_t MaxLabels = 254;
  static constexpr uint8_t MaxButtonMappings = 254;
  static constexpr uint8_t PassAllRawButtons = 255;

  // ----- Constructors
  BaseScreen() = default;
  ~BaseScreen() = default;

  // ----- The following must be implemented by subclasses
  virtual void display(bool force = false) = 0;
  virtual void processPeriodicActivity() = 0;

  // ---- The following may be implemented by subclasses
  virtual void activate();

  // ----- ScreenMgr calls this to pass along a button press
  bool physicalButtonPress(uint8_t pin, PressType pt);

  String name;
  bool special; // Is this a "one-off" screen like the splash screen, or
                // will it be used in the normal course of operation
                // e.g. in a sequence

protected:
  // ----- Data Members
  // These must all be set be concrete subclasses

  ButtonHandler buttonHandler;
    // The function to call when a button press of any type occurs
    // It might be a physical button or a virtual touch button

  Label* labels = nullptr;
  uint8_t nLabels = 0;
    // The label areas in use by this screen. This is only relevant
    // to the Screen class if any of the labels correspond to touch
    // targets on a touch screen display.

  WTButton::Mapping* buttonMappings = nullptr;
  uint8_t nButtonMappings = 0;
    // The physical buttons that this screen cares about. If
    // nButtonMappings is set to PassAllRawButtons, then the
    // array of mappings is ignored and all button presses are
    // passed to the buttonHandler with an ID that corresponds
    // to the button's associated pin.

};

#include "devices/DeviceSelect.h"
#include DeviceImplFor(Screen)

#endif // Screen_h