#include "../DeviceSelect.h"
#if DEVICE_TYPE == DEVICE_TYPE_OLED

#include "../../ScreenMgr.h"

void OLED_ScreenMgr::device_setup() {
  Display.begin(_displayOptions);
  initActivityIcon();
}

void OLED_ScreenMgr::device_processInput() {
  // Unlike Touch screens, OLED screens can't generate input events so there
  // is nothing to do here.
}

/*------------------------------------------------------------------------------
 *
 * Activity Icon Implementation: Public
 *
 *----------------------------------------------------------------------------*/

void OLED_ScreenMgr::showActivityIcon(uint16_t accentColor, char symbol) {
  if (aiDisplayed) return;

  saveBits();

  auto oled = Display.oled;
  uint16_t AI_X = Display.Width - AI_Size;
  uint16_t AI_Y = 0;

  uint16_t centerX = AI_X+(AI_Size/2);
  uint16_t centerY = AI_Y+(AI_Size/2);

  // Draw the border
  OLEDDISPLAY_COLOR color = accentColor ? OLEDDISPLAY_COLOR::WHITE : OLEDDISPLAY_COLOR::BLACK;
  oled->setColor(color);
  oled->fillCircle(centerX, centerY, AI_Size/2-1);

  // Draw the fill
  oled->setColor(Theme::Color_UpdatingFill);
  oled->fillCircle(centerX, centerY, (AI_Size/2-1)-AI_BorderSize);

  // Draw the text
  oled->setColor(Theme::Color_UpdatingText);
  oled->setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
  Display.setFont(Display.FontID::S10);
  char buf[2]; buf[0] = symbol; buf[1] = '\0';
  oled->drawString(centerX, centerY, buf);

  oled->display();
  aiDisplayed = true;
}

void OLED_ScreenMgr::hideActivityIcon() {
  if (!aiDisplayed) return;
  restoreBits();
  // refresh();
  aiDisplayed = false;
}

/*------------------------------------------------------------------------------
 *
 * Activity Icon Implementation: Private
 *
 *----------------------------------------------------------------------------*/

void OLED_ScreenMgr::initActivityIcon() { }

//
// Memory Layout of the frame buffer:
// https://learn.sparkfun.com/tutorials/microview-hookup-guide/oled-memory-map
//
// NOTE: Technically we cannot assume that oled->buffer is a pointer to the first
// 8 pixels. There may be a buffer offset which is given by oled->getBufferOffset().
// Unfortunately, that's a protected function so we can't get to it. Right now,
// all existing subclasses of OLEDDisplay set this to zero. To be safe we should
// find a way to read the offset. Perhaps create an empty subclass so we can get
// at the data.
//

void OLED_ScreenMgr::saveBits() {
  uint8_t* src = Display.oled->buffer + BufferOffsetOfAI;
  memcpy(savedPixels,           src,                       RowSize);
  memcpy(savedPixels + RowSize, src + OLED_Display::Width, RowSize);
}

void OLED_ScreenMgr::restoreBits() {
  uint8_t* dst = Display.oled->buffer + BufferOffsetOfAI;
  memcpy(dst,                       savedPixels,           RowSize);
  memcpy(dst + OLED_Display::Width, savedPixels + RowSize, RowSize);
  Display.oled->display();
}


// ----- GLOBAL STATE
OLED_ScreenMgr ScreenMgr;

#endif