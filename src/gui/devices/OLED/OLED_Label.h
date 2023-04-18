#ifndef OLED_Label_h
#define OLED_Label_h

class Label : public BaseLabel {
public:
  Label() = default;

  Label(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t id);

  virtual void clear(uint16_t color) override;

  virtual void drawSimple(
    const String& label, uint8_t font, uint8_t borderSize,
    uint16_t labelColor, uint16_t borderColor, uint16_t bgColor,
    bool buffer = false) override;

  virtual void drawProgress(
    float pct, const String& label, uint8_t font, uint8_t borderSize,
    uint16_t labelColor, uint16_t borderColor,
    uint16_t barColor, uint16_t bgColor, bool showPct,
    bool buffer = false) override;

};

#endif // OLED_Label_h
