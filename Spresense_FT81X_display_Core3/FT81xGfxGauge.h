#ifndef FT81XCOMPOENENTGAUGE_HEADER_GUARD
#define FT81XCOMPOENENTGAUGE_HEADER_GUARD

#include "FT81xGfxComponent.h"
#include "RegisterOperation.h"
#include "CommandOperation.h"

class FT81xDisplay;

/********************************************/
/*  FT81xGfxGauge Class                     */
/********************************************/

class FT81xGfxGauge : public FT81xGfxComponent {
public:
  virtual ~FT81xGfxGauge();
  virtual void draw();
  void draw(const uint16_t value);
  void setColor(const uint8_t r, const uint8_t g, const uint8_t b) { m_fgcolor = COLOR_RGB(r, g, b); }
  void setBgColor(const uint8_t r, const uint8_t g, const uint8_t b) { m_bgcolor = COLOR_RGB(r, g, b); }
  void setNeedleColor(const uint8_t r, const uint8_t g, const uint8_t b) { m_ncolor = COLOR_RGB(r, g, b); }
  void setPosition(const uint16_t x, const uint16_t y) { m_x = x; m_y = y; }
  void setSize(const uint16_t r) { m_rad = r; };
  void setMeter(const uint16_t major, const uint16_t minor, const uint16_t range) { m_major = major; m_minor = minor; m_range = range; }
  void setValue(const uint16_t value) { m_value = value; }

protected:
  friend class FT81xDisplay;
  FT81xGfxGauge(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp);

private:
  uint16_t m_x;
  uint16_t m_y;
  uint16_t m_rad;
  uint8_t  m_major;
  uint8_t  m_minor;
  uint16_t m_value;
  uint16_t m_range;
  uint32_t m_fgcolor;
  uint32_t m_bgcolor;
  uint32_t m_ncolor;  
  uint16_t m_options;
};


#endif
