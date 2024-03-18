#include "FT81GfxGauge.h"



/********************************************/
/*  FT81xGfxGauge Class                     */
/********************************************/

FT81xGfxGauge::~FT81xGfxGauge() { }

FT81xGfxGauge::FT81xGfxGauge(RegisterOperation* reg, CommandOperation* cmd, FT81xDisplay *disp)
 : FT81xGuiComponent(reg, cmd, disp) {
  mType = FT81xGuage;
  m_major = m_minor = m_range = 0;
  m_x = m_y = 0;
  m_value = 0;
  m_rad = 0;
  m_fgcolor = 0;
  m_bgcolor = 0;
  m_ncolor = 0;
  m_options = OPT_3D;
}

void FT81xGfxGauge::draw() {
  myCmd->cmd_start();
  myCmd->cmd(COLOR(m_fgcolor));
  myCmd->cmd(CMD_BGCOLOR);
  myCmd->cmd(m_bgcolor);
  myCmd->cmd(CMD_GAUGE);
  myCmd->cmd(m_x | ((uint32_t)(m_y) << 16));
  myCmd->cmd(m_rad | (uint32_t)(OPT_NOPOINTER) << 16);
  myCmd->cmd(m_major | ((uint32_t)(m_minor) << 16));
  myCmd->cmd(m_value | ((uint32_t)(m_range) << 16));
  myCmd->cmd_end();

  myCmd->cmd_start();
  myCmd->cmd(COLOR(m_ncolor));
  myCmd->cmd(CMD_BGCOLOR);
  myCmd->cmd(m_bgcolor);  
  myCmd->cmd(CMD_GAUGE);
  myCmd->cmd(m_x | (uint32_t)(m_y) << 16);
  myCmd->cmd(m_rad | (uint32_t)(OPT_NOBACK | OPT_NOTICKS) << 16);
  myCmd->cmd(m_major | (uint32_t)(m_minor) << 16);
  myCmd->cmd(m_value | (uint32_t)(m_range) << 16);
  myCmd->cmd_end();

}

void FT81xGfxGauge::draw(const uint16_t value) {
  m_value = value;
  draw();
}


