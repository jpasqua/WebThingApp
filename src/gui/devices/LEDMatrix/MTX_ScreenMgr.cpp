#include "../DeviceSelect.h"
#if DEVICE_TYPE == DEVICE_TYPE_MTX

#include "../../ScreenMgr.h"

void MTX_ScreenMgr::device_setup() {
  Display.begin(_displayOptions);
  initActivityIcon();
}

void MTX_ScreenMgr::device_processInput() {
  // Unlike Touch screens, LED Matrix screens can't generate input events so there
  // is nothing to do here.
}

/*------------------------------------------------------------------------------
 *
 * Activity Icon Implementation: Public
 *
 *----------------------------------------------------------------------------*/


void MTX_ScreenMgr::showActivityIcon(uint16_t accentColor, char symbol) {
  if (aiDisplayed) return;

  saveBits();

  auto mtx = Display.mtx;
  Display.setFont(Display.BuiltInFont_ID);

  uint16_t AI_X = Display.mtx->width() - Display.BuiltInFont_CharWidth;
  uint16_t AI_Y = 0;

  mtx->drawChar(AI_X, AI_Y, symbol, Theme::Color_BLACK, Theme::Color_WHITE, 1);

  mtx->write();
  aiDisplayed = true;
}

void MTX_ScreenMgr::hideActivityIcon() {
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

void MTX_ScreenMgr::initActivityIcon() { }

void MTX_ScreenMgr::saveBits() {
  int x = (Display.deviceOptions->hDisplays-1) * 8; // Each display unit is 8 pixels wide
  int y = 0;
  int savedBytesIndex = 0;
  for (int xOff = 0; xOff < AI_Size; xOff++) {
    uint8_t curByte = 0;
    for (int yOff = 0; yOff < AI_Size; yOff++) {
      uint8_t pix = (Display.mtx->readPixel(x+xOff, y+yOff) != 0);
      curByte |= pix;
      curByte = curByte << 1;
    }
    savedPixels[savedBytesIndex++] = curByte;
  }
}

void MTX_ScreenMgr::restoreBits() {
  auto mtx = Display.mtx;
  int x = (Display.deviceOptions->hDisplays-1) * 8; // Each display unit is 8 pixels wide
  int y = 0;
  int savedBytesIndex = 0;
  for (int xOff = 0; xOff < AI_Size; xOff++) {
    uint8_t curByte = savedPixels[savedBytesIndex++];
    for (int yOff = AI_Size-1; yOff >= 0; yOff--) {
      uint8_t pix = curByte & 0x1;
      mtx->drawPixel(x+xOff, y+yOff, pix);
      curByte = curByte >> 1;
    }
  }
  mtx->write();
}


// ----- GLOBAL STATE
MTX_ScreenMgr ScreenMgr;

#endif