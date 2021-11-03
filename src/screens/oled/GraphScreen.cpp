#include "../../gui/devices/DeviceSelect.h"
#if DEVICE_TYPE == DEVICE_TYPE_OLED

//--------------- Begin:  Includes ---------------------------------------------
//                                  Core Libraries
//                                  Third Party Libraries
//                                  WebThing Includes
#include <BPABasics.h>
#include <WebThing.h>
//                                  Local Includes
#include "GraphScreen.h"
#include "../../gui/Display.h"
//--------------- End:    Includes ---------------------------------------------

static constexpr Region GraphRegion {27, 0, 100, 54};

static const char* fmts[] {
  "%.2f",   // <            10
  "%.1f",   // <           100
  "%.0f",   // <         1,000
  "%.1fK",  // <        10,000
  "%.0fK",  // <       100,000
  "%.0fK",  // <     1,000,000
  "%.1fM",  // <    10,000,000
  "%.0fM",  // <   100,000,000
  "%.0fM"   // < 1,000,000,000
};
static constexpr size_t nFmts = countof(fmts);

static String fourDigits(float val) {
  char buf[6];  // Space for -123M[NULL]
  bool negative = (val < 0);
  if (negative) val = -val;
  float cutoff = 10;

  for (uint16_t i = 0; i < nFmts; i++) {
    if (val < cutoff) {
      val /= pow(10, (i/3)*3);
      if (negative) val = -val;
      sprintf(buf, fmts[i], val);
      return(String(buf));
    }
    cutoff *= 10;
  }
  return String(negative ? "----" : "++++");
}

GraphScreen::GraphScreen() { }

void GraphScreen::display(bool activating) {
  auto& oled = Display.oled;

  oled->clear();

  // Draw the axes:
  oled->setColor(OLEDDISPLAY_COLOR::WHITE);
  oled->drawHorizontalLine(GraphRegion.x, GraphRegion.bottom(), GraphRegion.w);
  oled->drawVerticalLine(GraphRegion.x, GraphRegion.y, GraphRegion.h);

  uint16_t nPoints = dataProvider->count();
  float hiVal = dataProvider->hiVal(activating);
  float loVal = dataProvider->loVal(activating);

  float yAxisMin = displayParams.suggestedYAxisMin;
  float yAxisMax = displayParams.suggestedYAxisMax;
  if (displayParams.autoScale) {
    yAxisMin = loVal * 0.95;
    yAxisMax = hiVal * 1.05;
  } else {
    if (hiVal > yAxisMax) yAxisMax = hiVal;
    if (loVal < yAxisMin) yAxisMin = loVal;
  }
  float range = yAxisMax - yAxisMin;

  Display.setFont(Display.FontID::M5);
  oled->setTextAlignment(TEXT_ALIGN_RIGHT);
  oled->drawString(GraphRegion.x-2, GraphRegion.bottom()-7, fourDigits(yAxisMin));
  oled->drawString(GraphRegion.x-2, GraphRegion.y, fourDigits(yAxisMax));

  String legend = dataProvider->getYLegend();
  if (legend.length() > 5) legend.remove(5);
  oled->drawString(GraphRegion.x-2, GraphRegion.y + GraphRegion.h/2, legend);

  uint16_t maxXLabelWidth = GraphRegion.w/2 - 5;  // Split the space and leave a buffer
  oled->setTextAlignment(TEXT_ALIGN_LEFT);
  String minLabel, maxLabel;
  dataProvider->getXLabels(minLabel, maxLabel);
  oled->drawStringMaxWidth(GraphRegion.x, Display.Height-7, maxXLabelWidth, minLabel);
  oled->setTextAlignment(TEXT_ALIGN_RIGHT);
  oled->drawStringMaxWidth(Display.Width, Display.Height-7, maxXLabelWidth, maxLabel);
  legend = dataProvider->getXLegend();
  oled->setTextAlignment(TEXT_ALIGN_CENTER);
  oled->drawString(GraphRegion.x+GraphRegion.w/2, Display.Height-7, legend);

  // Draw the actual graph contents
  uint16_t lastX, lastY;
  uint16_t bottom = GraphRegion.bottom();
  float x = GraphRegion.x;
  float deltaX = ((float)GraphRegion.w)/(nPoints-1);
  for (int i = 0; i < nPoints; i++) {
    float pctOfRange = ((dataProvider->getVal(i) - yAxisMin)/range);
    uint16_t y = bottom - (pctOfRange * GraphRegion.h);
    if (i) {
      // Log.verbose("(%d, %d) -> (%F, %d)", lastX, lastY, x, y);
      oled->drawLine(lastX, lastY, x, y);
    }
    lastX = x; lastY = y;
    x += deltaX;
  }

  oled->display();
}

#endif