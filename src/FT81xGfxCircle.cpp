#include "FT81xGfxCircle.h"


/********************************************/
/*  FT81xGfxCircle Class                    */
/********************************************/

FT81xGfxCircle::~FT81xGfxCircle() {

}

FT81xGfxCircle::FT81xGfxCircle(RegisterOperation* reg, CommandOperation* cmd, FT81xDisplay *disp)
 : FT81xGfxComponent(reg, cmd, disp) {
  mType = FT81xCircle;
  m_x = m_y = 0;
  m_r = 0;
  m_color = 0;
}

void FT81xGfxCircle::draw() {
  myCmd->cmd_start();
  myCmd->cmd(COLOR(m_color));
  myCmd->cmd(POINT_SIZE(m_r*16));
  myCmd->cmd(BEGIN(FTPOINTS));
  myCmd->cmd(VERTEX2F(m_x*16, m_y*16));
  myCmd->cmd(END());
  myCmd->cmd_end();
}

