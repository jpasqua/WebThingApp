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

  void initActivityIcon();
  void saveBits();
  void restoreBits();

  bool aiDisplayed = false;
  uint8_t savedPixels[SavedPixelsSize];
};

extern MTX_ScreenMgr ScreenMgr;
#endif  // MTX_ScreenMgr_h
