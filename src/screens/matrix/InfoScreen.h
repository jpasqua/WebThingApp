#ifndef InfoScreen_h
#define InfoScreen_h

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
//                                  WebThing Includes
#include <BPABasics.h>
#include <WebThing.h>
//                                  Local Includes
#include "ScrollScreen.h"
//--------------- End:    Includes ---------------------------------------------


class InfoScreen : public ScrollScreen {
public:
  InfoScreen();
  virtual void innerActivation() override;


private:
};

#endif  // InfoScreen_h
