#ifndef InfoScreen_h
#define InfoScreen_h

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
//                                  WebThing Includes
#include <BPABasics.h>
#include <WebThing.h>
//                                  Local Includes
#include "../../WTApp.h"
#include "../../gui/Display.h"
#include "../../gui/Theme.h"
#include "../../gui/ScreenMgr.h"
//--------------- End:    Includes ---------------------------------------------


class InfoScreen : public Screen {
public:
  InfoScreen() { }

  virtual void display(bool);

  virtual void processPeriodicActivity() { }

private:
  void drawRssi(uint16_t x, uint16_t y);  
};

#endif  // InfoScreen_h
