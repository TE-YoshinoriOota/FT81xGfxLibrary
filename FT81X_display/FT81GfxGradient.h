#ifndef FT81GFXGRADIENT_HEADER_GUARD
#define FT81GFXGRADIENT_HEADER_GUARD


#include "FT81GfxComponent.h"
#include "FT81RegisterOperation.h"
#include "FT81CommandOperation.h"
#include "FT81Display.h"


/********************************************/
/*  FT81xGfxGradient Class                  */
/********************************************/

class FT81xGfxGradient : public FT81xGfxComponent {
public:
  virtual ~FT81xGfxGradient();
  virtual void draw();
  void setStartColor(const uint8_t r, const uint8_t g, const uint8_t b) { m_color1 = COLOR_RGB(r, g, b); }
  void setEndColor(const uint8_t r, const uint8_t g, const uint8_t b) { m_color2 = COLOR_RGB(r, g, b); }
  void setPosition(const uint16_t x, const uint16_t y) { m_x = x; m_y = y; }
  void setSize(const uint16_t w, const uint16_t h) { m_w = w; m_h = h; } 

protected:
  friend class FT81xDisplay;
  FT81xGfxGradient(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp);

private:
  uint16_t m_x;
  uint16_t m_y;
  uint16_t m_w;
  uint16_t m_h;
  uint32_t m_color1;
  uint32_t m_color2;
};

#endif // FT81GFXGRADIENT_HEADER_GUARD
