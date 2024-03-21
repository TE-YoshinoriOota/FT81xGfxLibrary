#include "FT81xGfxLine.h"

/********************************************/
/*  FT81xGfxLine Class                    */
/********************************************/

FT81xGfxLine::~FT81xGfxLine() {

}

FT81xGfxLine::FT81xGfxLine(RegisterOperation* reg, CommandOperation* cmd, FT81xDisplay *disp)
 : FT81xGuiComponent(reg, cmd, disp) {
  mType = FT81xLine;
  m_x1 = m_y1 = 0;
  m_x2 = m_y2 = 0;
  m_w = 0;
  m_color = 0;
}

void FT81xGfxLine::draw() {
  myCmd->cmd_start();
  myCmd->cmd(COLOR(m_color));
  myCmd->cmd(LINE_WIDTH(m_w*16));
  myCmd->cmd(BEGIN(LINES));
  myCmd->cmd(VERTEX2F(m_x1*16, m_y1*16));
  myCmd->cmd(VERTEX2F(m_x2*16, m_y2*16));
  myCmd->cmd(END());
  myCmd->cmd_end();
}

