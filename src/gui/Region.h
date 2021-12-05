#ifndef Region_h
#define Region_h

class Region {
public:
  inline bool contains(uint16_t tx, uint16_t ty) {
    return ((tx >= x) && (tx < x+w) && (ty >= y) && (ty < y+h));
  }

  inline uint16_t bottom() const { return (y + h); }
  inline uint16_t right() const { return (x + w); }

  uint16_t x;
  uint16_t y;
  uint16_t w;
  uint16_t h;
};

#endif // Region_h