/*
 * ScrollScreen:
 *    Base class for scrolling text displays.
 *                    
 */

#include "../../gui/devices/DeviceSelect.h"
#if DEVICE_TYPE == DEVICE_TYPE_MTX
#ifndef ScrollScreen_h
#define ScrollScreen_h

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
//                                  WebThing Includes
#include "../../gui/Screen.h"
//                                  Local Includes
//--------------- End:    Includes ---------------------------------------------

class ScrollScreen : public Screen {
public:
  static void setDefaultFrameDelay(uint32_t delay);
  
  ScrollScreen();
  void display(bool activating = false);
  void processPeriodicActivity();

  void init(bool autoAdvance = true, uint32_t delay = 0, uint8_t forceCycles = 0);
  void setText(String text, uint8_t fontID = Display.BuiltInFont_ID);
  void goHomeWhenComplete(bool goHome) { _goHome = goHome; }

  virtual void innerActivation() {};
  virtual bool innerPeriodic() { return false; }

private:
  static uint32_t defaultDelayBetweenFrames;
  void innerDisplay();

  String _text;
  uint8_t _fontID;
  uint16_t _textWidth;
  uint16_t _baseline;
  bool _goHome;

  uint16_t _offset = 0;
  uint16_t _mtxWidth;
  bool _autoAdvance = true;

  uint32_t _delayBetweenFrames = 20;
  uint32_t _nextTimeToDisplay = 0;
  uint8_t _forceCycles = 0;
  uint8_t _cyclesCompleted = 0;
};

#endif  // ScrollScreen_h
#endif