/*
 * ScreenMgr
 *    Manages the screens used in a WebThing app
 *
 */

#ifndef MTX_ScreenMgr_h
#define MTX_ScreenMgr_h

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
//                                  WebThing Includes
//                                  Local Includes
#include "../../Display.h"
#include "../../Theme.h"
//--------------- End:    Includes ---------------------------------------------

struct AnimationState {
  bool aiDisplayed = false;
  char symbol = 'i';
  uint16_t x = 0;
  uint16_t y = 0;
  int8_t pos = 0;
  int8_t inc = 1;
};

class MTX_ScreenMgr : public BaseScreenMgr {
public:
  MTX_ScreenMgr() = default;
  ~MTX_ScreenMgr() = default;

  virtual void device_setup() override;
  virtual void device_processInput() override;
  virtual void showActivityIcon(uint16_t accentColor, char symbol = 'i') override;
  virtual void hideActivityIcon() override;


private:
  static constexpr uint16_t AI_Size = 8;
  static constexpr uint16_t AI_BorderSize = 1;
  static constexpr uint16_t PixelsPerByte = 8;
  static constexpr size_t SavedPixelsSize = (AI_Size * AI_Size)/PixelsPerByte;
  static constexpr uint16_t AI_AnimationInterval = 20;

  void initActivityIcon();
  void saveBits();
  void restoreBits();

  AnimationState animationState;
  uint8_t savedPixels[SavedPixelsSize];
};

extern MTX_ScreenMgr ScreenMgr;
#endif  // MTX_ScreenMgr_h
