#ifndef FT81XGFXLINE_HEADER_GUARD
#define FT81XGFXLINE_HEADER_GUARD

/********************************************/
/*  FT81xGfxLine Class                    */
/********************************************/

#include "FT81xGfxComponent.h"
#include "RegisterOperation.h"
#include "CommandOperation.h"

class FT81xDisplay;

class FT81xGfxLine : public FT81xGfxComponent {
public:
  virtual ~FT81xGfxLine();
  virtual void draw();
  void setColor(const uint8_t r, const uint8_t g, const uint8_t b) { m_color = COLOR_RGB(r, g, b); }
  void setPoints(const uint16_t x1, const uint16_t y1, const uint16_t x2, const uint16_t y2) { m_x1 = x1; m_y1 = y1; m_x2 = x2; m_y2 = y2; } 
  void setWidth(const uint16_t w) { m_w = w; }

protected:
  friend class FT81xDisplay;
  FT81xGfxLine(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp);

private:
  uint16_t m_x1;
  uint16_t m_y1;
  uint16_t m_x2;
  uint16_t m_y2;
  uint16_t m_w;
  uint32_t m_color;
};


#endif
