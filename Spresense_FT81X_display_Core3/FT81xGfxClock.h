#ifndef FT81XCOMPOENENTCLOCK_HEADER_GUARD
#define FT81XCOMPOENENTCLOCK_HEADER_GUARD

#include "FT81xGfxComponent.h"
#include "RegisterOperation.h"
#include "CommandOperation.h"

class FT81xDisplay;

/********************************************/
/*  FT81xGfxClock Class                     */
/********************************************/

class FT81xGfxClock : public FT81xGfxComponent {
public:
  virtual ~FT81xGfxClock();
  virtual void draw();
  void draw(const uint8_t hour, const uint8_t min, const uint8_t sec);
  void setColor(const uint8_t r, const uint8_t g, const uint8_t b) { m_fgcolor = COLOR_RGB(r, g, b); }
  void setBgColor(const uint8_t r, const uint8_t g, const uint8_t b) { m_bgcolor = COLOR_RGB(r, g, b); }
  void setPosition(const uint16_t x, const uint16_t y) { m_x = x; m_y = y; }
  void setSize(const uint16_t r) { m_rad = r; };
  void setTime(const uint8_t hour, const uint8_t min, const uint8_t sec) { m_hour = hour; m_min = min; m_sec = sec; }
  void setNo3dEffect() { m_options |= OPT_FLAT; }
  void setNoSeconds() { m_options |= OPT_NOSECS; }
  void setNoBackground() { m_options |= OPT_NOSECS; }
  void setNoTicks() { m_options |= OPT_NOTICKS; }
  void setNoHands() { m_options |= OPT_NOHANDS; }

protected:
  friend class FT81xDisplay;
  FT81xGfxClock(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp);

private:
  uint16_t m_x;
  uint16_t m_y;
  uint16_t m_rad;
  uint8_t  m_hour;
  uint8_t  m_min;
  uint8_t  m_sec;
  uint32_t m_fgcolor;
  uint32_t m_bgcolor;
  uint16_t m_options;
};


#endif
