//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
#include <Arduino.h>
//                                  Third Party Libraries
#include <TimeLib.h>
//                                  WebThingApp
#include <gui/Display.h>
//                                  Local Includes
#include "../../OLEDTestApp.h"
//--------------- End:    Includes ---------------------------------------------


static uint16_t compose(int h, int m) { return(h * 100 + m); }

static constexpr auto CL_Font = Display.FontID::S10;
static constexpr uint16_t CL_FontHeight = 13;
static constexpr uint16_t CL_XOrigin = 1;
static constexpr uint16_t CL_YOrigin = 51;
static constexpr uint16_t CL_Width = 42;
static constexpr uint16_t CL_Height = CL_FontHeight;
static constexpr uint16_t CL_BorderSize = 1;

static constexpr uint8_t Currency1Label = 0;
static constexpr uint8_t Currency2Label = Currency1Label + 1;
static constexpr uint8_t Currency3Label = Currency2Label + 1;
static constexpr uint8_t NCurrencyLabels = Currency3Label + 1;

static constexpr auto CN_Font = Display.FontID::S10;
static constexpr uint16_t CN_FontHeight = 13;


HomeScreen::HomeScreen() {
  uint16_t x = CL_XOrigin;
  labels = new Label[NCurrencyLabels];
  for (int i = 0; i < NCurrencyLabels; i++) {
    labels[i].init(x, CL_YOrigin, CL_Width, CL_Height, i);
    x += CL_Width;
  }
}

void HomeScreen::display(bool) {
  auto oled = Display.oled;
  
  oled->clear();

  bool am = true;
  int  m = minute();
  int  h = hour();

  if (h > 12) { h -= 12; am = false; }
  else if (h == 0) { h = 12;}
  else if (h == 12) { am = false; }

  constexpr uint8_t bufSize = 32;
  char buf[bufSize];
  snprintf(buf, bufSize, "%d:%02d", h, m);
  Display.setFont(Display.FontID::D32);
  oled->setTextAlignment(TEXT_ALIGN_RIGHT);
  oled->drawString(Display.Width-24, 2, buf);
  oled->setTextAlignment(TEXT_ALIGN_LEFT);
  Display.setFont(Display.FontID::S10);
  oled->drawString(Display.Width-20, 4, am ? "AM" : "PM");

  drawCurrencies();
  drawCurrencyNames();

  oled->display();

  compositeTime = compose(h, m);
}

void HomeScreen::processPeriodicActivity() {
  if (compositeTime != compose(hour(), minute())) display(true);
}

// ----- Private Functions

void HomeScreen::drawCurrencyNames() {
  uint16_t yPos = CL_YOrigin - CN_FontHeight + 1; // Add an extra pixel for padding
  uint16_t xDelta = Display.Width/NCurrencyLabels;
  uint16_t xPos = 0 + xDelta/2;
  Display.oled->setTextAlignment(TEXT_ALIGN_CENTER);
  Display.oled->setColor(Theme::Color_NormalText);
  for (int i = 0; i < NCurrencyLabels; i++) {
    String label = otSettings->currencies[i].nickname;

    if (otApp->currencies[i].inactive()) {
      label = "Unused";
    } else {
      if (label.isEmpty()) label = otSettings->currencies[i].id;
    }

    Display.oled->setColor(Theme::Color_NormalText);
    Display.setFont(CN_Font);
    Display.oled->drawString(xPos, yPos, label);      
    xPos += xDelta;
  }
}

void HomeScreen::drawCurrencies() {
  for (int i = 0; i < NCurrencyLabels; i++) {
    String value = "N/A";
    if (otApp->currencies[i].active())
      value = String(otApp->currencies[i].exchangeRate, 2);

    labels[i].drawSimple(
      value, CL_Font, CL_BorderSize, Theme::Color_NormalText, Theme::Color_Border, Theme::Color_Background);
  }
}
