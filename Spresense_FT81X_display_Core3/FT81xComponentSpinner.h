#ifndef FT81XCOMPOENENTSPINNER_HEADER_GUARD
#define FT81XCOMPOENENTSPINNER_HEADER_GUARD

#include "FT81xGuiComponent.h"
#include "RegisterOperation.h"
#include "CommandOperation.h"
#include "FT81xDisplay.h"


/********************************************/
/*  FT81xComponentSpinner Class         */
/********************************************/

class FT81xComponentSpinner : public FT81xGuiComponent {
public:
  virtual ~FT81xComponentSpinner();
  virtual void draw();
  void setColor(const uint8_t r, const uint8_t g, const uint8_t b) { m_color = COLOR_RGB(r, g, b); }
  void setPosition(const uint16_t x, const uint16_t y) { m_x = x; m_y = y; }
  void setSize(const uint16_t w, const uint16_t h) { m_w = w; m_h = h; }
  void setCircleStyle() { m_style = dot_circle; }
  void setLineStyle() { m_style = dot_line; }
  void setClockStyle() { m_style = rot_clock; }
  void setDotStyle() { m_style = two_dots; }

protected:
  friend class FT81xDisplay;
  FT81xComponentSpinner(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp);

private:
  enum spinner_style {
     dot_circle
    ,dot_line
    ,rot_clock
    ,two_dots
  };

private:
  uint16_t m_x;
  uint16_t m_y;
  uint16_t m_w;
  uint16_t m_h;
  uint16_t m_options;
  uint16_t m_style;
  uint16_t m_scale;
  uint32_t m_color;
};



#endif