#ifndef FT81XINTERACTIONBUTTON_HEADER_GUARD
#define FT81XINTERACTIONBUTTON_HEADER_GUARD

#include "FT81xGuiComponent.h"
#include "RegisterOperation.h"
#include "CommandOperation.h"

class FT81xDisplay;

/********************************************/
/*  FT81xGuiButton Class                    */
/********************************************/  

class FT81xGuiButton : public FT81xGuiComponent {
public:
  virtual ~FT81xGuiButton();
  virtual void draw();
  void setFont(const uint8_t font);
  void setColor(const uint8_t r, const uint8_t g, const uint8_t b) { m_color = COLOR_RGB(r, g, b); }
  void setButtonColor(const uint8_t r, const uint8_t g, const uint8_t b) { m_bgcolor = COLOR_RGB(r, g, b); }
  void setPosition(const uint16_t x, const uint16_t y) { m_x = x; m_y = y; }
  void setSize(const uint16_t w, const uint16_t h) { m_w = w; m_h = h; }
  void setText(const char text[]);
  bool pushed();

protected:
  friend class FT81xDisplay;
  FT81xGuiButton(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp);
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
  uint8_t  m_pushed;
  char *m_text;
};



#endif
