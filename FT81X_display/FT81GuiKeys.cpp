#include "FT81GuiKey.h"


/********************************************/
/*  FT81xGuiKeys Class                      */
/********************************************/  

FT81xGuiKeys::~FT81xGuiKeys() {
  if (m_keymap != NULL) {
    uint16_t length = strlen(m_keymap);
    for (int n = 0; n < length; ++n) {
      this->releaseTag(m_keymap[n]);
    }    
    delete m_keymap;
    m_keymap = NULL;
  }
}

FT81xGuiKeys::FT81xGuiKeys(RegisterOperation* reg, CommandOperation* cmd, FT81xDisplay *disp)
 : FT81xInteractionComponent(reg, cmd, disp) {
  mType = FT81xKeys;
  m_x = m_y = 0;
  m_w = m_h = 0;
  m_font = 20;
  m_color = COLOR_RGB(255, 255, 255);
  m_bgcolor = 0;
  m_options = OPT_3D;
  m_key = 0;
  m_keymap = NULL;
  m_func = NULL;
}

void FT81xGuiKeys::draw() {
  myCmd->cmd_start();
  myCmd->cmd(COLOR(m_color));
  myCmd->cmd(CMD_FGCOLOR);
  myCmd->cmd(m_bgcolor);
  myCmd->cmd(CMD_KEYS);
  myCmd->cmd(m_x | ((uint32_t)(m_y) << 16));
  myCmd->cmd(m_w | ((uint32_t)(m_h) << 16));
  myCmd->cmd(m_font | ((uint32_t)(m_options) << 16));
  uint32_t length = _send_text_helper_(m_keymap, myCmd);
  myCmd->cmd(length);
  myCmd->cmd_end();
}

void FT81xGuiKeys::setFont(const uint8_t font) {
  if (font > 31) {
    m_font = 31;
  } else {
    m_font = font;
  }
}

void FT81xGuiKeys::setKeyMap(const char keymap[]) {
  static const char str_null[] = "-";
  uint16_t length = strlen(keymap) + 1;
  //Serial.println("m_textSize: " + String(m_textSize));
  //Serial.println(text);
  if (m_keymap != NULL) {
    free(m_keymap); m_keymap = NULL;
  }
  m_keymap = (char*)malloc(length);
  if (m_keymap == NULL) {
    Serial.println("keymap size is too big");
    m_keymap = (char*)str_null;
  } else {
    memset(m_keymap, NULL, length+1);
    memcpy(m_keymap, keymap, length);
    for (uint16_t n = 0; n < length; ++n) {
      this->setTag(m_keymap[n]);
    }
  }
}

void FT81xGuiKeys::doInteraction_(const uint32_t tag) {
  m_key = (char)tag;
  Serial.println("Keys::doInteruction_ is called");
  if (m_func != NULL) {
    m_func(tag);
  }
}

char FT81xGuiKeys::readKey() {
  char value = m_key;
  m_key = 0;
  return value;
}

