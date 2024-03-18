#include "FT81GfxGradient.h"


/********************************************/
/*  FT81xGfxGradient Class                  */
/********************************************/

FT81xGfxGradient::~FT81xGfxGradient() { }

FT81xGfxGradient::FT81xGfxGradient(RegisterOperation* reg, CommandOperation* cmd, FT81xDisplay *disp)
 : FT81xGuiComponent(reg, cmd, disp) {
  mType = FT81xGradient;
  m_x = m_y = 0;
  m_w = m_h = 0;
  m_color1 = 0;
  m_color2 = 0;
}

void FT81xGfxGradient::draw() {
  myCmd->cmd_start();
  myCmd->cmd(SCISSOR_XY(m_x, m_y));
  myCmd->cmd(SCISSOR_SIZE(m_w, m_h));
  myCmd->cmd(CMD_GRADIENT);
  myCmd->cmd(m_x | ((uint32_t)(m_y) << 16));
  myCmd->cmd(m_color1);
  myCmd->cmd((m_x + m_w) | ((uint32_t)(m_y + m_h) << 16));
  myCmd->cmd(m_color2);
  myCmd->cmd(SCISSOR_XY(0, 0));
  uint16_t w = myDisp->getDisplayWidth();
  uint16_t h = myDisp->getDisplayHeight();
  myCmd->cmd(SCISSOR_SIZE(w, h));  
  myCmd->cmd_end();
}

