#ifndef FT81GFXCIRCLE_HEADER_GUARD
#define FT81GFXCIRCLE_HEADER_GUARD

#include "FT81GfxComponent.h"


/********************************************/
/*  FT81xGfxCircle Class                    */
/********************************************/

class FT81xGfxCircle : public FT81xGfxComponent {
public:
  virtual ~FT81xGfxCircle();
  virtual void draw();
  void setPosition(const uint16_t x, const uint16_t y) { m_x = x; m_y = y; }
  void setColor(const uint8_t r, const uint8_t g, const uint8_t b) { m_color = COLOR_RGB(r, g, b); }
  void setSize(const uint16_t r) { m_r = r; }

protected:
 friend class FT81xDisplay;
 FT81xGfxCircle(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp);

private:
  uint16_t m_x;
  uint16_t m_y;
  uint16_t m_r;
  uint32_t m_color;
};

#endif // FT81GFXCIRCLE_HEADER_GUARD
