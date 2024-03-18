#ifndef FT81GUIDIAL_HEADER_GUARD
#define FT81GUIDIAL_HEADER_GUARD


#include "FT81GuiComponent.h"


/********************************************/
/*  FT81xGuiDial Class                      */
/********************************************/  

class FT81xGuiDial : public FT81xGuiComponent {
public:
  virtual ~FT81xGuiDial();
  virtual void draw();
  void setColor(const uint8_t r, const uint8_t g, const uint8_t b) { m_color = COLOR_RGB(r, g, b); }
  void setDialColor(const uint8_t r, const uint8_t g, const uint8_t b) { m_bgcolor = COLOR_RGB(r, g, b); }
  void setPosition(const uint16_t x, const uint16_t y) { m_x = x; m_y = y; }
  void setSize(const uint16_t r) { m_rad = r; }
  uint16_t readValue() { return m_normValue; }

protected:
  friend class FT81xDisplay;
  FT81xGuiDial(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp);
  virtual void doInteraction_(const uint32_t value);
  virtual bool isTracker() { return true; }
  
private:
  uint16_t m_x;
  uint16_t m_y;
  uint16_t m_rad;
  uint32_t m_color;
  uint32_t m_bgcolor;
  uint16_t m_options;
  uint16_t m_value;
  uint16_t m_normValue;
};


#endif // FT81GUIDIAL_HEADER_GUARD
