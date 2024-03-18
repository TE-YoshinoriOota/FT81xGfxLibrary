#include <SPI.h>
#include <SDHCI.h>
#include <Flash.h>
#include "FT81Core.h"

SDClass SD;



SPISettings CmdSPISettings(20000000, MSBFIRST, SPI_MODE0);
/*
uint8_t rd8(const uint32_t addr) {
  uint8_t value;
  digitalWrite(CS_PIN, LOW);
  SPI.transfer(addr >> 16);
  SPI.transfer(highByte(addr));
  SPI.transfer(lowByte(addr));
  SPI.transfer(0); //Dummy Read Byte
  value = SPI.transfer(0);
  digitalWrite(CS_PIN, HIGH);
  return value;
}

uint16_t rd16(const uint32_t addr) {
  uint16_t value;
  uint16_t byte0;
  uint16_t byte1;
  
  digitalWrite(CS_PIN, LOW);
  SPI.transfer(addr >> 16);
  SPI.transfer(highByte(addr));
  SPI.transfer(lowByte(addr));
  SPI.transfer(0); //Dummy Read Byte
  byte0 = SPI.transfer(0);
  byte1 = SPI.transfer(0);
  value = (byte1 << 8) | byte0;
  digitalWrite(CS_PIN, HIGH);
  return value;
}

uint32_t rd32(const uint32_t addr) {
  uint32_t value;
  uint32_t byte0;
  uint32_t byte1;
  uint32_t byte2;
  uint32_t byte3;

  digitalWrite(CS_PIN, LOW);
  SPI.transfer(addr >> 16);
  SPI.transfer(highByte(addr));
  SPI.transfer(lowByte(addr));
  SPI.transfer(0); //Dummy Read Byte
  byte0 = SPI.transfer(0);
  byte1 = SPI.transfer(0);
  byte2 = SPI.transfer(0);
  byte3 = SPI.transfer(0);
  value = (byte3 << 24) | (byte2 << 16) | (byte1 << 8) | byte0;
  digitalWrite(CS_PIN, HIGH);
  return value;
}

void wr8(const uint32_t addr, const uint8_t value) {
  digitalWrite(CS_PIN, LOW);
  SPI.transfer(0x80 | (addr >> 16));
  SPI.transfer(highByte(addr));
  SPI.transfer(lowByte(addr));
  SPI.transfer(value);
  digitalWrite(CS_PIN, HIGH);
}

void wr16(const uint32_t addr, const uint16_t value) {
  digitalWrite(CS_PIN, LOW);
  SPI.transfer(0x80 | (addr >> 16));
  SPI.transfer(highByte(addr));
  SPI.transfer(lowByte(addr));
  SPI.transfer(value & 0xFF);//LSB first
  SPI.transfer((value >> 8) & 0xFF);
  digitalWrite(CS_PIN, HIGH);
}

void wr32(const uint32_t addr, const uint32_t value) {
  digitalWrite(CS_PIN, LOW);
  SPI.transfer(0x80 | (addr >> 16));
  SPI.transfer(highByte(addr));
  SPI.transfer(lowByte(addr));
  SPI.transfer(value & 0xFF);//LSB first
  SPI.transfer((value >>  8) & 0xFF);  // 2Byte
  SPI.transfer((value >> 16) & 0xFF);  // 3Byte
  SPI.transfer((value >> 24) & 0xFF);  // 4Byte
  digitalWrite(CS_PIN, HIGH);
}

void host_command(const uint8_t cmd) {
  digitalWrite(CS_PIN, LOW);
  SPI.transfer(cmd);
  SPI.transfer(0);
  SPI.transfer(0);
  digitalWrite(CS_PIN, HIGH);
}


static uint32_t cmd_write_address = 0;

inline void incCmdWriteAddress(uint16_t delta) { 
  cmd_write_address = (cmd_write_address + delta) % 4096; 
  cmd_write_address = (cmd_write_address + 3) & 0xffc; // 4 byte alignment
}

inline void updateCmdWriteAddress() { 
  wr16(REG_CMD_WRITE, cmd_write_address); 
}

void cmd_start() {
  waitForCommandFiFoEmpty();
  cmd_write_address = rd16(REG_CMD_WRITE);
  uint32_t addr = (RAM_CMD + cmd_write_address) | WRITE;
  SPI.beginTransaction(CmdSPISettings);
  digitalWrite(CS_PIN, LOW);

  SPI.transfer(addr >> 16);
  SPI.transfer(addr >> 8);
  SPI.transfer(addr);
}

void cmd(const uint32_t cmd) {
  SPI.transfer(cmd);
  SPI.transfer(cmd >> 8);
  SPI.transfer(cmd >> 16);
  SPI.transfer(cmd >> 24);
  incCmdWriteAddress(4);
}

void cmd_end() {
  digitalWrite(CS_PIN, HIGH);
  SPI.endTransaction();
  updateCmdWriteAddress();
}


void waitForCommandFiFoEmpty() {
  static uint32_t last_cmd_write_address = cmd_write_address;
  while(rd16(REG_CMD_READ) != rd16(REG_CMD_WRITE)){
    #if 0
    cmd_write_address = rd16(REG_CMD_WRITE);
    if (cmd_write_address != last_cmd_write_address) {
      Serial.println("Wait for command fifo empty");
      last_cmd_write_address = cmd_write_address;
    }
    #else
      __asm__ volatile("nop");
    #endif
  };
}

void checkCmdFiFoBuffer(const uint32_t count) {
  uint16_t getfreespace;
  do {
    getfreespace = checkCmdFiFoFreespace();
  } while (getfreespace < count);  
}

uint16_t checkCmdFiFoFreespace() {
  uint16_t fullness = 0;
  uint16_t retval = 0;
  fullness = (cmd_write_address - rd16(REG_CMD_READ)) & 4095;
  retval = (FT_CMD_FIFO_SIZE - 4) - fullness;
  return retval;
}

void waitCmdFiFoFreespace(uint32_t need_freespace) {
  uint16_t freespace;
  do {
    freespace = checkCmdFiFoFreespace(); 
  } while (freespace < need_freespace);
}


// application api

void begin_display_list() {
  cmd_start();
  cmd(CMD_DLSTART);
  cmd(CLEAR(1, 1, 1));
  cmd_end();
}

void clear(const uint32_t color) {
  cmd_start();
  cmd(CLEAR_COLOR(color));
  cmd(CLEAR(1, 1, 1));
  cmd_end();
}


void draw_circle(const int16_t x, const int16_t y, const uint8_t size, const uint32_t color) {
  cmd_start();
  cmd(COLOR(color));
  cmd(POINT_SIZE(size * 16));
  cmd(BEGIN(FTPOINTS));
  cmd(VERTEX2F(x * 16, y * 16));
  cmd(END());
  cmd_end();
}

void draw_rect(const int16_t x, const int16_t y, const uint16_t width, const uint16_t height, const uint8_t cornerRadius, const uint32_t color) {
  cmd_start();
  cmd(COLOR(color));
  cmd(LINE_WIDTH(cornerRadius * 16));
  cmd(BEGIN(RECTS));
  cmd(VERTEX2F(x * 16, y * 16));
  cmd(VERTEX2F((x + width) * 16, (y + height) * 16));
  cmd(END());
  cmd_end();
}

void draw_triangle(const int16_t x1, const int16_t y1, const int16_t x2, const int16_t y2, const int16_t x3, const int16_t y3, const uint32_t color, const uint32_t bgcolor) {
  int16_t x_1;
  int16_t y_1;
  int16_t x_2;  // point 2 is highest (smallest y)
  int16_t y_2;  // point 2 is highest (smallest y)
  int16_t x_3;
  int16_t y_3;

  if ((y1 <= y2) && (y1 <= y3)) {  // point 1 is highest on screen
    x_1 = x3;
    y_1 = y3;
    x_2 = x1;
    y_2 = y1;
    x_3 = x2;
    y_3 = y2;
  } else if ((y2 <= y3) && (y2 <= y1)) {  // point 2 is highest
    x_1 = x1;
    y_1 = y1;
    x_2 = x2;
    y_2 = y2;
    x_3 = x3;
    y_3 = y3;
  } else {  // point 3 highest
    x_1 = x2;
    y_1 = y2;
    x_2 = x3;
    y_2 = y3;
    x_3 = x1;
    y_3 = y1;
  }

  if (x_2 <= x_1) {  // one colour wipe (2-3), two bg wipes
    cmd_start();
    cmd(COLOR(color));
    cmd(LINE_WIDTH(1 * 16));
    cmd(BEGIN(EDGE_STRIP_B));
    cmd(VERTEX2F(x_2 * 16, y_2 * 16));
    cmd(VERTEX2F(x_3 * 16, y_3 * 16));
    cmd(COLOR(bgcolor));
    cmd(LINE_WIDTH(1 * 16));
    cmd(BEGIN(EDGE_STRIP_B));
    cmd(VERTEX2F(x_3 * 16, y_3 * 16));
    cmd(VERTEX2F(x_1 * 16, y_1 * 16));
    cmd(VERTEX2F(x_2 * 16, y_2 * 16));
  } else if (x_2 >= x_3) {  // one colour wipe (1-2), two bg wipes
    cmd_start();
    cmd(COLOR(color));
    cmd(LINE_WIDTH(1 * 16));
    cmd(BEGIN(EDGE_STRIP_B));
    cmd(VERTEX2F(x_1 * 16, y_1 * 16));
    cmd(VERTEX2F(x_2 * 16, y_2 * 16));
    cmd(COLOR(bgcolor));
    cmd(LINE_WIDTH(1 * 16));
    cmd(BEGIN(EDGE_STRIP_B));
    cmd(VERTEX2F(x_2 * 16, y_2 * 16));
    cmd(VERTEX2F(x_3 * 16, y_3 * 16));
    cmd(VERTEX2F(x_1 * 16, y_1 * 16));
  } else {  // two colour wipes, one bg wipe
    cmd_start();
    cmd(COLOR(color));
    cmd(LINE_WIDTH(1 * 16));
    cmd(BEGIN(EDGE_STRIP_B));
    cmd(VERTEX2F(x_1 * 16, y_1 * 16));
    cmd(VERTEX2F(x_2 * 16, y_2 * 16));
    cmd(VERTEX2F(x_3 * 16, y_3 * 16));
    cmd(COLOR(bgcolor));
    cmd(LINE_WIDTH(1 * 16));
    cmd(BEGIN(EDGE_STRIP_B));
    cmd(VERTEX2F(x_3 * 16, y_3 * 16));
    cmd(VERTEX2F(x_1 * 16, y_1 * 16));
  }
  cmd(END());
  cmd_end();
}

void draw_line(const int16_t x1, const int16_t y1, const int16_t x2, const int16_t y2, const uint8_t width, const uint32_t color) {
  cmd_start();
  cmd(COLOR(color));
  cmd(LINE_WIDTH(width * 16));
  cmd(BEGIN(LINES));
  cmd(VERTEX2F(x1 * 16, y1 * 16));
  cmd(VERTEX2F(x2 * 16, y2 * 16));
  cmd(END());
  cmd_end();
}

void draw_gradient(const int16_t x1, const int16_t y1, const uint32_t color1, const int16_t x2, const int16_t y2, const uint32_t color2) {
  cmd_start();
  cmd(CMD_GRADIENT);
  cmd(x1 | ((uint32_t)(y1) << 16));
  cmd(color1);
  cmd(x2 | ((uint32_t)(y2) << 16));
  cmd(color2);
  cmd_end();
}

void draw_clock(const int16_t x, const int16_t y, const int16_t radius, const uint32_t handsColor, const uint32_t backgroundColor, const uint16_t options, const uint16_t hours, const uint16_t minutes, const uint16_t seconds) {
  cmd_start();
  cmd(COLOR(handsColor));
  cmd(CMD_BGCOLOR);
  cmd(backgroundColor);
  cmd(CMD_CLOCK);
  cmd(x | ((uint32_t)(y) << 16));
  cmd(radius | ((uint32_t)(options) << 16));
  cmd(hours | ((uint32_t)(minutes) << 16));
  cmd(seconds);
  cmd_end();
}

uint32_t send_text(const char text[]) {
  uint32_t data = 0xFFFFFFFF;
  for (uint8_t i = 0; (data >> 24) != 0; i += 4) {
    data = 0;
    if (text[i] != 0) {
      data |= text[i];
      if (text[i + 1] != 0) {
        data |= text[i + 1] << 8;
        if (text[i + 2] != 0) {
          data |= (uint32_t)text[i + 2] << 16;
          if (text[i + 3] != 0) {
            data |= (uint32_t)text[i + 3] << 24;
          }
        }
      }
    }

    if ((data >> 24) != 0)  cmd(data);
    else                 return data;
  }

  if ((data >> 24) != 0) {
    return 0;
  }

  return data;
}

uint8_t initBitmapHandleForFont(uint8_t font) {
  if (font > 31) {
    cmd_start();
    cmd(CMD_ROMFONT);
    cmd(14);
    cmd(font);
    cmd_end();
    return 14;
  }
  return font;
}


void draw_text(const int16_t x, const int16_t y, const uint8_t font, const uint32_t color, const uint16_t options, const char text[]) {
  uint8_t fontHandle = initBitmapHandleForFont(font);
  cmd_start();
  cmd(COLOR(color));
  // set cordinate
 #if 0
  cmd(CMD_LOADIDENTITY);
  cmd(CMD_SCALE);
  cmd(1 * 65536);
  cmd(1 * 65536);
  cmd(CMD_SETMATRIX);
#endif
  cmd(CMD_TEXT);
  cmd(x | ((uint32_t)y << 16));
  cmd(fontHandle | ((uint32_t)options << 16));
  uint32_t rest_data = send_text(text);
  cmd(rest_data);
  cmd_end();
}

enum spinner_style {
   dot_circle
  ,dot_line
  ,rot_clock
  ,two_dots
};

void draw_spinner(const int16_t x, const int16_t y, const uint16_t style, const uint16_t scale, const uint32_t color) {
  cmd_start();
  cmd(COLOR(color));
  cmd(CMD_SPINNER);
  cmd(x | ((uint32_t)y << 16));
  cmd(style | ((uint32_t)scale << 16));
  cmd_end();
}

void draw_gauge(const int16_t x, const int16_t y, const int16_t radius, const uint32_t handsColor, const uint32_t backgroundColor, const uint16_t options, const uint8_t major, const uint8_t minor, const uint16_t value, const uint16_t range) {
  cmd_start();
  cmd(COLOR(handsColor));
  cmd(CMD_BGCOLOR);
  cmd(backgroundColor);
  cmd(CMD_GAUGE);
  cmd(  x    | ((uint32_t)(   y    << 16)));
  cmd(radius | ((uint32_t)(options << 16)));
  cmd( major | ((uint32_t)( minor  << 16)));
  cmd( value | ((uint32_t)( range  << 16)));
  cmd_end();
}

void draw_progressbar(const int16_t x, const int16_t y, const int16_t width, const int16_t height, const uint32_t foregroundColor, const uint32_t backgroundColor, const uint16_t options, const uint16_t value, const uint16_t range) {
  cmd_start();
  cmd(COLOR(foregroundColor));
  cmd(CMD_BGCOLOR);
  cmd(backgroundColor);
  cmd(CMD_PROGRESS);
  cmd(   x    | ((uint32_t)(   y   << 16)));
  cmd( width  | ((uint32_t)(height << 16)));
  cmd(options | ((uint32_t)( value << 16)));
  cmd(range);
  cmd_end();
}


const uint32_t media_fifo_size = 0x40000UL;
const uint32_t media_fifo_base = RAM_G + 0x100000 - media_fifo_size;
uint32_t store_jpeg(uint32_t addr, uint8_t img_buf[], const uint32_t img_size) {
  waitForCommandFiFoEmpty();

  // Setup Media FIFO
  cmd_start();
  cmd(CMD_MEDIAFIFO);
  cmd(media_fifo_base);
  cmd(media_fifo_size);
  cmd_end();

  // Load Image
  cmd_start();
  cmd(CMD_LOADIMAGE);
  cmd(RAM_G + addr);
  cmd(OPT_NODL | OPT_MEDIAFIFO);
  cmd_end();

  // Transfer Image data to RAM_G
  const int32_t chunk = 512;
  int32_t length = (int32_t)(img_size);
  uint32_t total_transfer_size = 0;
  uint32_t media_write_address = rd32(REG_MEDIAFIFO_WRITE);
  // Serial.println("media_write_address: " + String(media_write_address));

  uint8_t *buf = img_buf;
  while (length > 0) {
    uint16_t trans_size = min(chunk, length);
    trans_size = (trans_size + 3) & ~3; // force 32-bit alignment

    ma
    uint32_t address = (media_fifo_base + media_write_address) | WRITE;

    SPI.beginTransaction(CmdSPISettings);
    digitalWrite(CS_PIN, LOW);  
    SPI.transfer((address >> 16) & 0xFF);
    SPI.transfer((address >> 8) & 0xFF);
    SPI.transfer(address & 0xFF);
    SPI.send(buf, trans_size);
    digitalWrite(CS_PIN, HIGH);
    SPI.endTransaction();

    buf += trans_size;
    length -= (int32_t)trans_size;

    total_transfer_size += trans_size;
    wr32(REG_MEDIAFIFO_WRITE, total_transfer_size);
  }
  
  // Serial.println("img_size      : " + String(img_size));
  // Serial.println("transfer size : " + String(total_transfer_size));
  return total_transfer_size;
}



bool play_mjpeg(File& aviFile) {
  waitForCommandFiFoEmpty();

  // Setup Media FIFO
  cmd_start();
  cmd(CMD_MEDIAFIFO);
  cmd(media_fifo_base);
  cmd(media_fifo_size);
  cmd_end();

  // play the video
  cmd_start();
  cmd(CMD_PLAYVIDEO);
  cmd(OPT_MEDIAFIFO | OPT_FULLSCREEN | OPT_SOUND);
  cmd_end();    

  // stream buf for AVI data
  static const int32_t chunk = 512;
  static uint8_t buf[chunk];
  bool video_end = false;


  do {

    uint32_t media_write_address = rd32(REG_MEDIAFIFO_WRITE);
    usleep(1);
    uint32_t media_read_address  = rd32(REG_MEDIAFIFO_READ);
    uint32_t fullness = (media_write_address - media_read_address) & (media_fifo_size - 1);
    Serial.println("media_write_address: " + String(media_write_address));
    Serial.println("media_read_address : " + String(media_read_address));
    Serial.println("fullness           : " + String(fullness));

    while (fullness < media_fifo_size - chunk) {

      // write a chunk of AVI data to the media fifo
      if (aviFile.read(buf, chunk) < 0) {
        Serial.println("End of AVI file");
        video_end = true;
        break;
      };
      uint32_t cmd = (media_fifo_base + media_write_address) | WRITE;

      SPI.beginTransaction(CmdSPISettings);
      digitalWrite(CS_PIN, LOW);  
      SPI.transfer((cmd >> 16) & 0xFF);
      SPI.transfer((cmd >> 8) & 0xFF);
      SPI.transfer(cmd & 0xFF);
      SPI.send(buf, chunk);
      digitalWrite(CS_PIN, HIGH);
      SPI.endTransaction();

      // note that the media fifo is a ring buffer
      media_write_address = (media_write_address + chunk) & (media_fifo_size - 1);
      fullness += chunk;
      wr32(REG_MEDIAFIFO_WRITE, media_write_address);

    } 
    usleep(50000);
  } while (!video_end);
  
  // Serial.println("img_size      : " + String(img_size));
  // Serial.println("transfer size : " + String(total_transfer_size));
  return true;
}

struct button_prop {
  uint8_t          tag;
  uint16_t           x;
  uint16_t           y;
  uint16_t           w;
  uint16_t           h;
  uint8_t         font;
  uint32_t   textColor;
  uint32_t buttonColor;
  uint16_t     options;
  char           *text;  
};

void draw_button(struct button_prop *button) {
  uint8_t fontHandle = initBitmapHandleForFont(button->font);
  cmd_start();
  cmd(COLOR(button->textColor));
  cmd(CMD_FGCOLOR);
  cmd(button->buttonColor);
  cmd_end();

  cmd_start();
  cmd(TAG_MASK(1));
  cmd_end();

  uint32_t tag = button->tag;
  uint32_t x   = button->x;
  uint32_t y   = button->y;
  uint32_t w   = button->w;
  uint32_t h   = button->h;
  cmd_start();
  cmd(TAG(tag));  
  cmd_end();

  cmd_start();
  cmd(CMD_BUTTON); 
  cmd(((y << 16)) | (x & 0x0000ffff));
  cmd(((h << 16)) | (w & 0x0000ffff));
  cmd(fontHandle | ((uint32_t)(button->options) << 16));
  uint32_t strlen = send_text(button->text);
  cmd(strlen);
  cmd_end();

  cmd_start();
  cmd(TAG_MASK(0));
  cmd_end();
}

uint8_t read_tag() {
  return rd8(REG_TOUCH_TAG);
}


const char prop_file[] = "eve_prop.txt";

bool is_calibrated() {
  return Flash.exists(prop_file);
}

void force_calibration() {
  if (Flash.exists(prop_file)) {
    Flash.remove(prop_file);
  }
}

void touch_calibration() {
  cmd_start();
  cmd(CMD_CALIBRATE);
  cmd(0);
  cmd_end();
  waitForCommandFiFoEmpty();

  //prop.config = rd32(REG_TOUCH_CONFIG);
  uint32_t A = rd32(REG_TOUCH_TRANSFORM_A);
  uint32_t B = rd32(REG_TOUCH_TRANSFORM_B);
  uint32_t C = rd32(REG_TOUCH_TRANSFORM_C);
  uint32_t D = rd32(REG_TOUCH_TRANSFORM_D);
  uint32_t E = rd32(REG_TOUCH_TRANSFORM_E);
  uint32_t F = rd32(REG_TOUCH_TRANSFORM_F);

  if (Flash.exists(prop_file)) {
    Serial.println("Error: property file already exists");
    Flash.remove(prop_file); 
  }
  File myFile = Flash.open("eve_prop.txt", FILE_WRITE);
  //myFile.println(String(prop.config)):
  myFile.println(String(A));
  myFile.println(String(B));
  myFile.println(String(C));
  myFile.println(String(D));
  myFile.println(String(E));
  myFile.println(String(F));
  myFile.close();
}

void restore_touch_calibration() {
  File myFile = Flash.open(prop_file);
  //String prop_calib = myFile.readStringUntil('\n');
  String a = myFile.readStringUntil('\n');
  String b = myFile.readStringUntil('\n');
  String c = myFile.readStringUntil('\n');
  String d = myFile.readStringUntil('\n');
  String e = myFile.readStringUntil('\n');
  String f = myFile.readStringUntil('\n');

  //uint32_t config = prop_calib.toInt();
  uint32_t A = strtoul(a.c_str(), NULL, 10);
  uint32_t B = strtoul(b.c_str(), NULL, 10);
  uint32_t C = strtoul(c.c_str(), NULL, 10);
  uint32_t D = strtoul(d.c_str(), NULL, 10);
  uint32_t E = strtoul(e.c_str(), NULL, 10);
  uint32_t F = strtoul(f.c_str(), NULL, 10);

  //wr32(REG_TOUCH_CONFIG, config);
  wr32(REG_TOUCH_TRANSFORM_A, A);
  wr32(REG_TOUCH_TRANSFORM_B, B);
  wr32(REG_TOUCH_TRANSFORM_C, C);
  wr32(REG_TOUCH_TRANSFORM_D, D);
  wr32(REG_TOUCH_TRANSFORM_E, E);
  wr32(REG_TOUCH_TRANSFORM_F, F);
}

bool is_touch(uint16_t *x, uint16_t *y) {
  bool     ret = !(rd16(REG_TOUCH_RAW_XY) & 0x8000);
  uint32_t xy  = rd32(REG_TOUCH_SCREEN_XY);
  if (ret) {
    *x =  xy & 0x0000ffff;
    *y = (xy >> 16) & 0x0000ffff;
  }
  return ret;
}

struct scrollbar_prop {
  uint8_t              tag;
  uint16_t               x;
  uint16_t               y;
  uint16_t               w;
  uint16_t               h;
  uint32_t foregroundColor;
  uint32_t backgroundColor;
  uint16_t         options;
  uint16_t           value;
  uint16_t            size;
  uint16_t           range;
};


void draw_scrollbar(struct scrollbar_prop *prop) {
  uint32_t tag = prop->tag;
  uint32_t   x = prop->x;
  uint32_t   y = prop->y;
  uint32_t   w = prop->w;
  uint32_t   h = prop->h;
  uint32_t  fg = prop->foregroundColor;
  uint32_t  bg = prop->backgroundColor;
  uint32_t  op = prop->options;
  uint32_t val = prop->value;
  uint32_t  sz = prop->size;
  uint32_t rng = prop->range;

  cmd_start();
  cmd(TAG_MASK(1));
  cmd_end();

  cmd_start();
  cmd(TAG(tag));  
  cmd_end();

  cmd_start();
  cmd(COLOR(fg));
  cmd(CMD_BGCOLOR);
  cmd(bg);
  cmd(CMD_SCROLLBAR);
  cmd(( y  << 16) | x );
  cmd(( h  << 16) | w );
  cmd((val << 16) | op);
  cmd((rng << 16) | sz);
  cmd_end();

  cmd_start();
  cmd(TAG_MASK(0));
  cmd_end();

  cmd_start();
  cmd(CMD_TRACK);
  cmd(( y  << 16) | x);
  cmd(( h  << 16) | w);
  cmd(tag);
  cmd_end();
}


struct slider_prop {
  uint8_t              tag;
  uint16_t               x;
  uint16_t               y;
  uint16_t               w;
  uint16_t               h;
  uint32_t foregroundColor;
  uint32_t backgroundColor;
  uint16_t         options;
  uint16_t           value;
  uint16_t           range;
};


void draw_slider(struct slider_prop *prop) {
  uint32_t tag = prop->tag;
  uint32_t   x = prop->x;
  uint32_t   y = prop->y;
  uint32_t   w = prop->w;
  uint32_t   h = prop->h;
  uint32_t  fg = prop->foregroundColor;
  uint32_t  bg = prop->backgroundColor;
  uint32_t  op = prop->options;
  uint32_t val = prop->value;
  uint32_t rng = prop->range;

  cmd_start();
  cmd(TAG_MASK(1));
  cmd_end();

  cmd_start();
  cmd(TAG(tag));  
  cmd_end();

  cmd_start();
  cmd(COLOR(fg));
  cmd(CMD_BGCOLOR);
  cmd(bg);
  cmd(CMD_SLIDER);
  cmd(( y  << 16) | x );
  cmd(( h  << 16) | w );
  cmd((val << 16) | op);
  cmd(rng);
  cmd_end();

  cmd_start();
  cmd(TAG_MASK(0));
  cmd_end();

  cmd_start();
  cmd(CMD_TRACK);
  cmd(( y  << 16) | x);
  cmd(( h  << 16) | w);
  cmd(tag);
  cmd_end();
}


struct dial_prop {
  uint8_t              tag;
  uint16_t               x;
  uint16_t               y;
  uint16_t               r;
  uint32_t foregroundColor;
  uint32_t backgroundColor;
  uint16_t         options;
  uint16_t           value;
};

void draw_dial(struct dial_prop *prop) {
  uint32_t tag = prop->tag;
  uint32_t   x = prop->x;
  uint32_t   y = prop->y;
  uint32_t   r = prop->r;
  uint32_t  fg = prop->foregroundColor;
  uint32_t  bg = prop->backgroundColor;
  uint32_t  op = prop->options;
  uint32_t val = prop->value;

  cmd_start();
  cmd(TAG_MASK(1));
  cmd_end();

  cmd_start();
  cmd(TAG(tag));  
  cmd_end();

  cmd_start();
  cmd(COLOR(fg));
  cmd(CMD_BGCOLOR);
  cmd(bg);
  cmd(CMD_DIAL);
  cmd(( y << 16) | x );
  cmd((op << 16) | r);
  cmd(val);
  cmd_end();

  cmd_start();
  cmd(TAG_MASK(0));
  cmd_end();

  cmd_start();
  cmd(CMD_TRACK);
  cmd(( y  << 16) | x);
  cmd(( 1  << 16) | 1);
  cmd(tag);
  cmd_end();
}

struct toggle_prop {
  uint8_t              tag;
  uint16_t               x;
  uint16_t               y;
  uint16_t               w;
  uint16_t               f;
  uint32_t foregroundColor;
  uint32_t backgroundColor;
  uint16_t         options;
  uint16_t           stats;
  const char         *text;  
};

void draw_toggle(struct toggle_prop *prop) {
  uint32_t  tag = prop->tag;
  uint32_t    x = prop->x;
  uint32_t    y = prop->y;
  uint32_t    w = prop->w;
  uint32_t    f = prop->f;
  uint32_t   fg = prop->foregroundColor;
  uint32_t   bg = prop->backgroundColor;
  uint32_t   op = prop->options;
  uint32_t   st = prop->stats;
  const char *s = prop->text;

  cmd_start();
  cmd(TAG_MASK(1));
  cmd_end();

  cmd_start();
  cmd(TAG(tag));  
  cmd_end();

  cmd_start();
  cmd(COLOR(fg));
  cmd(CMD_BGCOLOR);
  cmd(bg);
  cmd(CMD_TOGGLE);
  cmd(( y << 16) | x );
  cmd(( f << 16) | w );
  cmd((st << 16) | op);
  uint32_t strlen = send_text(s);
  cmd(strlen);
  cmd_end();

  cmd_start();
  cmd(TAG_MASK(0));
  cmd_end();

  cmd_start();
  cmd(CMD_TRACK);
  cmd(( y  << 16) | x);
  cmd(( 1  << 16) | 1);
  cmd(tag);
  cmd_end();
}

struct keys_prop {
  uint8_t              tag;
  uint16_t               x;
  uint16_t               y;
  uint16_t               w;
  uint16_t               h;
  uint16_t               f;
  uint32_t foregroundColor;
  uint32_t backgroundColor;
  uint16_t         options;
  const char         *text;  
};

void draw_keys(struct keys_prop *prop) {
  uint32_t  tag = prop->tag;
  uint32_t    x = prop->x;
  uint32_t    y = prop->y;
  uint32_t    w = prop->w;
  uint32_t    h = prop->h;
  uint32_t    f = prop->f;
  uint32_t   fg = prop->foregroundColor;
  uint32_t   bg = prop->backgroundColor;
  uint32_t   op = prop->options;
  const char *s = prop->text;

  cmd_start();
  cmd(TAG_MASK(1));
  cmd_end();

  cmd_start();
  cmd(TAG(tag));  
  cmd_end();

  cmd_start();
  cmd(COLOR(fg));
  cmd(CMD_BGCOLOR);
  cmd(bg);
  cmd(CMD_KEYS);
  cmd(( y << 16) | x );
  cmd(( h << 16) | w );
  cmd((op << 16) | f );
  uint32_t strlen = send_text(s);
  cmd(strlen);
  cmd_end();

  cmd_start();
  cmd(TAG_MASK(0));
  cmd_end();
}

uint16_t get_tracker_value(const uint8_t tag) {
  uint32_t result = rd32(REG_TRACKER);
  uint8_t    rtag = result & 0x000000ff;
  uint16_t  value = (result >> 16);

  if (rtag == tag)  return value;
  return 0;
}

void set_touch_sensitivity(uint16_t sensitivity) {
  wr32(REG_TOUCH_RZTHRESH, sensitivity);
}

struct jpeg_prop {
  uint32_t handle;
  uint32_t   addr;
  uint16_t      x;
  uint16_t      y;
  uint32_t      w;
  uint32_t      h;
  uint8_t   scale;
  uint8_t     rot;
};


void draw_jpeg(struct jpeg_prop *prop) {
  uint32_t handle = prop->handle;
  uint32_t addr   = prop->addr;
  uint32_t x      = prop->x;
  uint32_t y      = prop->y;
  uint32_t w      = prop->w;
  uint32_t h      = prop->h;
  uint32_t s      = prop->scale;
  uint32_t r      = prop->rot;

  cmd_start();
  cmd(BITMAP_HANDLE(handle));
  cmd(BITMAP_SOURCE(RAM_G + addr));
  cmd(BITMAP_LAYOUT(RGB565, w*2, h));
  cmd(BITMAP_SIZE(NEAREST, BORDER, BORDER, w, h));
  cmd(BEGIN(BITMAPS));
  cmd(CMD_LOADIDENTITY);
  cmd(CMD_SCALE);
  cmd(s*65536);
  cmd(s*65536);
  cmd(CMD_TRANSLATE);
  cmd(65536*(w/2));
  cmd(65536*(h/2));
  cmd(CMD_ROTATE);
  cmd(r*65536/360);
  cmd(CMD_TRANSLATE);
  cmd(65536*(-w/2));
  cmd(65536*(-h/2));
  cmd(CMD_SETMATRIX);
  cmd(VERTEX2II(x, y, handle, 0));
  cmd(DISPLAY());
  cmd(END());
  cmd_end();
}



struct audio_prop {
  uint32_t addr;
  uint8_t  vol;
  uint32_t offset;
  uint16_t sampling_rate;
  uint8_t  format;
  uint8_t  loop;
};

bool play_audio(File& audioFile, struct audio_prop *prop) {
  waitForCommandFiFoEmpty();
  int32_t audio_size = audioFile.size();

  wr8(REG_VOL_PB, prop->vol);
  wr32(REG_PLAYBACK_START, prop->offset);
  wr32(REG_PLAYBACK_LENGTH, audio_size);
  wr16(REG_PLAYBACK_FREQ, prop->sampling_rate);
  wr8(REG_PLAYBACK_FORMAT, prop->format);
  wr8(REG_PLAYBACK_LOOP, prop->loop);

  // stream buf for AVI data
  static const int32_t chunk = 512;
  static uint8_t buf[chunk];

  uint32_t playback_write_address = RAM_G + prop->addr;
  uint32_t playback_read_address  = rd32(REG_PLAYBACK_READPTR);
  Serial.println("playback_write_address: " + String(playback_write_address));
  Serial.println("playback_read_address : " + String(playback_read_address));


  // write a chunk of AVI data to the media fifo
  if (audioFile.read(buf, chunk) < 0) {
    Serial.println("Audio file is too short");
    return false;
  };

  uint32_t address = playback_write_address | WRITE;
  SPI.beginTransaction(CmdSPISettings);
  digitalWrite(CS_PIN, LOW);  
  SPI.transfer((address >> 16) & 0xFF);
  SPI.transfer((address >> 8) & 0xFF);
  SPI.transfer(address & 0xFF);
  SPI.send(buf, chunk);
  digitalWrite(CS_PIN, HIGH);
  SPI.endTransaction();

  playback_write_address = playback_write_address + chunk;

  wr8(REG_PLAYBACK_PLAY, 1);

  while (audio_size > 0) {
    
    playback_read_address  = rd32(REG_PLAYBACK_READPTR);
    uint16_t diff = (playback_read_address - playback_write_address) & 0x1FFF;
    
    if (diff > chunk) {  // waiting for trasfering data to memory
      uint16_t n = min(chunk, audio_size);

      if (audioFile.read(buf, chunk) < 0) {
        Serial.println("End of Audio file");
        break;
      };

      address = playback_write_address | WRITE;


      SPI.beginTransaction(CmdSPISettings);
      digitalWrite(CS_PIN, LOW);  
      SPI.transfer((address >> 16) & 0xFF);
      SPI.transfer((address >> 8) & 0xFF);
      SPI.transfer(address & 0xFF);
      SPI.send(buf, chunk);
      digitalWrite(CS_PIN, HIGH);
      SPI.endTransaction();

      playback_write_address += n;
      audio_size -= n;
    }
  } 

  // Stop audio playback
  wr32(REG_PLAYBACK_LENGTH, 0);
  wr8(REG_PLAYBACK_PLAY, 1);

  return true;
}

void stop_audio() {
  wr32(REG_PLAYBACK_LENGTH, 0);
  wr8(REG_PLAYBACK_PLAY, 1);
}

struct sound_effect_prop {
  uint8_t vol;
  uint8_t effect;
  uint8_t pitch;
};

void play_sound_effect(struct sound_effect_prop *prop) {
  uint16_t effect = prop->effect;
  uint16_t pitch  = prop->pitch;
  wr8(REG_VOL_SOUND, prop->vol);
  wr8(REG_SOUND, (pitch << 8) | effect);
  wr8(REG_PLAY, 1);
}

void stop_sound_effect() {
  wr8(REG_VOL_SOUND, 0);
  wr8(REG_SOUND, 0);
  wr8(REG_PLAY, 1);
}

void swap_screen() {
  cmd_start();
  cmd(END_DL());
  cmd(SWAP());  
  cmd_end();
}

void begin_display() {
  pinMode(PDN_PIN, OUTPUT);
  digitalWrite(PDN_PIN, HIGH);
  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);  
  delay(100);


  // Initialization Sequence from Power Down using PD_N pin 
  digitalWrite(PDN_PIN, LOW);
  delay(100);
  digitalWrite(PDN_PIN, HIGH);
  delay(100);
  Serial.println("DISPLAY Power On");

  // MCU_SPI_CLK_Freq(<11MHz); //use the MCU SPI clock less than 11MHz
  SPI.begin();
  //SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));
	SPI.setClockDivider(SPI_CLOCK_DIV8);  
	SPI.setBitOrder(MSBFIRST);
	SPI.setDataMode(SPI_MODE0);    
  Serial.println("SPI open");

  host_command(FT_GPU_ACTIVE_M);

  uint8_t chipid; 
  do {   
    chipid = rd8(REG_ID);
    delay(100);
  } while (chipid != 0x7C);
  Serial.println("chipid = " + String(chipid, HEX));

  Serial.println("DISPLAY Settings");
  wr16(REG_HCYCLE,  FT_DispHCycle);
  wr16(REG_HOFFSET, FT_DispHOffset);
  wr16(REG_HSYNC0,  FT_DispHSync0);
  wr16(REG_HSYNC1,  FT_DispHSync1);
  wr16(REG_VCYCLE,  FT_DispVCycle);
  wr16(REG_VOFFSET, FT_DispVOffset);
  wr16(REG_VSYNC0,  FT_DispVSync0);
  wr16(REG_VSYNC1,  FT_DispVSync1);
  wr8(REG_SWIZZLE,  FT_DispSwizzle);
  wr8(REG_PCLK_POL, FT_DispPCLKPol);
  wr16(REG_HSIZE,   FT_DispWidth);
  wr16(REG_VSIZE,   FT_DispHeight);
  wr16(REG_CSPREAD, FT_DispCSpread);
  wr16(REG_DITHER,  FT_DispDither);

  wr8(REG_DLSWAP, DLSWAP_FRAME); //display list swap
  wr8(REG_GPIO_DIR,0x80 | rd8(REG_GPIO_DIR)); 
  wr8(REG_GPIO,0x080 | rd8(REG_GPIO)); //enable display bit
  wr8(REG_PCLK,5); //after this display is visible on the LCD
  delay(100);
}

void end_display() {
  SPI.end();
}

#define AUDIO_LINEAR_SAMPLE (0x00)
#define AUDIO_ULAW_SAMPLE   (0x01)
#define AUDIO_4BIT_ADPCM    (0x10)
#define AUDIO_UNDFINED      (0x00)

static uint8_t audio_buf[200000];
struct sound_effect_prop eprop;
struct audio_prop aprop;
void setup() {
  Serial.begin(115200);
  while(!SD.begin());

  File myFile = SD.open("test2.wav");
  if (myFile == NULL) {
    Serial.println("image file not found");
    return;
  }

  uint32_t audio_size = myFile.size();

  begin_display();

  aprop.vol = 0xff;
  aprop.offset = 0;;
  aprop.sampling_rate = 16000;
  aprop.format = AUDIO_LINEAR_SAMPLE;
  aprop.loop = 0;
  Serial.println("play_audio");
  play_audio(myFile, &aprop);


  eprop.vol = 0xff;
  eprop.effect = 0x00;
  eprop.pitch = 0x00;

  for (int e = 0x01; e <= 0xff; ++e) {
    eprop.effect = e;
    //for (int p = 0; p < 0xff; p += 16) {
    //  eprop.pitch = p;
      play_sound_effect(&eprop);
      //Serial.println("effect: " + String(e) + " pitch: " + String(p));
      Serial.println("effect: " + String(e));
      delay(300);
    //}
  }

  stop_sound_effect();
}

void loop() {

}
*/
