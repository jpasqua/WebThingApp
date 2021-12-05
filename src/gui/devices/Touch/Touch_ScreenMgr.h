/*
 * ScreenMgr
 *    Manages the screens used in a WebThing app
 *
 */

#ifndef Touch_ScreenMgr_h
#define Touch_ScreenMgr_h

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
//                                  WebThing Includes
//                                  Local Includes
#include "../../Display.h"
#include "../../Theme.h"
//--------------- End:    Includes ---------------------------------------------


class Touch_ScreenMgr : public BaseScreenMgr {
public:
  Touch_ScreenMgr() = default;
  ~Touch_ScreenMgr() = default;

  virtual void device_setup() override;
  virtual void device_processInput() override;
  virtual void showActivityIcon(uint16_t accentColor, char symbol = 'i') override;
  virtual void hideActivityIcon() override;


private:
  static constexpr uint16_t AI_Size = 32;
  static constexpr uint16_t AI_BorderSize = 5;
  static constexpr uint16_t AI_X = (320 - AI_Size);
  static constexpr uint16_t AI_Y = 0;

  void initActivityIcon();

  bool isDisplayed;
  uint16_t *savedPixels = NULL;
};

extern Touch_ScreenMgr ScreenMgr;

#endif  // Touch_ScreenMgr_h
