#include "FT81xInteractionSlider.h"
#include "FT81xDisplay.h"


/********************************************/
/*  FT81xInteractionSlider Class            */
/********************************************/  

FT81xInteractionSlider::~FT81xInteractionSlider() {

}

FT81xInteractionSlider::FT81xInteractionSlider(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp)
 : FT81xInteractionComponent(reg, cmd, disp) {
  m_x = m_y = 0;
  m_w = m_h = 0;
  m_color = COLOR_RGB(255, 255, 255);
  m_bgcolor = COLOR_RGB(0, 0, 0);
  m_options = OPT_3D;
  m_value = 0;
  m_range = 100;
}

void FT81xInteractionSlider::draw() {
  myCmd->cmd_start();
  myCmd->cmd(TAG_MASK(1));
  myCmd->cmd(TAG(mTag));  
  myCmd->cmd(CMD_FGCOLOR);
  myCmd->cmd(m_color);
  myCmd->cmd(CMD_BGCOLOR);
  myCmd->cmd(m_bgcolor);
  myCmd->cmd(CMD_SLIDER);
  myCmd->cmd(m_x | ((uint32_t)(m_y) << 16));
  myCmd->cmd(m_w | ((uint32_t)(m_h) << 16));
  myCmd->cmd(m_options | ((uint32_t)(m_value) << 16));
  myCmd->cmd(m_range);
  myCmd->cmd(TAG_MASK(0));
  myCmd->cmd_end();

  myCmd->cmd_start();
  myCmd->cmd(CMD_TRACK);
  myCmd->cmd(m_x | ((uint32_t)(m_y) << 16));
  myCmd->cmd(m_w | ((uint32_t)(m_h) << 16));
  myCmd->cmd(mTag);
  myCmd->cmd_end();
}

void  FT81xInteractionSlider::doInteraction_(const uint32_t value) {
  m_value = (uint16_t)(value*m_range/0xffff);
  Serial.println("Slider::doInteruction_ is called");
  if (m_func != NULL) {
    m_func(m_value);
  }
}
