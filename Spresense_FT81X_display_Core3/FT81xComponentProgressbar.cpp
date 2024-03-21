#include "FT81xComponentProgressbar.h"

/********************************************/
/*  FT81xComponentProgressbar Class               */
/********************************************/

FT81xComponentProgressbar::~FT81xComponentProgressbar() {

}

FT81xComponentProgressbar::FT81xComponentProgressbar(RegisterOperation* reg, CommandOperation* cmd, FT81xDisplay *disp)
 : FT81xGuiComponent(reg, cmd, disp) {
  mType = FT81xProgressbar;
  m_range = 0;
  m_x = m_y = 0;
  m_value = 0;
  m_fgcolor = 0;
  m_bgcolor = 0;
  m_options = OPT_3D;
}

void FT81xComponentProgressbar::draw() {
  myCmd->cmd_start();
  myCmd->cmd(COLOR(m_fgcolor));
  myCmd->cmd(CMD_BGCOLOR);
  myCmd->cmd(m_bgcolor);
  myCmd->cmd(CMD_PROGRESS);
  myCmd->cmd(m_x  | ((uint32_t)(m_y) << 16));
  myCmd->cmd(m_w | ((uint32_t)(m_h) << 16));
  myCmd->cmd(m_options | ((uint32_t)(m_value) << 16));
  myCmd->cmd(m_range);
  myCmd->cmd_end();
}

void FT81xComponentProgressbar::draw(const uint16_t value) {
  m_value = value;
  draw();
}

