#ifndef FT81GFXTRIANGLE_HEADER_GUARD
#define FT81GFXTRIANGLE_HEADER_GUARD


#include "FT81GfxComponent.h"
#include "FT81RegisterOperation.h"
#include "FT81CommandOperation.h"
#include "FT81Display.h"


/********************************************/
/*  FT81xGfxTriangle Class                  */
/********************************************/

class FT81xGfxTriangle : public FT81xGfxComponent {
public:
  virtual ~FT81xGfxTriangle();
  virtual void draw();
  void setColor(const uint8_t r, const uint8_t g, const uint8_t b) { m_fgcolor = COLOR_RGB(r, g, b); }
  void setPoints(const uint16_t x1, const uint16_t y1, const uint16_t x2, const uint16_t y2, const uint16_t x3, const uint16_t y3); 

protected:
  friend class FT81xDisplay;
  FT81xGfxTriangle(RegisterOperation* reg, CommandOperation* cmd, FT81xDisplay *disp);

private:
  uint16_t m_x1;
  uint16_t m_y1;
  uint16_t m_x2;
  uint16_t m_y2;
  uint16_t m_x3;
  uint16_t m_y3;    
  uint32_t m_fgcolor;
  uint32_t m_bgcolor;
};

#endif // FT81GFXTRIANGLE_HEADER_GUARD
