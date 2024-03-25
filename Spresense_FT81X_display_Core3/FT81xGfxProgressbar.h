#ifndef FT81XCOMPONENTPROGRESSBAR_HEADER_GUARD
#define FT81XCOMPONENTPROGRESSBAR_HEADER_GUARD

#include "FT81xGfxComponent.h"
#include "RegisterOperation.h"
#include "CommandOperation.h"

class FT81xDisplay;

/********************************************/
/*  FT81xGfxProgressbar Class         */
/********************************************/

class FT81xGfxProgressbar : public FT81xGfxComponent {
public:
  virtual ~FT81xGfxProgressbar();
  virtual void draw();
  void draw(const uint16_t value);
  void setColor(const uint8_t r, const uint8_t g, const uint8_t b) { m_fgcolor = COLOR_RGB(r, g, b); }
  void setBgColor(const uint8_t r, const uint8_t g, const uint8_t b) { m_bgcolor = COLOR_RGB(r, g, b); }
  void setPosition(const uint16_t x, const uint16_t y) { m_x = x; m_y = y; }
  void setSize(const uint16_t w, const uint16_t h) { m_w = w; m_h = h; }
  void setRange(const uint16_t range) { m_range = range; }
  void setValue(const uint16_t value) { m_value = value; }

protected:
  friend class FT81xDisplay;
  FT81xGfxProgressbar(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp);

private:
  uint16_t m_x;
  uint16_t m_y;
  uint16_t m_w;
  uint16_t m_h;
  uint16_t m_options;
  uint16_t m_value;
  uint16_t m_range;
  uint32_t m_fgcolor;
  uint32_t m_bgcolor;
};




#endif
