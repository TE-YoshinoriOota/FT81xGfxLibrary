#ifndef FT81GUISCROLLBAR_HEADER_GUARD
#define FT81GUISCROLLBAR_HEADER_GUARD


#include "FT81GuiScrollbar.h"



/********************************************/
/*  FT81xGuiScrollbar Class            */
/********************************************/  

class FT81xGuiScrollbar : public FT81xGuiComponent {
public:
  virtual ~FT81xGuiScrollbar() {}
  virtual void draw();
  void setColor(const uint8_t r, const uint8_t g, const uint8_t b) { m_color = COLOR_RGB(r, g, b); }
  void setScrollbarColor(const uint8_t r, const uint8_t g, const uint8_t b) { m_bgcolor = COLOR_RGB(r, g, b); }
  void setPosition(const uint16_t x, const uint16_t y) { m_x = x; m_y = y; }
  void setSize(const uint16_t w, const uint16_t h) { m_w = w; m_h = h; }
  void setBarSize(const uint16_t size) { m_size = size; }
  void setRange(const uint16_t range) { m_range = range; }
  uint16_t readValue() { return m_value; }

protected:
  friend class FT81xDisplay;
  FT81xGuiScrollbar(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp);
  virtual void doInteraction_(const uint32_t value);
  virtual bool isTracker() { return true; }
  
private:
  uint16_t m_x;
  uint16_t m_y;
  uint16_t m_w;
  uint16_t m_h;
  uint32_t m_color;
  uint32_t m_bgcolor;
  uint16_t m_options;
  uint16_t m_value;
  uint16_t m_size;
  uint16_t m_range;
};


#endif // FT81GUISCROLLBARD_HEADER_GUARD
