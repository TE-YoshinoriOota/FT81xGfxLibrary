#include "FT81xGfxRectangle.h"


/********************************************/
/*  FT81xGfxRectangle Class                    */
/********************************************/

FT81xGfxRectangle::~FT81xGfxRectangle() {

}

FT81xGfxRectangle::FT81xGfxRectangle(RegisterOperation* reg, CommandOperation* cmd, FT81xDisplay *disp)
 : FT81xGfxComponent(reg, cmd, disp) {
  mType = FT81xRectangle;
  m_x = m_y = 0;
  m_w = m_h = 0;
  m_rad = 1;
  m_color = 0;
}

void FT81xGfxRectangle::draw() {
  myCmd->cmd_start();
  myCmd->cmd(COLOR(m_color));
  myCmd->cmd(LINE_WIDTH(m_rad*16));
  myCmd->cmd(BEGIN(RECTS));
  myCmd->cmd(VERTEX2F(m_x*16, m_y*16));
  myCmd->cmd(VERTEX2F((m_x + m_w)*16, (m_y + m_h)*16));
  myCmd->cmd(END());
  myCmd->cmd_end();
}

