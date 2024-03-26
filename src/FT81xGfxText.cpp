#include "FT81xGfxText.h"

/********************************************/
/*  FT81xGfxText Class                    */
/********************************************/

FT81xGfxText::~FT81xGfxText() {
 if (m_text != NULL) {
  free(m_text);
  m_text = NULL;
 }
}

FT81xGfxText::FT81xGfxText(RegisterOperation* reg, CommandOperation* cmd, FT81xDisplay *disp)
 : FT81xGfxComponent(reg, cmd, disp) {
  mType = FT81xText;
  m_x = m_y = 0;
  m_font = 31;
  m_options = OPT_FLAT;
  m_color = 0;
  m_text = NULL;
}

void FT81xGfxText::setFont(const uint8_t font) {
  if (font > 31) {
    // myCmd->cmd_start();
    // myCmd->cmd(CMD_ROMFONT);
    // myCmd->cmd(14);
    // myCmd->cmd(font);
    // myCmd->cmd_end();
    m_font = 31;
  } else {
    m_font = font;
  }
}

void FT81xGfxText::setText(const char text[]) {
  static const char null[] = "null";
  uint16_t length = strlen(text);
  //Serial.println("m_textSize: " + String(m_textSize));
  //Serial.println(text);
  m_text = (char*)malloc(length+1);
  if (m_text == NULL) {
    Serial.println("text size is too big");
    m_text = (char*)null;
  }
  memset(m_text, NULL, length+1);
  memcpy(m_text, text, length);
}


void FT81xGfxText::draw() {
  myCmd->cmd_start();
  myCmd->cmd(COLOR(m_color));
  myCmd->cmd(CMD_TEXT);
  myCmd->cmd(m_x | ((uint32_t)(m_y) << 16));
  myCmd->cmd(m_font | ((uint32_t)(m_options) << 16));
  uint32_t rest_data = _send_text_helper_(m_text, myCmd);
  myCmd->cmd(rest_data);
  myCmd->cmd_end();
}

void FT81xGfxText::draw(const char text[]) {
  this->setText(text);
  this->draw();
}


