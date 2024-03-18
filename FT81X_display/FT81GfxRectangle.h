#ifndef FT81GFXRECTANGLE_HEADER_GUARD
#define FT81GFXRECTANGLE_HEADER_GUARD


#include "FT81GfxComponent.h"
#include "FT81RegisterOperation.h"
#include "FT81CommandOperation.h"
#include "FT81Display.h"


/********************************************/
/*  FT81xGfxRectangle Class                    */
/********************************************/

class FT81xGfxRectangle : public FT81xGfxComponent {
public:
  virtual ~FT81xGfxRectangle();
  virtual void draw();
  void setCorner(const uint8_t rad) { m_rad = rad; }
  void setColor(const uint8_t r, const uint8_t g, const uint8_t b) { m_color = COLOR_RGB(r, g, b); }
  void setPosition(const uint16_t x, const uint16_t y) { m_x = x; m_y = y; }
  void setSize(const uint16_t w, const uint16_t h) { m_w = w; m_h = h; }

protected:
  friend class FT81xDisplay;
  FT81xGfxRectangle(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp);

private:
  uint16_t m_x;
  uint16_t m_y;
  uint16_t m_h;
  uint16_t m_w;  
  uint32_t m_color;
  uint8_t  m_rad;
};

#endif // FT81GFXRECTANGLE_HEADER_GUARD
