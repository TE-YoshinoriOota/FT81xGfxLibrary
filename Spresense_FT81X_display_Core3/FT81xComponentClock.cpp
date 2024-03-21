#include "FT81xComponentClock.h"


/********************************************/
/*  FT81xComponentClock Class               */
/********************************************/

FT81xComponentClock::~FT81xComponentClock() {

}

FT81xComponentClock::FT81xComponentClock(RegisterOperation* reg, CommandOperation* cmd, FT81xDisplay *disp)
 : FT81xGuiComponent(reg, cmd, disp) {
  mType = FT81xClock;
  m_hour = m_min = m_sec = 0;
  m_x = m_y = 0;
  m_rad = 0;
  m_fgcolor = 0;
  m_bgcolor = 0;
}

void FT81xComponentClock::draw() {
  myCmd->cmd_start();
  myCmd->cmd(COLOR(m_fgcolor));
  myCmd->cmd(CMD_BGCOLOR);
  myCmd->cmd(m_bgcolor);
  myCmd->cmd(CMD_CLOCK);
  myCmd->cmd(m_x | ((uint32_t)(m_y) << 16));
  myCmd->cmd(m_rad | ((uint32_t)(m_options) << 16));
  myCmd->cmd(m_hour | ((uint32_t)(m_min) << 16));
  myCmd->cmd(m_sec);
  myCmd->cmd_end();
}

void FT81xComponentClock::draw(const uint8_t hour, const uint8_t min, const uint8_t sec) {
  myCmd->cmd_start();
  myCmd->cmd(COLOR(m_fgcolor));
  myCmd->cmd(CMD_BGCOLOR);
  myCmd->cmd(m_bgcolor);
  myCmd->cmd(CMD_CLOCK);
  myCmd->cmd(m_x | ((uint32_t)(m_y) << 16));
  myCmd->cmd(m_rad | ((uint32_t)(m_options) << 16));
  myCmd->cmd(hour | ((uint32_t)(min) << 16));
  myCmd->cmd(sec);
  myCmd->cmd_end();
}

