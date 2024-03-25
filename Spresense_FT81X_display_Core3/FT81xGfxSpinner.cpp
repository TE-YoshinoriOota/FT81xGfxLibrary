#include "FT81xGfxSpinner.h"


/********************************************/
/*  FT81xGfxSpinner Class                   */
/********************************************/

FT81xGfxSpinner::~FT81xGfxSpinner() {

}

FT81xGfxSpinner::FT81xGfxSpinner(RegisterOperation* reg, CommandOperation* cmd, FT81xDisplay *disp)
 : FT81xGfxComponent(reg, cmd, disp) {
  mType = FT81xSpinner;
  m_x = m_y = 0;
  m_w = m_h = 0;
  m_options = OPT_3D;
  m_style = dot_circle;
  m_scale = 0;
  m_color = 0;
}

void FT81xGfxSpinner::draw() {
  myCmd->cmd_start();
  myCmd->cmd(COLOR(m_color));  
  myCmd->cmd(CMD_SPINNER);
  myCmd->cmd(m_x | ((uint32_t)(m_y) << 16));
  myCmd->cmd(m_style | ((uint32_t)(m_scale) << 16));
  myCmd->cmd_end();
}
