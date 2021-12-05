#ifndef OLED_NullDriver_h
#define OLED_NullDriver_h

#include <OLEDDisplay.h>

class OLED_NullDriver : public OLEDDisplay {
public:
    OLED_NullDriver(OLEDDISPLAY_GEOMETRY g = GEOMETRY_128_64) { setGeometry(g); }
    bool connect() override { return true; }
    void display(void) override { }

private:
  virtual int getBufferOffset(void) override { return 0; };
};

#endif // OLED_NullDriver_h
