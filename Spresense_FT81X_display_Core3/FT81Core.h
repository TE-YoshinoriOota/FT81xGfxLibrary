#ifndef FT81CORE_HEADER_GUARD
#define FT81CORE_HEADER_GUARD

#include <Arduino.h>
#include <SPI.h>
#include <File.h>
#include <map>
#include "FT81_GPU.h"

#define SPI1_CS_PIN  (4)
#define SPI1_PDN_PIN (8)
#define SPI3_CS_PIN  (25)
#define SPI3_PDN_PIN (18)
#define SPI5_CS_PIN  (25)
#define SPI5_PDN_PIN (18)

#define SPI1_FREQ_LAUNCH   (8000000)
#define SPI1_FREQ_RUNING  (18000000)
#define SPI3_FREQ_LAUNCH   (4000000)
#define SPI3_FREQ_RUNING   (8000000)
#define SPI5_FREQ_LAUNCH   (4000000)
#define SPI5_FREQ_RUNING   (8000000)


#define READ  0x000000  // Bitmask address reading
#define WRITE 0x800000  // Bitmask for address writing

#define END_DL()         0x00  // End current display list
#define SWAP()           0xFFFFFF01  // Swap the current display list
#define COLOR(rgb)       ((0x04L << 24) | ((rgb)&0xFFFFFF))     
#define CLEAR_COLOR(rgb) ((0x02L << 24) | ((rgb)&0xFFFFFF))   


#define WVGA_FT_DispWidth     800
#define WVGA_FT_DispHeight    480
#define WVGA_FT_DispHCycle    928
#define WVGA_FT_DispHOffset   88
#define WVGA_FT_DispHSync0    0
#define WVGA_FT_DispHSync1    48
#define WVGA_FT_DispVCycle    525
#define WVGA_FT_DispVOffset   32
#define WVGA_FT_DispVSync0    0
#define WVGA_FT_DispVSync1    3
#define WVGA_FT_DispPCLK      2
#define WVGA_FT_DispSwizzle   0
#define WVGA_FT_DispPCLKPol   1
#define WVGA_FT_DispCSpread   0
#define WVGA_FT_DispDither    1


#define WQVGA_FT_DispWidth    480
#define WQVGA_FT_DispHeight   272
#define WQVGA_FT_DispHCycle   548
#define WQVGA_FT_DispHOffset  43
#define WQVGA_FT_DispHSync0   0
#define WQVGA_FT_DispHSync1   41
#define WQVGA_FT_DispVCycle   292
#define WQVGA_FT_DispVOffset  12
#define WQVGA_FT_DispVSync0   0
#define WQVGA_FT_DispVSync1   10
#define WQVGA_FT_DispPCLK     5
#define WQVGA_FT_DispSwizzle  0
#define WQVGA_FT_DispPCLKPol  1
#define WQVGA_FT_DispCSpread  1
#define WQVGA_FT_DispDither   1


#define QVGA_FT_DispWidth    320
#define QVGA_FT_DispHeight   240
#define QVGA_FT_DispHCycle   408
#define QVGA_FT_DispHOffset  70
#define QVGA_FT_DispHSync0   0
#define QVGA_FT_DispHSync1   10
#define QVGA_FT_DispVCycle   263
#define QVGA_FT_DispVOffset  13
#define QVGA_FT_DispVSync0   0
#define QVGA_FT_DispVSync1   2
#define QVGA_FT_DispPCLK     8
#define QVGA_FT_DispSwizzle  2
#define QVGA_FT_DispPCLKPol  0
#define QVGA_FT_DispCSpread  1
#define QVGA_FT_DispDither   1

/********************************************/
/*  RegisterOperation class                 */
/********************************************/

class RegisterOperation {
public:
  RegisterOperation(SPIClass *spi);
  ~RegisterOperation();

  uint8_t  rd8(const uint32_t  addr);
  uint16_t rd16(const uint32_t addr);
  uint32_t rd32(const uint32_t addr);
  void wr8(const uint32_t  addr, const uint8_t value);
  void wr16(const uint32_t addr, const uint16_t value);
  void wr32(const uint32_t addr, const uint32_t value);
  void host_command(const uint8_t cmd) ;

private:
  SPIClass* mySpi;
};


/********************************************/
/*  CommandOperation class                  */
/********************************************/

class CommandOperation {
public:
  CommandOperation(SPIClass *spi, RegisterOperation *reg);
  ~CommandOperation();

public:
  void cmd_start();
  void cmd(const uint32_t cmd);
  void cmd_end();

  void waitForCmdFiFoEmpty();
  void checkCmdFiFoBuffer(const uint32_t count);
  uint16_t checkCmdFiFoFreespace();
  void waitCmdFiFoFreespace(uint32_t need_freespace);

private:
  void incCmdWriteAddress(uint16_t delta);
  void updateCmdWriteAddress();

  uint32_t cmd_write_address = 0;
  SPISettings CmdSPISettings;
  SPIClass *mySpi;
  RegisterOperation *myReg;
};



/********************************************/
/*  MemoryOperation class                  */
/********************************************/

#define MEM_CHUNK (512)

class MemoryOperation {
public:
  MemoryOperation(SPIClass *spi, RegisterOperation *reg, CommandOperation *cmd);
  virtual ~MemoryOperation() {}
  void memWrite(uint32_t address, uint8_t buf[], uint32_t size);

protected:
  SPISettings CmdSPISettings;
  SPIClass *mySpi;
  CommandOperation *myCmd;
  RegisterOperation *myReg;
};



/********************************************/
/*  MediaOperation class                  */
/********************************************/

class MediaOperation : public MemoryOperation {
public:
  MediaOperation(SPIClass *spi,  RegisterOperation *reg, CommandOperation *cmd,const uint32_t size = 0x40000UL, const uint32_t offset = 0x100000UL);
  virtual ~MediaOperation() { media_fifo_exists = false; }
  void setupMediaFifo();
  void setupPlaybackMemory(uint32_t addr);
  void writeMediaFifo(uint8_t buf[], uint32_t size);
  void resetMediaFifo();
  void writePlaybackMemory(uint8_t buf[], uint32_t size);
  void resetPlaybackMemory() { playback_write_address = playback_address; }
  uint32_t getMediaFifoSize() { return media_fifo_size; }
  uint32_t getMediaFifoWriteAddress();
  uint32_t getMediaFifoReadAddress();
  uint32_t getPlaybackWriteAddress() { return playback_write_address; }
  uint32_t getPlaybackReadAddress();
  bool isMediaFifo() { return media_fifo_exists; }

private:
  bool media_fifo_exists;
  uint32_t media_fifo_size;
  uint32_t media_fifo_base;
  uint32_t media_write_address;
  uint32_t playback_address;
  uint32_t playback_write_address;
};






/********************************************/
/*  FT81xDisplay Class                      */
/********************************************/
class FT81xGuiComponent;
class FT81xInteractionComponent;

struct DisplayProperty {
  uint16_t  FT_DispWidth;
  uint16_t 	FT_DispHeight;
  uint16_t 	FT_DispHCycle;
  uint16_t 	FT_DispHOffset;
  uint16_t 	FT_DispHSync0;
  uint16_t 	FT_DispHSync1;
  uint16_t 	FT_DispVCycle;
  uint16_t 	FT_DispVOffset;
  uint16_t 	FT_DispVSync0;
  uint16_t 	FT_DispVSync1;
  uint16_t 	FT_DispPCLK;
  uint16_t 	FT_DispSwizzle;
  uint16_t 	FT_DispPCLKPol;
  uint16_t 	FT_DispCSpread;
  uint16_t 	FT_DispDither;
};


enum GuiComponents {
   FT81xCircle
  ,FT81xRectangle
  ,FT81xTriangle
  ,FT81xLine
  ,FT81xGradient
  ,FT81xClock
  ,FT81xText
  ,FT81xSpinner
  ,FT81xGuage
  ,FT81xProgressbar
  ,FT81xMJpeg
  ,FT81xButton
  ,FT81xScrollbar
  ,FT81xSlider
  ,FT81xDial
  ,FT81xToggle
  ,FT81xKeys
  ,FT81xImage
  ,FT81xAudio
  ,FT81xSynth
};

enum DisplaySize {
   DISPLAY_RESOLUTION_WVGA
  ,DISPLAY_RESOLUTION_WQVGA
  ,DISPLAY_RESOLUTION_QVGA
};


enum SPI_CHANNEL {
   SPI_CH1
  ,SPI_CH3
  ,SPI_CH5
};

class FT81xDisplay {
public:
  static FT81xDisplay* Instance(enum SPI_CHANNEL spi_ch);
  void begin(enum DisplaySize disp);
  void startDisplayList();
  void clear(const uint8_t r, const uint8_t g, const uint8_t b);
  bool senseInteractionComponents();
  void swap();

  uint16_t getDisplayWidth() { return mDispProp.FT_DispWidth; }
  uint16_t getDisplayHeight() { return mDispProp.FT_DispHeight; }
  uint32_t getBgColor() { return m_bgcolor;  }

public:
  FT81xGuiComponent* Create(enum GuiComponents elem);

protected:
  friend class FT81xInteractionComponent;
  void setTag(const uint8_t t, FT81xInteractionComponent *elem);
  void releaseTag(const uint8_t t);

public:
  bool doTouchCalibration();
  bool isTouchCalibration();
  void forceTouchCalibration();
  void removeTouchCalibration();

  bool isTouched();
  bool isTouched(uint16_t *x, uint16_t *y);

  void setTouchSensitivity(uint16_t sensitivity);

private:
  char m_prop_file[16];
  void _restoreTouchCalibration();
  uint8_t _read_tag();
  uint32_t _read_tracker_value(const uint8_t tag);

private:
  FT81xDisplay(SPIClass *spi);
  ~FT81xDisplay();
  struct DisplayProperty mDispProp;
  static FT81xDisplay *theDisplay1;
  static FT81xDisplay *theDisplay3;
  static FT81xDisplay *theDisplay5;
  uint32_t m_bgcolor;
  SPIClass *mySpi;
  static RegisterOperation *myReg;
  static CommandOperation *myCmd;
  static MediaOperation *myMem;

private:
  std::map<uint8_t, FT81xInteractionComponent*> mInteractionComponents;
};



/********************************************/
/*  FT81xGuiComponent Class                 */
/********************************************/

class FT81xGuiComponent {
public:
  virtual ~FT81xGuiComponent() {};
  enum GuiComponents type() { return mType; }
  virtual void draw() = 0;

protected:
  friend class FT81xDisplay;
  FT81xGuiComponent(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp) { myReg = reg; myCmd = cmd; myDisp = disp; };

protected:
  enum GuiComponents mType;
  FT81xDisplay *myDisp;
  RegisterOperation *myReg;
  CommandOperation *myCmd;
};


/********************************************/
/*  FT81xGfxCircle Class                    */
/********************************************/

class FT81xGfxCircle : public FT81xGuiComponent {
public:
  virtual ~FT81xGfxCircle();
  virtual void draw();
  void setPosition(const uint16_t x, const uint16_t y) { m_x = x; m_y = y; }
  void setColor(const uint8_t r, const uint8_t g, const uint8_t b) { m_color = COLOR_RGB(r, g, b); }
  void setSize(const uint16_t r) { m_r = r; }

protected:
 friend class FT81xDisplay;
 FT81xGfxCircle(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp);

private:
  uint16_t m_x;
  uint16_t m_y;
  uint16_t m_r;
  uint32_t m_color;
};


/********************************************/
/*  FT81xGfxRectangle Class                    */
/********************************************/

class FT81xGfxRectangle : public FT81xGuiComponent {
public:
  virtual ~FT81xGfxRectangle();
  virtual void draw();
  void setCorner(const uint8_t rad) { m_rad = rad; }
  void setColor(const uint8_t r, const uint8_t g, const uint8_t b) { m_color = COLOR_RGB(r, g, b); }
  void setPosition(const uint16_t x, const uint16_t y) { m_x = x; m_y = y; }
  void setSize(const uint16_t w, const uint16_t h) { m_w = w; m_h = h; }

protected:
  friend class FT81xDisplay;
  FT81xGfxRectangle(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp);

private:
  uint16_t m_x;
  uint16_t m_y;
  uint16_t m_h;
  uint16_t m_w;  
  uint32_t m_color;
  uint8_t  m_rad;
};



/********************************************/
/*  FT81xGfxTriangle Class                    */
/********************************************/

class FT81xGfxTriangle : public FT81xGuiComponent {
public:
  virtual ~FT81xGfxTriangle();
  virtual void draw();
  void setColor(const uint8_t r, const uint8_t g, const uint8_t b) { m_fgcolor = COLOR_RGB(r, g, b); }
  void setPoints(const uint16_t x1, const uint16_t y1, const uint16_t x2, const uint16_t y2, const uint16_t x3, const uint16_t y3); 

protected:
  friend class FT81xDisplay;
  FT81xGfxTriangle(RegisterOperation* reg, CommandOperation* cmd, FT81xDisplay *disp);

private:
  uint16_t m_x1;
  uint16_t m_y1;
  uint16_t m_x2;
  uint16_t m_y2;
  uint16_t m_x3;
  uint16_t m_y3;    
  uint32_t m_fgcolor;
  uint32_t m_bgcolor;
};


/********************************************/
/*  FT81xGfxLine Class                    */
/********************************************/

class FT81xGfxLine : public FT81xGuiComponent {
public:
  virtual ~FT81xGfxLine();
  virtual void draw();
  void setColor(const uint8_t r, const uint8_t g, const uint8_t b) { m_color = COLOR_RGB(r, g, b); }
  void setPoints(const uint16_t x1, const uint16_t y1, const uint16_t x2, const uint16_t y2) { m_x1 = x1; m_y1 = y1; m_x2 = x2; m_y2 = y2; } 
  void setWidth(const uint16_t w) { m_w = w; }

protected:
  friend class FT81xDisplay;
  FT81xGfxLine(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp);

private:
  uint16_t m_x1;
  uint16_t m_y1;
  uint16_t m_x2;
  uint16_t m_y2;
  uint16_t m_w;
  uint32_t m_color;
};


/********************************************/
/*  FT81xGfxGradient Class                    */
/********************************************/

class FT81xGfxGradient : public FT81xGuiComponent {
public:
  virtual ~FT81xGfxGradient();
  virtual void draw();
  void setStartColor(const uint8_t r, const uint8_t g, const uint8_t b) { m_color1 = COLOR_RGB(r, g, b); }
  void setEndColor(const uint8_t r, const uint8_t g, const uint8_t b) { m_color2 = COLOR_RGB(r, g, b); }
  void setPosition(const uint16_t x, const uint16_t y) { m_x = x; m_y = y; }
  void setSize(const uint16_t w, const uint16_t h) { m_w = w; m_h = h; } 

protected:
  friend class FT81xDisplay;
  FT81xGfxGradient(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp);

private:
  uint16_t m_x;
  uint16_t m_y;
  uint16_t m_w;
  uint16_t m_h;
  uint32_t m_color1;
  uint32_t m_color2;
};


/********************************************/
/*  FT81xGfxText Class                    */
/********************************************/

class FT81xGfxText : public FT81xGuiComponent {
public:
  virtual ~FT81xGfxText();
  virtual void draw();
  void draw(const char text[]);  
  void setFont(const uint8_t font);
  void set3dEffect() { m_options = OPT_3D; }
  void setColor(const uint8_t r, const uint8_t g, const uint8_t b) { m_color = COLOR_RGB(r, g, b); }
  void setPosition(const uint16_t x, const uint16_t y) { m_x = x; m_y = y; }
  void setText(const char text[]);

protected:
  friend class FT81xDisplay;
  FT81xGfxText(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp);

private:
  uint8_t  m_font;
  uint16_t m_options;
  uint16_t m_x;
  uint16_t m_y;
  uint32_t m_color;
  char *m_text;
};

inline uint32_t _send_text_helper_(const char text[], CommandOperation *cmd);

/********************************************/
/*  FT81xComponentClock Class                    */
/********************************************/

class FT81xComponentClock : public FT81xGuiComponent {
public:
  virtual ~FT81xComponentClock();
  virtual void draw();
  void draw(const uint8_t hour, const uint8_t min, const uint8_t sec);
  void setColor(const uint8_t r, const uint8_t g, const uint8_t b) { m_fgcolor = COLOR_RGB(r, g, b); }
  void setBgColor(const uint8_t r, const uint8_t g, const uint8_t b) { m_bgcolor = COLOR_RGB(r, g, b); }
  void setPosition(const uint16_t x, const uint16_t y) { m_x = x; m_y = y; }
  void setSize(const uint16_t r) { m_rad = r; };
  void setTime(const uint8_t hour, const uint8_t min, const uint8_t sec) { m_hour = hour; m_min = min; m_sec = sec; }
  void setNo3dEffect() { m_options |= OPT_FLAT; }
  void setNoSeconds() { m_options |= OPT_NOSECS; }
  void setNoBackground() { m_options |= OPT_NOSECS; }
  void setNoTicks() { m_options |= OPT_NOTICKS; }
  void setNoHands() { m_options |= OPT_NOHANDS; }

protected:
  friend class FT81xDisplay;
  FT81xComponentClock(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp);

private:
  uint16_t m_x;
  uint16_t m_y;
  uint16_t m_rad;
  uint8_t  m_hour;
  uint8_t  m_min;
  uint8_t  m_sec;
  uint32_t m_fgcolor;
  uint32_t m_bgcolor;
  uint16_t m_options;
};



/********************************************/
/*  FT81xComponentGuage Class                    */
/********************************************/

class FT81xComponentGuage : public FT81xGuiComponent {
public:
  virtual ~FT81xComponentGuage();
  virtual void draw();
  void draw(const uint16_t value);
  void setColor(const uint8_t r, const uint8_t g, const uint8_t b) { m_fgcolor = COLOR_RGB(r, g, b); }
  void setBgColor(const uint8_t r, const uint8_t g, const uint8_t b) { m_bgcolor = COLOR_RGB(r, g, b); }
  void setNeedleColor(const uint8_t r, const uint8_t g, const uint8_t b) { m_ncolor = COLOR_RGB(r, g, b); }
  void setPosition(const uint16_t x, const uint16_t y) { m_x = x; m_y = y; }
  void setSize(const uint16_t r) { m_rad = r; };
  void setMeter(const uint16_t major, const uint16_t minor, const uint16_t range) { m_major = major; m_minor = minor; m_range = range; }
  void setValue(const uint16_t value) { m_value = value; }

protected:
  friend class FT81xDisplay;
  FT81xComponentGuage(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp);

private:
  uint16_t m_x;
  uint16_t m_y;
  uint16_t m_rad;
  uint8_t  m_major;
  uint8_t  m_minor;
  uint16_t m_value;
  uint16_t m_range;
  uint32_t m_fgcolor;
  uint32_t m_bgcolor;
  uint32_t m_ncolor;  
  uint16_t m_options;
};


/********************************************/
/*  FT81xComponentProgressbar Class         */
/********************************************/

class FT81xComponentProgressbar : public FT81xGuiComponent {
public:
  virtual ~FT81xComponentProgressbar();
  virtual void draw();
  void draw(const uint16_t value);
  void setColor(const uint8_t r, const uint8_t g, const uint8_t b) { m_fgcolor = COLOR_RGB(r, g, b); }
  void setBgColor(const uint8_t r, const uint8_t g, const uint8_t b) { m_bgcolor = COLOR_RGB(r, g, b); }
  void setPosition(const uint16_t x, const uint16_t y) { m_x = x; m_y = y; }
  void setSize(const uint16_t w, const uint16_t h) { m_w = w; m_h = h; }
  void setRange(const uint16_t range) { m_range = range; }
  void setValue(const uint16_t value) { m_value = value; }

protected:
  friend class FT81xDisplay;
  FT81xComponentProgressbar(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp);

private:
  uint16_t m_x;
  uint16_t m_y;
  uint16_t m_w;
  uint16_t m_h;
  uint16_t m_options;
  uint16_t m_value;
  uint16_t m_range;
  uint32_t m_fgcolor;
  uint32_t m_bgcolor;
};




/********************************************/
/*  FT81xComponentSpinner Class         */
/********************************************/

class FT81xComponentSpinner : public FT81xGuiComponent {
public:
  virtual ~FT81xComponentSpinner();
  virtual void draw();
  void setColor(const uint8_t r, const uint8_t g, const uint8_t b) { m_color = COLOR_RGB(r, g, b); }
  void setPosition(const uint16_t x, const uint16_t y) { m_x = x; m_y = y; }
  void setSize(const uint16_t w, const uint16_t h) { m_w = w; m_h = h; }
  void setCircleStyle() { m_style = dot_circle; }
  void setLineStyle() { m_style = dot_line; }
  void setClockStyle() { m_style = rot_clock; }
  void setDotStyle() { m_style = two_dots; }

protected:
  friend class FT81xDisplay;
  FT81xComponentSpinner(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp);

private:
  enum spinner_style {
     dot_circle
    ,dot_line
    ,rot_clock
    ,two_dots
  };

private:
  uint16_t m_x;
  uint16_t m_y;
  uint16_t m_w;
  uint16_t m_h;
  uint16_t m_options;
  uint16_t m_style;
  uint16_t m_scale;
  uint32_t m_color;
};


/********************************************/
/*  FT81xInteractionCompoennt Class         */
/********************************************/

class FT81xInteractionComponent : public FT81xGuiComponent {
public:
  virtual ~FT81xInteractionComponent();
  virtual void draw() = 0;
  uint8_t getTag() { return mTag; }
  void setCallback(void (*func)(const uint32_t)) { m_func = func; }
  bool setTag(const uint8_t tag);
  void releaseTag(const uint8_t tag);

protected:
  friend class FT81xDisplay;
  FT81xInteractionComponent(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp);
  virtual void doInteraction_(const uint32_t value) = 0;
  virtual bool isTracker() = 0;

protected:
  // In case of InteructionKeys, mTag holds the most height tag number of the keymap.
  // For example, if the keymap is "12345", the mTag of interactionKey object is "0x35 (ascii 5)"
  uint8_t mTag;  
  void (*m_func)(const uint32_t);
};


/********************************************/
/*  FT81xInteractionButton Class            */
/********************************************/  

class FT81xInteractionButton : public FT81xInteractionComponent {
public:
  virtual ~FT81xInteractionButton();
  virtual void draw();
  void setFont(const uint8_t font);
  void setColor(const uint8_t r, const uint8_t g, const uint8_t b) { m_color = COLOR_RGB(r, g, b); }
  void setButtonColor(const uint8_t r, const uint8_t g, const uint8_t b) { m_bgcolor = COLOR_RGB(r, g, b); }
  void setPosition(const uint16_t x, const uint16_t y) { m_x = x; m_y = y; }
  void setSize(const uint16_t w, const uint16_t h) { m_w = w; m_h = h; }
  void setText(const char text[]);
  bool pushed();

protected:
  friend class FT81xDisplay;
  FT81xInteractionButton(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp);
  virtual void doInteraction_(const uint32_t value);
  virtual bool isTracker() { return false; }

private:
  uint16_t m_x;
  uint16_t m_y;
  uint16_t m_w;
  uint16_t m_h;
  uint8_t  m_font;
  uint32_t m_color;
  uint32_t m_bgcolor;
  uint16_t m_options;
  uint8_t  m_pushed;
  char *m_text;
};


/********************************************/
/*  FT81xInteractionButton Class            */
/********************************************/  

class FT81xInteractionKeys : public FT81xInteractionComponent {
public:
  virtual ~FT81xInteractionKeys();
  virtual void draw();
  void setFont(const uint8_t font);
  void setColor(const uint8_t r, const uint8_t g, const uint8_t b) { m_color = COLOR_RGB(r, g, b); }
  void setKeysColor(const uint8_t r, const uint8_t g, const uint8_t b) { m_bgcolor = COLOR_RGB(r, g, b); }
  void setPosition(const uint16_t x, const uint16_t y) { m_x = x; m_y = y; }
  void setSize(const uint16_t w, const uint16_t h) { m_w = w; m_h = h; }
  void setKeyMap(const char keymap[]);
  char readKey();

protected:
  friend class FT81xDisplay;
  FT81xInteractionKeys(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp);
  virtual void doInteraction_(const uint32_t value);
  virtual bool isTracker() { return false; }

private:
  uint16_t m_x;
  uint16_t m_y;
  uint16_t m_w;
  uint16_t m_h;
  uint8_t  m_font;
  uint32_t m_color;
  uint32_t m_bgcolor;
  uint16_t m_options;
  char  m_key;
  char *m_keymap;
};



/********************************************/
/*  FT81xInteractionDial Class            */
/********************************************/  

class FT81xInteractionDial : public FT81xInteractionComponent {
public:
  virtual ~FT81xInteractionDial();
  virtual void draw();
  void setColor(const uint8_t r, const uint8_t g, const uint8_t b) { m_color = COLOR_RGB(r, g, b); }
  void setDialColor(const uint8_t r, const uint8_t g, const uint8_t b) { m_bgcolor = COLOR_RGB(r, g, b); }
  void setPosition(const uint16_t x, const uint16_t y) { m_x = x; m_y = y; }
  void setSize(const uint16_t r) { m_rad = r; }
  uint16_t readValue() { return m_normValue; }

protected:
  friend class FT81xDisplay;
  FT81xInteractionDial(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp);
  virtual void doInteraction_(const uint32_t value);
  virtual bool isTracker() { return true; }
  
private:
  uint16_t m_x;
  uint16_t m_y;
  uint16_t m_rad;
  uint32_t m_color;
  uint32_t m_bgcolor;
  uint16_t m_options;
  uint16_t m_value;
  uint16_t m_normValue;
};


/********************************************/
/*  FT81xInteractionSlider Class            */
/********************************************/  

class FT81xInteractionSlider : public FT81xInteractionComponent {
public:
  virtual ~FT81xInteractionSlider();
  virtual void draw();
  void setColor(const uint8_t r, const uint8_t g, const uint8_t b) { m_color = COLOR_RGB(r, g, b); }
  void setSliderColor(const uint8_t r, const uint8_t g, const uint8_t b) { m_bgcolor = COLOR_RGB(r, g, b); }
  void setPosition(const uint16_t x, const uint16_t y) { m_x = x; m_y = y; }
  void setSize(const uint16_t w, const uint16_t h) { m_w = w; m_h = h; }
  void setRange(const uint16_t range) { m_range = range; }
  uint16_t readValue() { return m_value; }

protected:
  friend class FT81xDisplay;
  FT81xInteractionSlider(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp);
  virtual void doInteraction_(const uint32_t value);
  virtual bool isTracker() { return true; }
  
private:
  uint16_t m_x;
  uint16_t m_y;
  uint16_t m_w;
  uint16_t m_h;
  uint32_t m_color;
  uint32_t m_bgcolor;
  uint16_t m_options;
  uint16_t m_range;
  uint16_t m_value;
};



/********************************************/
/*  FT81xInteractionToggle Class            */
/********************************************/  

class FT81xInteractionToggle : public FT81xInteractionComponent {
public:
  virtual ~FT81xInteractionToggle();
  virtual void draw();
  void setFont(const uint8_t font);
  void setColor(const uint8_t r, const uint8_t g, const uint8_t b) { m_color = COLOR_RGB(r, g, b); }
  void setToggleColor(const uint8_t r, const uint8_t g, const uint8_t b) { m_bgcolor = COLOR_RGB(r, g, b); }
  void setPosition(const uint16_t x, const uint16_t y) { m_x = x; m_y = y; }
  void setSize(const uint16_t w) { m_w = w; }
  void setText(const char text0[], const char text1[]);
  uint16_t readState() { return m_state; }

protected:
  friend class FT81xDisplay;
  FT81xInteractionToggle(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp);
  virtual void doInteraction_(const uint32_t value);
  virtual bool isTracker() { return true; }
  
private:
  uint16_t m_x;
  uint16_t m_y;
  uint16_t m_w;
  uint16_t m_h;
  uint8_t  m_font;
  uint32_t m_color;
  uint32_t m_bgcolor;
  uint16_t m_options;
  uint8_t  m_pushed;
  uint16_t m_state;
  char *m_text;
};



/********************************************/
/*  FT81xInteractionScrollbar Class            */
/********************************************/  

class FT81xInteractionScrollbar : public FT81xInteractionComponent {
public:
  virtual ~FT81xInteractionScrollbar() {}
  virtual void draw();
  void setColor(const uint8_t r, const uint8_t g, const uint8_t b) { m_color = COLOR_RGB(r, g, b); }
  void setScrollbarColor(const uint8_t r, const uint8_t g, const uint8_t b) { m_bgcolor = COLOR_RGB(r, g, b); }
  void setPosition(const uint16_t x, const uint16_t y) { m_x = x; m_y = y; }
  void setSize(const uint16_t w, const uint16_t h) { m_w = w; m_h = h; }
  void setBarSize(const uint16_t size) { m_size = size; }
  void setRange(const uint16_t range) { m_range = range; }
  uint16_t readValue() { return m_value; }

protected:
  friend class FT81xDisplay;
  FT81xInteractionScrollbar(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp);
  virtual void doInteraction_(const uint32_t value);
  virtual bool isTracker() { return true; }
  
private:
  uint16_t m_x;
  uint16_t m_y;
  uint16_t m_w;
  uint16_t m_h;
  uint32_t m_color;
  uint32_t m_bgcolor;
  uint16_t m_options;
  uint16_t m_value;
  uint16_t m_size;
  uint16_t m_range;
};



/********************************************/
/*  FT81xMediaCompoennt Class               */
/********************************************/

class FT81xMediaComponent : public FT81xGuiComponent {
public:
  virtual ~FT81xMediaComponent();
  virtual void draw() = 0;
  virtual void setupMemory(uint32_t addr) = 0;
  uint32_t getChunkSize() { return mChunk; }
  uint8_t* getTransBuf() { return mTransBuf; }

protected:
  friend class FT81xDisplay;
  FT81xMediaComponent(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp, MediaOperation *mem, const uint32_t chunk);
 
protected:
  MediaOperation* myMem;

private:
  uint32_t mChunk;
  uint8_t *mTransBuf;
};



/********************************************/
/*  FT81xImageCompoennt Class               */
/********************************************/

class FT81xMediaImage : public FT81xMediaComponent {
public:
  virtual ~FT81xMediaImage();
  virtual void draw();
  virtual void setupMemory(uint32_t addr);
  bool setImage(File &file);
  void setImage(const uint8_t buf[], const uint32_t size);
  void setPosition(uint16_t x, uint16_t y) { m_x = x; m_y = y; }
  void setSize(uint16_t w, uint16_t h) { m_w = w; m_h = h; }
  void setScale(uint8_t scale) { m_scale = scale; }
  void setRotation(uint16_t rot) { m_rot = rot; }

protected:
  friend class FT81xDisplay;
  void setHandle(uint32_t handle) { mHandle = handle; }
  FT81xMediaImage(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp, MediaOperation *mem, const uint32_t chunk = 512);
 
private:
  uint32_t mHandle;
  uint32_t mAddress;
  uint16_t m_x;
  uint16_t m_y;
  uint16_t m_w;
  uint16_t m_h;
  uint8_t  m_scale;
  uint16_t m_rot;
};



/********************************************/
/*  FT81xMediaSynth Class                   */
/********************************************/
/* Note definitions */
#define NOTE_A0 (21)
#define NOTE_A_SHARP_0 (22)
#define NOTE_B0 (23)
#define NOTE_C1 (24)
#define NOTE_C_SHARP_1 (25)
#define NOTE_D1 (26)
#define NOTE_D_SHARP_1 (27)
#define NOTE_E1 (28)
#define NOTE_F1 (29)
#define NOTE_F_SHARP_1 (30)
#define NOTE_G1 (31)
#define NOTE_G_SHARP_1 (32)
#define NOTE_A1 (33)
#define NOTE_A_SHARP_1 (34)
#define NOTE_B1 (35)
#define NOTE_C2 (36)
#define NOTE_C_SHARP_2 (37)
#define NOTE_D2 (38)
#define NOTE_D_SHARP_2 (39)
#define NOTE_E2 (40)
#define NOTE_F2 (41)
#define NOTE_F_SHARP_2 (42)
#define NOTE_G2 (43)
#define NOTE_G_SHARP_2 (44)
#define NOTE_A2 (45)
#define NOTE_A_SHARP_2 (46)
#define NOTE_B2 (47)
#define NOTE_C3 (48)
#define NOTE_C_SHARP_3 (49)
#define NOTE_D3 (50)
#define NOTE_D_SHARP_3 (51)
#define NOTE_E3 (52)
#define NOTE_F3 (53)
#define NOTE_F_SHARP_3 (54)
#define NOTE_G3 (55)
#define NOTE_G_SHARP_3 (56)
#define NOTE_A3 (57)
#define NOTE_A_SHARP_3 (58)
#define NOTE_B3 (59)
#define NOTE_C4 (60)
#define NOTE_C_SHARP_4 (61)
#define NOTE_D4 (62)
#define NOTE_D_SHARP_4 (63)
#define NOTE_E4 (64)
#define NOTE_F4 (65)
#define NOTE_F_SHARP_4 (66)
#define NOTE_G4 (67)
#define NOTE_G_SHARP_4 (68)
#define NOTE_A4 (69)
#define NOTE_A_SHARP_4 (70)
#define NOTE_B4 (71)
#define NOTE_C5 (72)
#define NOTE_C_SHARP_5 (73)
#define NOTE_D5 (74)
#define NOTE_D_SHARP_5 (75)
#define NOTE_E5 (76)
#define NOTE_F5 (77)
#define NOTE_F_SHARP_5 (78)
#define NOTE_G5 (79)
#define NOTE_G_SHARP_5 (80)
#define NOTE_A5 (81)
#define NOTE_A_SHARP_5 (82)
#define NOTE_B5 (83)
#define NOTE_C6 (84)
#define NOTE_C_SHARP_6 (85)
#define NOTE_D6 (86)
#define NOTE_D_SHARP_6 (87)
#define NOTE_E6 (88)
#define NOTE_F6 (89)
#define NOTE_F_SHARP_6 (90)
#define NOTE_G6 (91)
#define NOTE_G_SHARP_6 (92)
#define NOTE_A6 (93)
#define NOTE_A_SHARP_6 (94)
#define NOTE_B6 (95)
#define NOTE_C7 (96)
#define NOTE_C_SHARP_7 (97)
#define NOTE_D7 (98)
#define NOTE_D_SHARP_7 (99)
#define NOTE_E7 (100)
#define NOTE_F7 (101)
#define NOTE_F_SHARP_7 (102)
#define NOTE_G7 (103)
#define NOTE_G_SHARP_7 (104)
#define NOTE_A7 (105)
#define NOTE_A_SHARP_7 (106)
#define NOTE_B7 (107)
#define NOTE_C8 (108)

/* Sound Effects                   continous   pitch */
#define SILENCE (0x00)          //     Y          N
#define SQURE_WAVE (0x01)       //     Y          Y
#define SIN_WAVE (0x02)         //     Y          Y
#define SAW_WAVE (0x03)         //     Y          Y
#define TRIANGLE_WAVE (0x04)    //     Y          Y
#define BEEP (0x05)             //     Y          Y
#define ALARM (0x06)            //     Y          Y
#define WARBLE (0x07)           //     Y          Y
#define CAROUSEL (0x08)         //     Y          Y
#define PIP01 (0x10)            //     N          Y
#define PIP02 (0x11)            //     N          Y
#define PIP03 (0x12)            //     N          Y
#define PIP04 (0x13)            //     N          Y
#define PIP05 (0x14)            //     N          Y
#define PIP06 (0x15)            //     N          Y
#define PIP07 (0x16)            //     N          Y
#define PIP08 (0x17)            //     N          Y
#define PIP09 (0x18)            //     N          Y
#define PIP10 (0x19)            //     N          Y
#define PIP11 (0x1A)            //     N          Y
#define PIP12 (0x1B)            //     N          Y
#define PIP13 (0x1C)            //     N          Y
#define PIP14 (0x1D)            //     N          Y
#define PIP15 (0x1E)            //     N          Y
#define PIP16 (0x1F)            //     N          Y
#define DTMFA (0x23)            //     Y          N
#define DTMFB (0x2C)            //     Y          N
#define DTMF0 (0x30)            //     Y          N
#define DTMF1 (0x31)            //     Y          N
#define DTMF2 (0x32)            //     Y          N
#define DTMF3 (0x33)            //     Y          N
#define DTMF4 (0x34)            //     Y          N
#define DTMF5 (0x35)            //     Y          N
#define DTMF6 (0x36)            //     Y          N
#define DTMF7 (0x37)            //     Y          N
#define DTMF8 (0x38)            //     Y          N
#define DTMF9 (0x39)            //     Y          N
#define HARP (0x40)             //     N          Y
#define XYLOPHONE (0x41)        //     N          Y
#define TUBA (0x42)             //     N          Y
#define GLOCKENSPIEL (0x43)     //     N          Y
#define ORGAN (0x44)            //     N          Y
#define TRUNPET (0x45)          //     N          Y
#define PIANO (0x46)            //     N          Y
#define CHIMES (0x47)           //     N          Y
#define MUSICBOX (0x48)         //     N          Y
#define BELL (0x49)             //     N          Y
#define CLICK (0x50)            //     N          N
#define SWITCH (0x51)           //     N          N
#define COWBELL (0x52)          //     N          N
#define NOTCH (0x53)            //     N          N
#define HIHAT (0x54)            //     N          N
#define KICKDRUM (0x55)         //     N          N
#define POP (0x56)              //     N          N
#define CLACK (0x57)            //     N          N
#define CHACK (0x58)            //     N          N
#define MUTE (0x60)             //     N          N
#define UNMUTE (0x61)           //     N          N

class FT81xMediaSynth : public FT81xMediaComponent {
public:
  virtual ~FT81xMediaSynth() {}
  virtual void draw();
  virtual void setupMemory(uint32_t addr) { /* no need */};
  void play();
  void stop();  
  void setVolume(uint8_t vol);
  bool setNote(uint8_t note);
  bool setEffect(uint8_t effect); 
  bool isContinuous(uint8_t effect);

protected:
  friend class FT81xDisplay;
  FT81xMediaSynth(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp, MediaOperation *mem, const uint32_t chunk = 0);
 
private:
  uint8_t m_volume;
  uint8_t m_note;
  uint8_t m_effect;
};


/********************************************/
/*  FT81xMediaAudio Class                   */
/********************************************/


class FT81xMediaAudio : public FT81xMediaComponent {
public:
  virtual ~FT81xMediaAudio();
  virtual void draw();
  virtual void setupMemory(uint32_t addr);
  void play();
  void stop();  
  bool setAudio(File &file);
  void setAudio(uint8_t buf[], uint32_t size);
  bool setFormat(uint8_t format);  // LINEAR_SAMPLES, ULAW_SAMPLES, ADPCM_SAMPLES 
  bool setSamplingRate(uint16_t sampling_rate); // 8kHz - 48kHz
  void setVolume(uint8_t vol);
  void setLoop(bool loop) { m_loop = loop; }

protected:
  friend class FT81xDisplay;
  FT81xMediaAudio(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp, MediaOperation *mem, const uint32_t chunk = 512);
 
private:
  uint32_t mAddress;
  uint32_t m_audio_size;
  uint8_t m_volume;
  uint16_t m_sampling_rate;
  uint8_t m_format;
  bool m_loop;
};


/********************************************/
/*  FT81xMediaMJpeg Class               */
/********************************************/

class FT81xMediaMJpeg : public FT81xMediaComponent {
public:
  virtual ~FT81xMediaMJpeg();
  virtual void draw();
  virtual void setupMemory(uint32_t addr);
  bool play(File &file);
  void play();
  void setMJpeg(const uint8_t buf[], const uint32_t size);
  void setFullscreen() { m_options |= OPT_FULLSCREEN; }
  void enableSound() { m_options |= OPT_SOUND;}


protected:
  friend class FT81xDisplay;
  FT81xMediaMJpeg(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp, MediaOperation *mem, const uint32_t chunk = 512);
 
private:
  //uint32_t mAddress;
  uint32_t m_options;
};


#endif // FT81CORE_HEADER_GUARD