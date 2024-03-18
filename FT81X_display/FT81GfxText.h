#ifndef FT81GFXTEXT_HEADER_GUARD
#define FT81GFXTEXT_HEADER_GUARD


#include "FT81GfxComponent.h"
#include "FT81RegisterOperation.h"
#include "FT81CommandOperation.h"
#include "FT81Display.h"


/********************************************/
/*  FT81xGfxText Class                    */
/********************************************/

class FT81xGfxText : public FT81xGfxComponent {
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

inline uint32_t _send_text_helper_(const char text[], CommandOperation *cmd);

#endif // FT81GFXTEXT_HEADER_GUARD
