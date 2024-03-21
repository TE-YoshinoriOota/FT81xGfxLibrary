#ifndef FT81XINTERACTIONDIAL_HEADER_GUARD
#define FT81XINTERACTIONDIAL_HEADER_GUARD


#include "FT81xInteractionComponent.h"
#include "RegisterOperation.h"
#include "CommandOperation.h"

class FT81xDisplay;

/********************************************/
/*  FT81xInteractionDial Class            */
/********************************************/  

class FT81xInteractionDial : public FT81xInteractionComponent {
public:
  virtual ~FT81xInteractionDial();
  virtual void draw();
  void setColor(const uint8_t r, const uint8_t g, const uint8_t b) { m_color = COLOR_RGB(r, g, b); }
  void setDialColor(const uint8_t r, const uint8_t g, const uint8_t b) { m_bgcolor = COLOR_RGB(r, g, b); }
  void setPosition(const uint16_t x, const uint16_t y) { m_x = x; m_y = y; }
  void setSize(const uint16_t r) { m_rad = r; }
  uint16_t readValue() { return m_normValue; }

protected:
  friend class FT81xDisplay;
  FT81xInteractionDial(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp);
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



#endif