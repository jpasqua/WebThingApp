#include "../DeviceSelect.h"
#if DEVICE_TYPE == DEVICE_TYPE_TOUCH

#include "../../ScreenMgr.h"


void Touch_ScreenMgr::device_setup() {
  Display.begin(_displayOptions);
  initActivityIcon();
}

void Touch_ScreenMgr::device_processInput() {
  uint32_t curMillis = millis();
  uint16_t tx = 0, ty = 0;
  bool pressed = Display.tft.getTouch(&tx, &ty);
  if (pressed) _lastInteraction = curMillis;
  _curScreen->processTouch(pressed, tx, ty);

  // Test whether we should blank the screen
  if (_uiOptions->screenBlankMinutes && !isSuspended()) {
    if (curMillis > _lastInteraction + minutesToMS(_uiOptions->screenBlankMinutes)) suspend();
  }
}

void Touch_ScreenMgr::showActivityIcon(uint16_t accentColor, char symbol) {
  if (isDisplayed) return;

  Display.tft.readRect(AI_X, AI_Y, AI_Size, AI_Size, savedPixels);
  uint16_t centerX = AI_X+(AI_Size/2);
  uint16_t centerY = AI_Y+(AI_Size/2);
  Display.tft.fillCircle(centerX, centerY, AI_Size/2-1, accentColor);
  Display.tft.fillCircle(centerX, centerY, (AI_Size/2-1)-AI_BorderSize, Theme::Color_UpdatingFill);
  Display.tft.setTextDatum(MC_DATUM);
  Display.setFont(Display.FontID::SB9);
  Display.tft.setTextColor(Theme::Color_UpdatingText);

  char buf[2];
  buf[0] = symbol; buf[1] = '\0';
  Display.tft.drawString(buf, centerX, centerY);

  isDisplayed = true;
}

void Touch_ScreenMgr::hideActivityIcon() {
  if (!isDisplayed) return;
  Display.tft.pushRect(AI_X, AI_Y, AI_Size, AI_Size, savedPixels);
  isDisplayed = false;
}


void Touch_ScreenMgr::initActivityIcon() {
  // In theory it would be better to allocate/deallocate this as needed, but it causes
  // a lot more fragmentation and potentially a crash.
  savedPixels = (uint16_t *)malloc(AI_Size*AI_Size*sizeof(uint16_t));  // This is BIG!
}


// ----- GLOBAL STATE
Touch_ScreenMgr ScreenMgr;

#endif