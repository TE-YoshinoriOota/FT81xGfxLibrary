#ifndef FT81XINTERACTIONSLIDER_HEADER_GUARD
#define FT81XINTERACTIONSLIDER_HEADER_GUARD

#include "FT81xGuiComponent.h"
#include "RegisterOperation.h"
#include "CommandOperation.h"

class FT81xDisplay;


/********************************************/
/*  FT81xGuiSlider Class                    */
/********************************************/  

class FT81xGuiSlider : public FT81xGuiComponent {
public:
  virtual ~FT81xGuiSlider();
  virtual void draw();
  void setColor(const uint8_t r, const uint8_t g, const uint8_t b) { m_color = COLOR_RGB(r, g, b); }
  void setSliderColor(const uint8_t r, const uint8_t g, const uint8_t b) { m_bgcolor = COLOR_RGB(r, g, b); }
  void setPosition(const uint16_t x, const uint16_t y) { m_x = x; m_y = y; }
  void setSize(const uint16_t w, const uint16_t h) { m_w = w; m_h = h; }
  void setRange(const uint16_t range) { m_range = range; }
  uint16_t readValue() { return m_value; }

protected:
  friend class FT81xDisplay;
  FT81xGuiSlider(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp);
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
  uint16_t m_range;
  uint16_t m_value;
};



#endif
