#ifndef FT81XINTERACTIONKEYS_HEADER_GUARD
#define FT81XINTERACTIONKEYS_HEADER_GUARD

#include "FT81xInteractionComponent.h"
#include "RegisterOperation.h"
#include "CommandOperation.h"

class FT81xDisplay;


/********************************************/
/*  FT81xInteractionKeys   Class            */
/********************************************/  

class FT81xInteractionKeys : public FT81xInteractionComponent {
public:
  virtual ~FT81xInteractionKeys();
  virtual void draw();
  void setFont(const uint8_t font);
  void setColor(const uint8_t r, const uint8_t g, const uint8_t b) { m_color = COLOR_RGB(r, g, b); }
  void setKeysColor(const uint8_t r, const uint8_t g, const uint8_t b) { m_bgcolor = COLOR_RGB(r, g, b); }
  void setPosition(const uint16_t x, const uint16_t y) { m_x = x; m_y = y; }
  void setSize(const uint16_t w, const uint16_t h) { m_w = w; m_h = h; }
  void setKeyMap(const char keymap[]);
  char readKey();

protected:
  friend class FT81xDisplay;
  FT81xInteractionKeys(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp);
  virtual void doInteraction_(const uint32_t value);
  virtual bool isTracker() { return false; }

private:
  uint16_t m_x;
  uint16_t m_y;
  uint16_t m_w;
  uint16_t m_h;
  uint8_t  m_font;
  uint32_t m_color;
  uint32_t m_bgcolor;
  uint16_t m_options;
  char  m_key;
  char *m_keymap;
};



#endif