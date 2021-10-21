/*
 * ScreenMgr
 *    Manages the screens used in a WebThing app
 *
 */

#ifndef OLED_ScreenMgr_h
#define OLED_ScreenMgr_h

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
//                                  WebThing Includes
//                                  Local Includes
#include "../../Display.h"
#include "../../Theme.h"
//--------------- End:    Includes ---------------------------------------------


class OLED_ScreenMgr : public BaseScreenMgr {
public:
  OLED_ScreenMgr() = default;
  ~OLED_ScreenMgr() = default;

  virtual void device_setup() override;

  virtual void device_processInput() override;
  
  virtual void showActivityIcon(uint16_t accentColor, char symbol = 'i') override;

  virtual void hideActivityIcon() override;


private:
  static constexpr uint16_t AI_Size = 16;
  static constexpr uint16_t AI_BorderSize = 2;
  static constexpr uint16_t PixelsPerByte = 8;
  static constexpr size_t SavedPixelsSize = (AI_Size * AI_Size)/PixelsPerByte;
  static constexpr size_t BufferOffsetOfAI = OLED_Display::Width - AI_Size;
  static constexpr size_t RowSize = SavedPixelsSize/2;

  void initActivityIcon();
  void saveBits();
  void restoreBits();

  bool aiDisplayed = false;
  uint8_t savedPixels[SavedPixelsSize];
};

extern OLED_ScreenMgr ScreenMgr;
#endif  // OLED_ScreenMgr_h
