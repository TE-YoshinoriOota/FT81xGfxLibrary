#include "FT81Core.h"
#include "FT81xDisplay.h"
#include "FT81xGuiComponent.h"
#include "FT81xGfxCircle.h"
#include "FT81xGfxRectangle.h"
#include "FT81xGfxTriangle.h"
#include "FT81xGfxLine.h"
#include "FT81xGfxGradient.h"
#include "FT81xGfxText.h"
#include "FT81xComponentGauge.h"
#include "FT81xComponentClock.h"
#include "FT81xComponentProgressbar.h"
#include "FT81xComponentSpinner.h"

#include "FT81xInteractionComponent.h"
#include "FT81xInteractionButton.h"
#include "FT81xInteractionKeys.h"
#include "FT81xInteractionDial.h"
#include "FT81xInteractionSlider.h"
#include "FT81xInteractionToggle.h"
#include "FT81xInteractionScrollbar.h"

#include "FT81xMediaComponent.h"
#include "FT81xMediaImage.h"
#include "FT81xMediaSynth.h"
#include "FT81xMediaAudio.h"
#include "FT81xMediaMovie.h"

#include <Flash.h>


static uint32_t SPI_FREQ_LAUNCH = SPI5_FREQ_LAUNCH;
static uint32_t SPI_FREQ_RUNING = SPI5_FREQ_RUNING;
static uint8_t CS_PIN = SPI1_CS_PIN;
static uint8_t PDN_PIN = SPI1_PDN_PIN;


/********************************************/
/*  FT81xDisplay Class                      */
/********************************************/

FT81xDisplay *FT81xDisplay::theDisplay1 = NULL;
FT81xDisplay *FT81xDisplay::theDisplay3 = NULL;
FT81xDisplay *FT81xDisplay::theDisplay5 = NULL;
RegisterOperation *FT81xDisplay::myReg = NULL;
CommandOperation *FT81xDisplay::myCmd = NULL;
MediaOperation *FT81xDisplay::myMem = NULL;

FT81xDisplay::FT81xDisplay(SPIClass *spi) {
  Serial.begin(115200);
  mySpi = spi;
  if (myReg == NULL) {
    myReg = new RegisterOperation(spi);
  }  
  if (myCmd == NULL) {
    myCmd = new CommandOperation(spi, myReg);
  }
  memcpy(m_prop_file, "eve_prop.txt", strlen("eve_prop.txt"));
}

FT81xDisplay::~FT81xDisplay() {
  delete myCmd; myCmd = NULL;
  delete myReg; myReg = NULL;
}

FT81xDisplay* FT81xDisplay::Instance(enum SPI_CHANNEL spi_ch) {
  switch (spi_ch) {
  case SPI_CH1:
    if (theDisplay1 == NULL) {
      SPI_FREQ_LAUNCH = SPI1_FREQ_LAUNCH;
      SPI_FREQ_RUNING = SPI1_FREQ_RUNING;
      CS_PIN = SPI1_CS_PIN;
      PDN_PIN = SPI1_PDN_PIN;
      theDisplay1 = new FT81xDisplay(&SPI);
    }
    return theDisplay1;
  case SPI_CH3:
    if (theDisplay3 == NULL) {
      SPI_FREQ_LAUNCH = SPI3_FREQ_LAUNCH;
      SPI_FREQ_RUNING = SPI3_FREQ_RUNING;
      CS_PIN = SPI3_CS_PIN;
      PDN_PIN = SPI3_PDN_PIN;
      theDisplay3 = new FT81xDisplay(&SPI3);
    }
    return theDisplay3;
  case SPI_CH5:
    if (theDisplay5 == NULL) {
      SPI_FREQ_LAUNCH = SPI5_FREQ_LAUNCH;
      SPI_FREQ_RUNING = SPI5_FREQ_RUNING;
      CS_PIN = SPI5_CS_PIN;
      PDN_PIN = SPI5_PDN_PIN;
      theDisplay5 = new FT81xDisplay(&SPI5);
    }
    return theDisplay5;
  }
  return NULL;
}

void FT81xDisplay::begin(enum DisplaySize disp) {

  switch (disp) {
  case DISPLAY_RESOLUTION_WVGA:
    mDispProp.FT_DispWidth  = WVGA_FT_DispWidth;
    mDispProp.FT_DispHeight = WVGA_FT_DispHeight;
    mDispProp.FT_DispHCycle = WVGA_FT_DispHCycle;
    mDispProp.FT_DispHOffset = WVGA_FT_DispHOffset;
    mDispProp.FT_DispHSync0 = WVGA_FT_DispHSync0;
    mDispProp.FT_DispHSync1 = WVGA_FT_DispHSync1;
    mDispProp.FT_DispVCycle = WVGA_FT_DispVCycle;
    mDispProp.FT_DispVOffset = WVGA_FT_DispVOffset;
    mDispProp.FT_DispVSync0 = WVGA_FT_DispVSync0;
    mDispProp.FT_DispVSync1 = WVGA_FT_DispVSync1;
    mDispProp.FT_DispPCLK = WVGA_FT_DispPCLK;
    mDispProp.FT_DispSwizzle = WVGA_FT_DispSwizzle;
    mDispProp.FT_DispPCLKPol = WVGA_FT_DispPCLKPol;
    mDispProp.FT_DispCSpread = WVGA_FT_DispCSpread;
    mDispProp.FT_DispDither = WVGA_FT_DispDither;    
    break;
  case DISPLAY_RESOLUTION_WQVGA:
    mDispProp.FT_DispWidth  = WQVGA_FT_DispWidth;
    mDispProp.FT_DispHeight = WQVGA_FT_DispHeight;
    mDispProp.FT_DispHCycle = WQVGA_FT_DispHCycle;
    mDispProp.FT_DispHOffset = WQVGA_FT_DispHOffset;
    mDispProp.FT_DispHSync0 = WQVGA_FT_DispHSync0;
    mDispProp.FT_DispHSync1 = WQVGA_FT_DispHSync1;
    mDispProp.FT_DispVCycle = WQVGA_FT_DispVCycle;
    mDispProp.FT_DispVOffset = WQVGA_FT_DispVOffset;
    mDispProp.FT_DispVSync0 = WQVGA_FT_DispVSync0;
    mDispProp.FT_DispVSync1 = WQVGA_FT_DispVSync1;
    mDispProp.FT_DispPCLK = WQVGA_FT_DispPCLK;
    mDispProp.FT_DispSwizzle = WQVGA_FT_DispSwizzle;
    mDispProp.FT_DispPCLKPol = WQVGA_FT_DispPCLKPol;
    mDispProp.FT_DispCSpread = WQVGA_FT_DispCSpread;
    mDispProp.FT_DispDither = WQVGA_FT_DispDither;   
    break;
  case DISPLAY_RESOLUTION_QVGA:
    mDispProp.FT_DispWidth  = QVGA_FT_DispWidth;
    mDispProp.FT_DispHeight = QVGA_FT_DispHeight;
    mDispProp.FT_DispHCycle = QVGA_FT_DispHCycle;
    mDispProp.FT_DispHOffset = QVGA_FT_DispHOffset;
    mDispProp.FT_DispHSync0 = QVGA_FT_DispHSync0;
    mDispProp.FT_DispHSync1 = QVGA_FT_DispHSync1;
    mDispProp.FT_DispVCycle = QVGA_FT_DispVCycle;
    mDispProp.FT_DispVOffset = QVGA_FT_DispVOffset;
    mDispProp.FT_DispVSync0 = QVGA_FT_DispVSync0;
    mDispProp.FT_DispVSync1 = QVGA_FT_DispVSync1;
    mDispProp.FT_DispPCLK = QVGA_FT_DispPCLK;
    mDispProp.FT_DispSwizzle = QVGA_FT_DispSwizzle;
    mDispProp.FT_DispPCLKPol = QVGA_FT_DispPCLKPol;
    mDispProp.FT_DispCSpread = QVGA_FT_DispCSpread;
    mDispProp.FT_DispDither = QVGA_FT_DispDither;    
    break;
  }

  pinMode(PDN_PIN, OUTPUT);
  digitalWrite(PDN_PIN, HIGH);
  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);  
  delay(100);

  /* Initialization Sequence from Power Down using PD_N pin */
  digitalWrite(PDN_PIN, LOW);
  delay(100);
  digitalWrite(PDN_PIN, HIGH);
  delay(100);
  Serial.println("DISPLAY Power On");

  /* MCU_SPI_CLK_Freq(<11MHz); //use the MCU SPI clock less than 11MHz */
  mySpi->begin();
  delay(100);

  mySpi->beginTransaction(SPISettings(SPI_FREQ_LAUNCH, MSBFIRST, SPI_MODE0));
  Serial.println("SPI open");

  myReg->host_command(FT_GPU_ACTIVE_M);

  uint8_t chipid; 
  do {   
    chipid = myReg->rd8(REG_ID);
    delay(100);
  } while (chipid != 0x7C);
  Serial.println("chipid = " + String(chipid, HEX));

  Serial.println("DISPLAY Settings");
  myReg->wr16(REG_HCYCLE,  mDispProp.FT_DispHCycle);
  myReg->wr16(REG_HOFFSET, mDispProp.FT_DispHOffset);
  myReg->wr16(REG_HSYNC0,  mDispProp.FT_DispHSync0);
  myReg->wr16(REG_HSYNC1,  mDispProp.FT_DispHSync1);
  myReg->wr16(REG_VCYCLE,  mDispProp.FT_DispVCycle);
  myReg->wr16(REG_VOFFSET, mDispProp.FT_DispVOffset);
  myReg->wr16(REG_VSYNC0,  mDispProp.FT_DispVSync0);
  myReg->wr16(REG_VSYNC1,  mDispProp.FT_DispVSync1);
  myReg->wr8(REG_SWIZZLE,  mDispProp.FT_DispSwizzle);
  myReg->wr8(REG_PCLK_POL, mDispProp.FT_DispPCLKPol);
  myReg->wr16(REG_HSIZE,   mDispProp.FT_DispWidth);
  myReg->wr16(REG_VSIZE,   mDispProp.FT_DispHeight);
  myReg->wr16(REG_CSPREAD, mDispProp.FT_DispCSpread);
  myReg->wr16(REG_DITHER,  mDispProp.FT_DispDither);

  myReg->wr8(REG_DLSWAP, DLSWAP_FRAME); //display list swap
  myReg->wr8(REG_GPIO_DIR,0x80 | myReg->rd8(REG_GPIO_DIR)); 
  myReg->wr8(REG_GPIO,0x080 | myReg->rd8(REG_GPIO)); //enable display bit
  myReg->wr8(REG_PCLK,5); //after this display is visible on the LCD
  mySpi->endTransaction();
  delay(100);  
}

void FT81xDisplay::readyCanvas() {
  myCmd->cmd_start();
  myCmd->cmd(CMD_DLSTART);
  myCmd->cmd(CLEAR(1, 1, 1));
  myCmd->cmd_end();  
}


void FT81xDisplay::clear(const uint8_t r, const uint8_t g, const uint8_t b) {
  m_bgcolor = COLOR_RGB(r, g, b);
  myCmd->cmd_start();
  myCmd->cmd(CLEAR_COLOR_RGB(r, g, b));
  myCmd->cmd(CLEAR(1, 1, 1));
  myCmd->cmd_end();  
}

FT81xGuiComponent* FT81xDisplay::Create(enum GuiComponents elem) {
  switch(elem) {
  case FT81xCircle:
    return new FT81xGfxCircle(myReg, myCmd, this);
  case FT81xRectangle:
    return new FT81xGfxRectangle(myReg, myCmd, this);
  case FT81xTriangle:
    return new FT81xGfxTriangle(myReg, myCmd, this);
  case FT81xLine:
    return new FT81xGfxLine(myReg, myCmd, this);
  case FT81xGradient:
    return new FT81xGfxGradient(myReg, myCmd, this);
  case FT81xText:
    return new FT81xGfxText(myReg, myCmd, this);
  case FT81xClock:
    return new FT81xComponentClock(myReg, myCmd, this);
  case FT81xGauge:
    return new FT81xComponentGauge(myReg, myCmd, this);
  case FT81xProgressbar:
    return new FT81xComponentProgressbar(myReg, myCmd, this);
  case FT81xSpinner:
    return new FT81xComponentSpinner(myReg, myCmd, this);
  case FT81xButton:
    return new FT81xInteractionButton(myReg, myCmd, this);
  case FT81xKeys:
    return new FT81xInteractionKeys(myReg, myCmd, this);
  case FT81xDial:
    return new FT81xInteractionDial(myReg, myCmd, this);
  case FT81xSlider:
    return new FT81xInteractionSlider(myReg, myCmd, this);
  case FT81xToggle:
    return new FT81xInteractionToggle(myReg, myCmd, this);
  case FT81xScrollbar:
    return new FT81xInteractionScrollbar(myReg, myCmd, this);
  case FT81xImage:
    {
      if (myMem == NULL) {
        myMem = new MediaOperation(mySpi, myReg, myCmd);
      }    
      FT81xMediaImage* image = new FT81xMediaImage(myReg, myCmd, this, myMem);
      image->setHandle(1); // need to do
      return image;
    }
  case FT81xSynth:
    return new FT81xMediaSynth(myReg, myCmd, this, myMem);
  case FT81xAudio:
    {
      if (myMem == NULL) {
        myMem = new MediaOperation(mySpi, myReg, myCmd);
      }    
      return new FT81xMediaAudio(myReg, myCmd, this, myMem);
    }  
  case FT81xMovie:
    {
      if (myMem == NULL) {
        myMem = new MediaOperation(mySpi, myReg, myCmd);
      }    
      return new FT81xMediaMovie(myReg, myCmd, this, myMem);
    }    
  }
  return NULL;
}

void FT81xDisplay::releaseTag(uint8_t t) {
  myCmd->cmd_start();
  myCmd->cmd(TAG_MASK(1));  
  myCmd->cmd(CLEAR_TAG(t));
  myCmd->cmd(TAG_MASK(0));  
  myCmd->cmd_end();
  mInteractionComponents.erase(t);  
}

void FT81xDisplay::setTag(const uint8_t t, FT81xInteractionComponent *elem) {
  mInteractionComponents[t] = elem;
}

bool FT81xDisplay::senseInteractionComponents() {
  if (!this->isTouched()) return false;
  uint8_t tag = _read_tag();
  // Serial.println("tag : " + String(tag));
  if (tag == 0) return false;
  FT81xInteractionComponent* elem = mInteractionComponents[tag];
  if (elem != NULL) { 
    if (elem->isTracker()) {
      uint32_t value = _read_tracker_value(tag);
      elem->doInteraction_(value);
    } else {
      elem->doInteraction_(tag);
    }
  } else {
    Serial.println("Error: tag matching is wrong!");
    return false;
  }
  return true;
}

void FT81xDisplay::updateCanvas() {
  myCmd->cmd_start();
  myCmd->cmd(END_DL());
  myCmd->cmd(SWAP());  
  myCmd->cmd_end();  
}


bool FT81xDisplay::doTouchCalibration() {
  if (Flash.exists(m_prop_file)) {
    Serial.println("property file already exists");
    this->_restoreTouchCalibration();
    return false;
  } else {
    myCmd->cmd_start();
    myCmd->cmd(CMD_CALIBRATE);
    myCmd->cmd(0);
    myCmd->cmd_end();
    myCmd->waitForCmdFiFoEmpty();

    //prop.config = rd32(REG_TOUCH_CONFIG);
    uint32_t A = myReg->rd32(REG_TOUCH_TRANSFORM_A);
    uint32_t B = myReg->rd32(REG_TOUCH_TRANSFORM_B);
    uint32_t C = myReg->rd32(REG_TOUCH_TRANSFORM_C);
    uint32_t D = myReg->rd32(REG_TOUCH_TRANSFORM_D);
    uint32_t E = myReg->rd32(REG_TOUCH_TRANSFORM_E);
    uint32_t F = myReg->rd32(REG_TOUCH_TRANSFORM_F);

    File myFile = Flash.open(m_prop_file, FILE_WRITE);
    //myFile.println(String(prop.config)):
    myFile.println(String(A));
    myFile.println(String(B));
    myFile.println(String(C));
    myFile.println(String(D));
    myFile.println(String(E));
    myFile.println(String(F));
    myFile.close();
    return true;
  }
}

bool FT81xDisplay::isTouchCalibration() {
  return Flash.exists(m_prop_file);
}

void FT81xDisplay::forceTouchCalibration() {
  if (Flash.exists(m_prop_file)) {
    Flash.remove(m_prop_file);
  }
  this->doTouchCalibration();
}

void FT81xDisplay::_restoreTouchCalibration() {
  File myFile = Flash.open(m_prop_file);
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
  myReg->wr32(REG_TOUCH_TRANSFORM_A, A);
  myReg->wr32(REG_TOUCH_TRANSFORM_B, B);
  myReg->wr32(REG_TOUCH_TRANSFORM_C, C);
  myReg->wr32(REG_TOUCH_TRANSFORM_D, D);
  myReg->wr32(REG_TOUCH_TRANSFORM_E, E);
  myReg->wr32(REG_TOUCH_TRANSFORM_F, F);
}


void FT81xDisplay::removeTouchCalibration() {
  if (Flash.exists(m_prop_file)) {
    Flash.remove(m_prop_file);
  }
}

void FT81xDisplay::setTouchSensitivity(uint16_t sensitivity) {
  myReg->wr32(REG_TOUCH_RZTHRESH, sensitivity);
}


bool FT81xDisplay::isTouched() {
  uint16_t x, y;
  return isTouched(&x, &y);
}

bool FT81xDisplay::isTouched(uint16_t *x, uint16_t *y) {
  bool ret = !(myReg->rd16(REG_TOUCH_RAW_XY) & 0x8000);
  if (ret) {
    uint32_t xy  = myReg->rd32(REG_TOUCH_SCREEN_XY);
    *y = (xy) & 0x0000ffff;
    *x = (xy >> 16) & 0x0000ffff;
  } else {
    *x = *y = 0;
  }
  // Serial.println(String(*x) + ", " + String(*y));
  return ret;
}

uint8_t FT81xDisplay::_read_tag() {
  return myReg->rd8(REG_TOUCH_TAG);
}

uint32_t FT81xDisplay::_read_tracker_value(const uint8_t tag) {
  uint32_t result = myReg->rd32(REG_TRACKER);
  uint8_t rtag = result & 0x000000ff;
  uint16_t value = (result >> 16);
  if (rtag == tag)  return value;
  return 0;
}
