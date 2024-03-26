#include "FT81xGuiButton.h"
#include "FT81xDisplay.h"

/********************************************/
/*  FT81xGuiButton Class            */
/********************************************/  

FT81xGuiButton::~FT81xGuiButton() {
  if (m_text != NULL) {
    delete m_text;
    m_text = NULL;
  }
}

FT81xGuiButton::FT81xGuiButton(RegisterOperation* reg, CommandOperation* cmd, FT81xDisplay *disp)
 : FT81xGuiComponent(reg, cmd, disp) {
  mType = FT81xButton;
  m_x = m_y = 0;
  m_w = m_h = 0;
  m_font = 20;
  m_color = COLOR_RGB(255, 255, 255);
  m_bgcolor = 0;
  m_options = OPT_3D;
  m_pushed = 0;
  m_text = NULL;
}

void FT81xGuiButton::draw() {
  myCmd->cmd_start();

  myCmd->cmd(COLOR(m_color));
  myCmd->cmd(CMD_FGCOLOR);
  myCmd->cmd(m_bgcolor);
  
  myCmd->cmd(TAG_MASK(1));
  myCmd->cmd(TAG(mTag));  
  myCmd->cmd(CMD_BUTTON); 
  myCmd->cmd(m_x | ((uint32_t)(m_y) << 16));
  myCmd->cmd(m_w | ((uint32_t)(m_h) << 16));
  myCmd->cmd(m_font | ((uint32_t)(m_options) << 16));
  uint32_t strlen = _send_text_helper_(m_text, myCmd);
  myCmd->cmd(strlen);
  myCmd->cmd(TAG_MASK(0));

  myCmd->cmd_end();
}

void FT81xGuiButton::setFont(const uint8_t font) {
  if (font > 31) {
    m_font = 31;
  } else {
    m_font = font;
  }
}

void FT81xGuiButton::setText(const char text[]) {
  static const char str_null[] = "null";
  uint16_t length = strlen(text);
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
    memcpy(m_text, text, length);
  }
}

void FT81xGuiButton::doInteraction_(const uint32_t tag) {
  m_pushed = true;
  if (m_func != NULL) {
    m_func(tag);
  }
}

bool FT81xGuiButton::pushed() {
  bool value = m_pushed;
  m_pushed = false; // once the value is read, reset the value;
  return value; 
}
