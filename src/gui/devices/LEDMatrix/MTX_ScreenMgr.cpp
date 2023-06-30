#include "../DeviceSelect.h"
#if DEVICE_TYPE == DEVICE_TYPE_MTX

#include "../../ScreenMgr.h"

static constexpr int FixedLine = 0;
static constexpr int DynamicLine = 1;

void MTX_ScreenMgr::displayInfoScreen() { display("Info"); }

void MTX_ScreenMgr::device_setup() {
  Display.begin(_displayOptions);
  initActivityIcon();
}

void MTX_ScreenMgr::device_processInput() {
  // Unlike Touch screens, LED Matrix screens can't generate input events so there
  // is nothing to do here.
}

static uint32_t nextDisplayReset = 0;
constexpr uint32_t DisplayResetInterval = Basics::minutesToMS(5);

void MTX_ScreenMgr::device_changingScreens(Screen* screen) {
  uint32_t curMillis = millis();
  if (curMillis > nextDisplayReset) {
    // Periodically reset the display to get rid of any cruft
    if (Display.getBrightness()) Display.mtx->reset();
    nextDisplayReset = curMillis + DisplayResetInterval;
  }

  if (_fixedScreen) {
    if (_fixedScreenDisplayed || !screen->special) {
      Display.mtx->focusOnLine(FixedLine);
      _fixedScreen->display(true);
      _fixedScreenDisplayed = true;
      Display.mtx->focusOnLine(DynamicLine);
    } 
  }
}

void MTX_ScreenMgr::device_processPeriodicActivity() {
  if (_fixedScreen && _fixedScreenDisplayed) {
    Display.mtx->focusOnLine(FixedLine);
    _fixedScreen->processPeriodicActivity();
    Display.mtx->focusOnLine(DynamicLine);
  }
}


/*------------------------------------------------------------------------------
 *
 * Functions that are specific to DEVICE_TYPE_MTX
 *
 *----------------------------------------------------------------------------*/

void MTX_ScreenMgr::enableTwoLineOperation(Screen* fixedScreen) {
  _fixedScreen = fixedScreen;
  if (!_fixedScreen) return;
  Display.mtx->focusOnLine(DynamicLine);
}


/*------------------------------------------------------------------------------
 *
 * Activity Icon Implementation: Public
 *
 *----------------------------------------------------------------------------*/

#include <Ticker.h>
Ticker aiAnimationTicker;

static int previouslyActiveLine;

void animateAI(AnimationState* state) {
  Display.mtx->drawPixel(state->x, state->y - state->pos, state->inc < 0 ? 0 : 1);
  Display.mtx->write();
  state->pos += state->inc;
  if (state->pos == 8) { state->inc = -1; state->pos = 7; }
  else if (state->pos == -1) { state->inc = 1; state->pos = 0; }
}

void MTX_ScreenMgr::showActivityIcon(uint16_t, char symbol) {
  if (animationState.aiDisplayed) return;

  previouslyActiveLine = Display.mtx->swapFocus(DynamicLine);
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
  Display.mtx->focusOnLine(previouslyActiveLine);
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