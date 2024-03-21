#ifndef FT81XGFXTEXT_HEADER_GUARD
#define FT81XGFXTEXT_HEADER_GUARD

#include "FT81xGuiComponent.h"
#include "RegisterOperation.h"
#include "CommandOperation.h"
//#include "FT81xDisplay.h"

class FT81xDisplay;


/********************************************/
/*  FT81xGfxText Class                    */
/********************************************/

class FT81xGfxText : public FT81xGuiComponent {
public:
  virtual ~FT81xGfxText();
  virtual void draw();
  void draw(const char text[]);  
  void setFont(const uint8_t font);
  void set3dEffect() { m_options = OPT_3D; }
  void setColor(const uint8_t r, const uint8_t g, const uint8_t b) { m_color = COLOR_RGB(r, g, b); }
  void setPosition(const uint16_t x, const uint16_t y) { m_x = x; m_y = y; }
  void setText(const char text[]);

protected:
  friend class FT81xDisplay;
  FT81xGfxText(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp);

private:
  uint8_t  m_font;
  uint16_t m_options;
  uint16_t m_x;
  uint16_t m_y;
  uint32_t m_color;
  char *m_text;
};




#endif