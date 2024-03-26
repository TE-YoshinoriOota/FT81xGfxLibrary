#include "FT81xGuiDial.h"
#include "FT81xDisplay.h"

/********************************************/
/*  FT81xGuiDial Class                      */
/********************************************/  

FT81xGuiDial::~FT81xGuiDial() {
  
}

FT81xGuiDial::FT81xGuiDial(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp)
 : FT81xGuiComponent(reg, cmd, disp) {
  mType = FT81xDial;
  m_x = m_y = 0;
  m_rad = 0;
  m_color = COLOR_RGB(255, 255, 255);
  m_bgcolor = COLOR_RGB(0, 0, 0);
  m_options = OPT_3D;
  m_value = 0;
}

void FT81xGuiDial::draw() {
  myCmd->cmd_start();
  myCmd->cmd(TAG_MASK(1));
  myCmd->cmd(TAG(mTag));  
  myCmd->cmd(COLOR(m_color));
  myCmd->cmd(CMD_FGCOLOR);
  myCmd->cmd(m_bgcolor);
  myCmd->cmd(CMD_DIAL);
  myCmd->cmd(m_x | ((uint32_t)(m_y) << 16));
  myCmd->cmd(m_rad | ((uint32_t)(m_options) << 16));
  myCmd->cmd(m_value);
  myCmd->cmd(TAG_MASK(0));
  myCmd->cmd_end();

  myCmd->cmd_start();
  myCmd->cmd(CMD_TRACK);
  myCmd->cmd(m_x | ((uint32_t)(m_y) << 16));
  myCmd->cmd(1 | ((uint32_t)(1) << 16));
  myCmd->cmd(mTag);
  myCmd->cmd_end();
}


void FT81xGuiDial::doInteraction_(const uint32_t value) {
  m_value = value;
  m_normValue = (value*100)/(0xffff);
  Serial.println("Dial::doInteruction_ is called");
  if (m_func != NULL) {
    m_func(m_normValue);
  }
}

