#include "FT81xGuiToggle.h"
#include "FT81xDisplay.h"


/********************************************/
/*  FT81xGuiToggle Class            */
/********************************************/  

FT81xGuiToggle::~FT81xGuiToggle() {
  if (m_text != NULL) {
    delete m_text;
    m_text = NULL;
  }
}

FT81xGuiToggle::FT81xGuiToggle(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp)
 : FT81xGuiComponent(reg, cmd, disp) {
  mType = FT81xToggle;
  m_x = m_y = 0;
  m_w = 0;
  m_font = 20;
  m_color = COLOR_RGB(255, 255, 255);
  m_bgcolor = COLOR_RGB(0, 0, 0);
  m_options = OPT_3D;
  m_state = 0;
  m_text = NULL;
}

void FT81xGuiToggle::setFont(const uint8_t font) {
  if (font > 31) {
    m_font = 31;
  } else {
    m_font = font;
  }
}

void FT81xGuiToggle::setText(const char text0[], const char text1[]) {
  static const char str_null[] = "Y\xffN";
  String text = String(text0) + String("\xff") + String(text1);
  uint16_t length = strlen(text.c_str());
  //Serial.println("m_textSize: " + String(m_textSize));
  //Serial.println(text);
  if (m_text != NULL) {
    free(m_text); m_text = NULL;
  }
  m_text = (char*)malloc(length+1);
  if (m_text == NULL) {
    Serial.println("text size is too big");
    m_text = (char*)str_null;
  } else {
    memset(m_text, NULL, length+1);
    memcpy(m_text, text.c_str(), length);
  } 
}


void FT81xGuiToggle::draw() {
  myCmd->cmd_start();
  myCmd->cmd(TAG_MASK(1));
  myCmd->cmd(TAG(mTag));
  myCmd->cmd(CMD_FGCOLOR);  
  myCmd->cmd(m_color);
  myCmd->cmd(CMD_BGCOLOR);
  myCmd->cmd(m_bgcolor);
  myCmd->cmd(CMD_TOGGLE);
  myCmd->cmd(m_x | ((uint32_t)(m_y) << 16));
  myCmd->cmd(m_w | ((uint32_t)(m_font) << 16));
  myCmd->cmd(m_options | ((uint32_t)(m_state) << 16));
  uint32_t strlen = _send_text_helper_(m_text, myCmd);
  myCmd->cmd(strlen);
  myCmd->cmd(TAG_MASK(0));
  myCmd->cmd_end();

  myCmd->cmd_start();
  myCmd->cmd(CMD_TRACK);
  myCmd->cmd(m_x | ((uint32_t)(m_y) << 16));
  myCmd->cmd(1 | ((uint32_t)(1) << 16));
  myCmd->cmd(mTag);
  myCmd->cmd_end();
}

void FT81xGuiToggle::doInteraction_(const uint32_t value) {
  static bool state = false;
  state = state ? false : true; 
  if (state) m_state = 0xffff;
  else m_state = 0x0000;
  Serial.println("Toggle::doInteruction_ is called");
  if (m_func != NULL) {
    m_func(state);
  }  
}

