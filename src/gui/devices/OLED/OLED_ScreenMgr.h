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

  void device_setup() {
    Display.begin(_displayOptions);
    initActivityIcon();
  }

  void device_processInput() {
    // Nothing to do here...
  }
  
  void showActivityIcon(uint16_t accentColor, char symbol = 'i') {
    if (isDisplayed) return;

    uint16_t AI_X = Display.Width - AI_Size;
    uint16_t AI_Y = 0;

    uint16_t centerX = AI_X+(AI_Size/2);
    uint16_t centerY = AI_Y+(AI_Size/2);

    // Draw the border
    OLEDDISPLAY_COLOR color = accentColor ? OLEDDISPLAY_COLOR::WHITE : OLEDDISPLAY_COLOR::BLACK;
    Display.oled->setColor(color);
    Display.oled->fillCircle(centerX, centerY, AI_Size/2-1);

    // Draw the fill
    Display.oled->setColor(Theme::Color_UpdatingFill);
    Display.oled->fillCircle(centerX, centerY, (AI_Size/2-1)-AI_BorderSize);

    // Draw the text
    Display.oled->setColor(Theme::Color_UpdatingText);
    Display.setFont(Display.FontID::S10);
    char buf[2]; buf[0] = symbol; buf[1] = '\0';
    Display.oled->drawString(centerX, centerY + Display.getFontHeight(Display.FontID::S10)/2, buf);

    isDisplayed = true;
  }

  void hideActivityIcon() {
    if (!isDisplayed) return;
    refresh();
    isDisplayed = false;
  }


private:
  static constexpr uint16_t AI_Size = 16;
  static constexpr uint16_t AI_BorderSize = 2;

  void initActivityIcon() {  }

  bool isDisplayed = false;
};

extern OLED_ScreenMgr ScreenMgr;
#endif  // OLED_ScreenMgr_h
