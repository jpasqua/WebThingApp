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

#include <Ticker.h>
Ticker aiAnimationTicker;

void animateAI(AnimationState* state) {
  Display.mtx->drawPixel(state->x, state->y - state->pos, state->inc < 0 ? 0 : 1);
  Display.mtx->write();
  state->pos += state->inc;
  if (state->pos == 8) { state->inc = -1; state->pos = 7; }
  else if (state->pos == -1) { state->inc = 1; state->pos = 0; }
}

void MTX_ScreenMgr::showActivityIcon(uint16_t, char symbol) {
  if (animationState.aiDisplayed) return;
  saveBits();
  Display.setFont(Display.BuiltInFont_ID);

  animationState.symbol = symbol;
  animationState.x = Display.mtx->width() - 1;  // Could be done at init time
  animationState.y = 7;                         // Could be done at init time
  animationState.pos = 0;
  animationState.inc = 1;
  animationState.aiDisplayed = true;
  Display.mtx->fillRect(animationState.x, 0, 1, 8, 0);  // Clear the area, but don't write it yet
  aiAnimationTicker.attach_ms(AI_AnimationInterval, animateAI, &(this->animationState));
}

void MTX_ScreenMgr::hideActivityIcon() {
  if (!animationState.aiDisplayed) return;
  aiAnimationTicker.detach();
  restoreBits();
  animationState.aiDisplayed = false;
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
      curByte = curByte << 1;
      curByte |= pix;
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