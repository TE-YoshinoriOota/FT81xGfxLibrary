#include "FT81GfxText.h"


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
 : FT81xGuiComponent(reg, cmd, disp) {
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


inline uint32_t _send_text_helper_(const char text[], CommandOperation *cmd) {
  uint32_t data = 0xFFFFFFFF;
  for (uint8_t i = 0; (data >> 24) != 0; i += 4) {
    data = 0;
    if (text[i] != 0) {
      data |= text[i];
      if (text[i+1] != 0) {
        data |= text[i+1] << 8;
        if (text[i+2] != 0) {
          data |= (uint32_t)(text[i+2]) << 16;
          if (text[i+3] != 0) {
            data |= (uint32_t)(text[i+3]) << 24;
          }
        }
      }
    }
    if ((data >> 24) != 0) cmd->cmd(data);
    else  return data;
  }

  if ((data >> 24) != 0)  return 0;
  return data;  
}
