#include "FT81Core.h"
#include <Flash.h>



static uint32_t SPI_FREQ_LAUNCH = SPI5_FREQ_LAUNCH;
static uint32_t SPI_FREQ_RUNING = SPI5_FREQ_RUNING;
static uint8_t CS_PIN = SPI1_CS_PIN;
static uint8_t PDN_PIN = SPI1_PDN_PIN;

/********************************************/
/*  RegisterOperation class                 */
/********************************************/

RegisterOperation::RegisterOperation(SPIClass* spi) {
  mySpi = spi;
}

RegisterOperation::~RegisterOperation() {

}

uint8_t  RegisterOperation::rd8(const uint32_t  addr) {
  uint8_t value;
  digitalWrite(CS_PIN, LOW);
  mySpi->transfer(addr >> 16);
  mySpi->transfer(highByte(addr));
  mySpi->transfer(lowByte(addr));
  mySpi->transfer(0); //Dummy Read Byte
  value = mySpi->transfer(0);
  digitalWrite(CS_PIN, HIGH);
  return value;
}

uint16_t RegisterOperation::rd16(const uint32_t addr) {
  uint16_t value;
  uint16_t byte0;
  uint16_t byte1;
  
  digitalWrite(CS_PIN, LOW);
  mySpi->transfer(addr >> 16);
  mySpi->transfer(highByte(addr));
  mySpi->transfer(lowByte(addr));
  mySpi->transfer(0); //Dummy Read Byte
  byte0 = mySpi->transfer(0);
  byte1 = mySpi->transfer(0);
  value = (byte1 << 8) | byte0;
  digitalWrite(CS_PIN, HIGH);
  return value;
}

uint32_t RegisterOperation::rd32(const uint32_t addr) {
  uint32_t value;
  uint32_t byte0;
  uint32_t byte1;
  uint32_t byte2;
  uint32_t byte3;

  digitalWrite(CS_PIN, LOW);
  mySpi->transfer(addr >> 16);
  mySpi->transfer(highByte(addr));
  mySpi->transfer(lowByte(addr));
  mySpi->transfer(0); //Dummy Read Byte
  byte0 = mySpi->transfer(0);
  byte1 = mySpi->transfer(0);
  byte2 = mySpi->transfer(0);
  byte3 = mySpi->transfer(0);
  value = (byte3 << 24) | (byte2 << 16) | (byte1 << 8) | byte0;
  digitalWrite(CS_PIN, HIGH);
  return value;
}

void RegisterOperation::wr8(const uint32_t  addr, const uint8_t value) {
  digitalWrite(CS_PIN, LOW);
  mySpi->transfer(0x80 | (addr >> 16));
  mySpi->transfer(highByte(addr));
  mySpi->transfer(lowByte(addr));
  mySpi->transfer(value);
  digitalWrite(CS_PIN, HIGH);
}

void RegisterOperation::wr16(const uint32_t addr, const uint16_t value) {
  digitalWrite(CS_PIN, LOW);
  mySpi->transfer(0x80 | (addr >> 16));
  mySpi->transfer(highByte(addr));
  mySpi->transfer(lowByte(addr));
  mySpi->transfer(value & 0xFF);//LSB first
  mySpi->transfer((value >> 8) & 0xFF);
  digitalWrite(CS_PIN, HIGH);
}

void RegisterOperation::wr32(const uint32_t addr, const uint32_t value) {
  digitalWrite(CS_PIN, LOW);
  mySpi->transfer(0x80 | (addr >> 16));
  mySpi->transfer(highByte(addr));
  mySpi->transfer(lowByte(addr));
  mySpi->transfer(value & 0xFF);//LSB first
  mySpi->transfer((value >>  8) & 0xFF);  // 2Byte
  mySpi->transfer((value >> 16) & 0xFF);  // 3Byte
  mySpi->transfer((value >> 24) & 0xFF);  // 4Byte
  digitalWrite(CS_PIN, HIGH);
}

void RegisterOperation::host_command(const uint8_t cmd) {
  digitalWrite(CS_PIN, LOW);
  mySpi->transfer(cmd);
  mySpi->transfer(0);
  mySpi->transfer(0);
  digitalWrite(CS_PIN, HIGH);  
}


/********************************************/
/*  CommandOperation class                  */
/********************************************/

CommandOperation::CommandOperation(SPIClass *spi, RegisterOperation *reg) {
  mySpi = spi;
  CmdSPISettings = SPISettings(SPI_FREQ_RUNING, MSBFIRST, SPI_MODE0);
  myReg = reg;
}

CommandOperation::~CommandOperation() {

}


void CommandOperation::cmd_start() {
  waitForCmdFiFoEmpty();
  cmd_write_address = myReg->rd16(REG_CMD_WRITE);
  uint32_t addr = (RAM_CMD + cmd_write_address) | WRITE;
  mySpi->beginTransaction(CmdSPISettings);
  digitalWrite(CS_PIN, LOW);

  mySpi->transfer(addr >> 16);
  mySpi->transfer(addr >> 8);
  mySpi->transfer(addr);
}

void CommandOperation::cmd(const uint32_t cmd) {
  mySpi->transfer(cmd);
  mySpi->transfer(cmd >> 8);
  mySpi->transfer(cmd >> 16);
  mySpi->transfer(cmd >> 24);
  incCmdWriteAddress(4);  
}
void CommandOperation::cmd_end() {
  digitalWrite(CS_PIN, HIGH);
  mySpi->endTransaction();
  updateCmdWriteAddress();  
}

void CommandOperation::waitForCmdFiFoEmpty() {
  while(myReg->rd16(REG_CMD_READ) != myReg->rd16(REG_CMD_WRITE)) {
    #if 0
    static uint32_t last_cmd_write_address = cmd_write_address;
    cmd_write_address = rd16(REG_CMD_WRITE);
    if (cmd_write_address != last_cmd_write_address) {
      Serial.println("Wait for command fifo empty");
      last_cmd_write_address = cmd_write_address;
    }
    #else
      __asm__ volatile("nop");
    #endif
  }
}

void CommandOperation::checkCmdFiFoBuffer(const uint32_t count) {
  uint16_t getfreespace;
  do {
    getfreespace = checkCmdFiFoFreespace();
  } while (getfreespace < count); 
}

uint16_t CommandOperation::checkCmdFiFoFreespace() {
  uint16_t fullness = 0;
  uint16_t retval = 0;
  fullness = (cmd_write_address - myReg->rd16(REG_CMD_READ)) & 4095;
  retval = (FT_CMD_FIFO_SIZE - 4) - fullness;
  return retval;
}

void CommandOperation::waitCmdFiFoFreespace(uint32_t need_freespace) {
  uint16_t freespace;
  do {
    freespace = checkCmdFiFoFreespace(); 
  } while (freespace < need_freespace);
}

void CommandOperation::incCmdWriteAddress(uint16_t delta) {
  cmd_write_address = (cmd_write_address + delta) % 4096; 
  cmd_write_address = (cmd_write_address + 3) & 0xffc; // 4 byte alignment
}

void CommandOperation::updateCmdWriteAddress() {
  myReg->wr16(REG_CMD_WRITE, cmd_write_address);   
}



/********************************************/
/*  MemoryOperation class                  */
/********************************************/

MemoryOperation::MemoryOperation(SPIClass *spi, RegisterOperation *reg, CommandOperation *cmd) {
  mySpi = spi;
  CmdSPISettings = SPISettings(SPI_FREQ_RUNING, MSBFIRST, SPI_MODE0);
  myCmd = cmd;
  myReg = reg;
}


void MemoryOperation::memWrite(uint32_t address, uint8_t buf[], uint32_t size) {
  uint32_t addr = address | WRITE;
  mySpi->beginTransaction(CmdSPISettings);
  digitalWrite(CS_PIN, LOW);  
  mySpi->transfer((addr >> 16) & 0xFF);
  mySpi->transfer((addr >> 8) & 0xFF);
  mySpi->transfer(addr & 0xFF);
  mySpi->send(buf, size);
  digitalWrite(CS_PIN, HIGH);
  mySpi->endTransaction();  
}


/********************************************/
/*  MediaOperation class                  */
/********************************************/

MediaOperation::MediaOperation(SPIClass *spi, RegisterOperation *reg, CommandOperation *cmd, const uint32_t size, const uint32_t offset)
 : MemoryOperation(spi, reg, cmd) {
  // setup fifo buffer
  media_fifo_exists = false;
  media_fifo_size = size;
  media_fifo_base = RAM_G + offset - media_fifo_size;
  media_write_address = 0;
  playback_write_address = 0;
 }

void MediaOperation::setupMediaFifo() {
  if (media_fifo_exists == false) {
    myCmd->cmd_start();
    myCmd->cmd(CMD_MEDIAFIFO);
    myCmd->cmd(media_fifo_base);
    myCmd->cmd(media_fifo_size);
    myCmd->cmd_end();
    media_fifo_exists = true;
  }
}

void MediaOperation::setupPlaybackMemory(uint32_t addr) {
  playback_address = addr;
  playback_write_address = playback_address;
}

void MediaOperation::writeMediaFifo(uint8_t buf[], uint32_t size) {
  media_write_address = myReg->rd32(REG_MEDIAFIFO_WRITE);
  uint32_t address = media_fifo_base + media_write_address;

  memWrite(address, buf, size);

  //media_write_address += size;
  media_write_address = (media_write_address + size) & (media_fifo_size - 1);  
  myReg->wr32(REG_MEDIAFIFO_WRITE, media_write_address);
}

void MediaOperation::resetMediaFifo() {
  uint32_t address = getMediaFifoReadAddress();
  myReg->wr32(REG_MEDIAFIFO_WRITE, address);
}

void MediaOperation::writePlaybackMemory(uint8_t buf[], uint32_t size) {

  memWrite(playback_write_address, buf, size);

  playback_write_address += size;
}

uint32_t MediaOperation::getMediaFifoWriteAddress() {
  return  myReg->rd32(REG_MEDIAFIFO_WRITE);
}

uint32_t MediaOperation::getMediaFifoReadAddress() {
  return  myReg->rd32(REG_MEDIAFIFO_READ);  
}

uint32_t MediaOperation::getPlaybackReadAddress() {
  return  myReg->rd32(REG_PLAYBACK_READPTR);  
}


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

void FT81xDisplay::startDisplayList() {
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
  case FT81xGuage:
    return new FT81xComponentGuage(myReg, myCmd, this);
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
  case FT81xMJpeg:
    {
      if (myMem == NULL) {
        myMem = new MediaOperation(mySpi, myReg, myCmd);
      }    
      return new FT81xMediaMJpeg(myReg, myCmd, this, myMem);
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

void FT81xDisplay::swap() {
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

/********************************************/
/*  FT81xGfxCircle Class                    */
/********************************************/

FT81xGfxCircle::~FT81xGfxCircle() {

}

FT81xGfxCircle::FT81xGfxCircle(RegisterOperation* reg, CommandOperation* cmd, FT81xDisplay *disp)
 : FT81xGuiComponent(reg, cmd, disp) {
  mType = FT81xCircle;
  m_x = m_y = 0;
  m_r = 0;
  m_color = 0;
}

void FT81xGfxCircle::draw() {
  myCmd->cmd_start();
  myCmd->cmd(COLOR(m_color));
  myCmd->cmd(POINT_SIZE(m_r*16));
  myCmd->cmd(BEGIN(FTPOINTS));
  myCmd->cmd(VERTEX2F(m_x*16, m_y*16));
  myCmd->cmd(END());
  myCmd->cmd_end();
}


/********************************************/
/*  FT81xGfxRectangle Class                    */
/********************************************/

FT81xGfxRectangle::~FT81xGfxRectangle() {

}

FT81xGfxRectangle::FT81xGfxRectangle(RegisterOperation* reg, CommandOperation* cmd, FT81xDisplay *disp)
 : FT81xGuiComponent(reg, cmd, disp) {
  mType = FT81xRectangle;
  m_x = m_y = 0;
  m_w = m_h = 0;
  m_rad = 1;
  m_color = 0;
}

void FT81xGfxRectangle::draw() {
  myCmd->cmd_start();
  myCmd->cmd(COLOR(m_color));
  myCmd->cmd(LINE_WIDTH(m_rad*16));
  myCmd->cmd(BEGIN(RECTS));
  myCmd->cmd(VERTEX2F(m_x*16, m_y*16));
  myCmd->cmd(VERTEX2F((m_x + m_w)*16, (m_y + m_h)*16));
  myCmd->cmd(END());
  myCmd->cmd_end();
}


/********************************************/
/*  FT81xGfxTriangle Class                    */
/********************************************/

FT81xGfxTriangle::~FT81xGfxTriangle() {

}

FT81xGfxTriangle::FT81xGfxTriangle(RegisterOperation* reg, CommandOperation* cmd, FT81xDisplay *disp)
 : FT81xGuiComponent(reg, cmd, disp) {
  mType = FT81xTriangle;
  m_x1 = m_y1 = 0;
  m_x2 = m_y2 = 0;
  m_x3 = m_y3 = 0;    
  m_fgcolor = 0;
  m_bgcolor = 0;
}

void FT81xGfxTriangle::setPoints(const uint16_t x1, const uint16_t y1, const uint16_t x2, const uint16_t y2, const uint16_t x3, const uint16_t y3) {
  if ((y1 <= y2) && (y1 <= y3)) {  // point 1 is highest on screen
    m_x1 = x3; m_y1 = y3;
    m_x2 = x1; m_y2 = y1;
    m_x3 = x2; m_y3 = y2;
  } else if ((y2 <= y3) && (y2 <= y1)) {  // point 2 is highest
    m_x1 = x1; m_y1 = y1;
    m_x2 = x2; m_y2 = y2;
    m_x3 = x3; m_y3 = y3;
  } else {  // point 3 highest
    m_x1 = x2; m_y1 = y2;
    m_x2 = x3; m_y2 = y3;
    m_x3 = x1; m_y3 = y1;
  }
}

void FT81xGfxTriangle::draw() {
  m_bgcolor = myDisp->getBgColor();
  myCmd->cmd_start();
  if (m_x2 <= m_x1) {  // one colour wipe (2-3), two bg wipes
    myCmd->cmd(COLOR(m_fgcolor));
    myCmd->cmd(LINE_WIDTH(1*16));
    myCmd->cmd(BEGIN(EDGE_STRIP_B));
    myCmd->cmd(VERTEX2F(m_x2*16, m_y2*16));
    myCmd->cmd(VERTEX2F(m_x3*16, m_y3*16));
    myCmd->cmd(COLOR(m_bgcolor));
    myCmd->cmd(LINE_WIDTH(1*16));
    myCmd->cmd(BEGIN(EDGE_STRIP_B));
    myCmd->cmd(VERTEX2F(m_x3*16, m_y3*16));
    myCmd->cmd(VERTEX2F(m_x1*16, m_y1*16));
    myCmd->cmd(VERTEX2F(m_x2*16, m_y2*16));
  } else if (m_x2 >= m_x3) {  // one colour wipe (1-2), two bg wipes
    myCmd->cmd(COLOR(m_fgcolor));
    myCmd->cmd(LINE_WIDTH(1*16));
    myCmd->cmd(BEGIN(EDGE_STRIP_B));
    myCmd->cmd(VERTEX2F(m_x1*16, m_y1*16));
    myCmd->cmd(VERTEX2F(m_x2*16, m_y2*16));
    myCmd->cmd(COLOR(m_bgcolor));
    myCmd->cmd(LINE_WIDTH(1*16));
    myCmd->cmd(BEGIN(EDGE_STRIP_B));
    myCmd->cmd(VERTEX2F(m_x2*16, m_y2*16));
    myCmd->cmd(VERTEX2F(m_x3*16, m_y3*16));
    myCmd->cmd(VERTEX2F(m_x1*16, m_y1*16));
  } else {  // two colour wipes, one bg wipe
    myCmd->cmd(COLOR(m_fgcolor));
    myCmd->cmd(LINE_WIDTH(1*16));
    myCmd->cmd(BEGIN(EDGE_STRIP_B));
    myCmd->cmd(VERTEX2F(m_x1*16, m_y1*16));
    myCmd->cmd(VERTEX2F(m_x2*16, m_y2*16));
    myCmd->cmd(VERTEX2F(m_x3*16, m_y3*16));
    myCmd->cmd(COLOR(m_bgcolor));
    myCmd->cmd(LINE_WIDTH(1*16));
    myCmd->cmd(BEGIN(EDGE_STRIP_B));
    myCmd->cmd(VERTEX2F(m_x3*16, m_y3*16));
    myCmd->cmd(VERTEX2F(m_x1*16, m_y1*16));
  }
  myCmd->cmd(END());
  myCmd->cmd_end();
}


/********************************************/
/*  FT81xGfxLine Class                    */
/********************************************/

FT81xGfxLine::~FT81xGfxLine() {

}

FT81xGfxLine::FT81xGfxLine(RegisterOperation* reg, CommandOperation* cmd, FT81xDisplay *disp)
 : FT81xGuiComponent(reg, cmd, disp) {
  mType = FT81xLine;
  m_x1 = m_y1 = 0;
  m_x2 = m_y2 = 0;
  m_w = 0;
  m_color = 0;
}

void FT81xGfxLine::draw() {
  myCmd->cmd_start();
  myCmd->cmd(COLOR(m_color));
  myCmd->cmd(LINE_WIDTH(m_w*16));
  myCmd->cmd(BEGIN(LINES));
  myCmd->cmd(VERTEX2F(m_x1*16, m_y1*16));
  myCmd->cmd(VERTEX2F(m_x2*16, m_y2*16));
  myCmd->cmd(END());
  myCmd->cmd_end();
}


/********************************************/
/*  FT81xGfxGradient Class                  */
/********************************************/

FT81xGfxGradient::~FT81xGfxGradient() {

}

FT81xGfxGradient::FT81xGfxGradient(RegisterOperation* reg, CommandOperation* cmd, FT81xDisplay *disp)
 : FT81xGuiComponent(reg, cmd, disp) {
  mType = FT81xGradient;
  m_x = m_y = 0;
  m_w = m_h = 0;
  m_color1 = 0;
  m_color2 = 0;
}

void FT81xGfxGradient::draw() {
  myCmd->cmd_start();
  myCmd->cmd(SCISSOR_XY(m_x, m_y));
  myCmd->cmd(SCISSOR_SIZE(m_w, m_h));
  myCmd->cmd(CMD_GRADIENT);
  myCmd->cmd(m_x | ((uint32_t)(m_y) << 16));
  myCmd->cmd(m_color1);
  myCmd->cmd((m_x + m_w) | ((uint32_t)(m_y + m_h) << 16));
  myCmd->cmd(m_color2);
  myCmd->cmd(SCISSOR_XY(0, 0));
  uint16_t w = myDisp->getDisplayWidth();
  uint16_t h = myDisp->getDisplayHeight();
  myCmd->cmd(SCISSOR_SIZE(w, h));  
  myCmd->cmd_end();
}



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



/********************************************/
/*  FT81xComponentClock Class               */
/********************************************/

FT81xComponentClock::~FT81xComponentClock() {

}

FT81xComponentClock::FT81xComponentClock(RegisterOperation* reg, CommandOperation* cmd, FT81xDisplay *disp)
 : FT81xGuiComponent(reg, cmd, disp) {
  mType = FT81xClock;
  m_hour = m_min = m_sec = 0;
  m_x = m_y = 0;
  m_rad = 0;
  m_fgcolor = 0;
  m_bgcolor = 0;
}

void FT81xComponentClock::draw() {
  myCmd->cmd_start();
  myCmd->cmd(COLOR(m_fgcolor));
  myCmd->cmd(CMD_BGCOLOR);
  myCmd->cmd(m_bgcolor);
  myCmd->cmd(CMD_CLOCK);
  myCmd->cmd(m_x | ((uint32_t)(m_y) << 16));
  myCmd->cmd(m_rad | ((uint32_t)(m_options) << 16));
  myCmd->cmd(m_hour | ((uint32_t)(m_min) << 16));
  myCmd->cmd(m_sec);
  myCmd->cmd_end();
}

void FT81xComponentClock::draw(const uint8_t hour, const uint8_t min, const uint8_t sec) {
  myCmd->cmd_start();
  myCmd->cmd(COLOR(m_fgcolor));
  myCmd->cmd(CMD_BGCOLOR);
  myCmd->cmd(m_bgcolor);
  myCmd->cmd(CMD_CLOCK);
  myCmd->cmd(m_x | ((uint32_t)(m_y) << 16));
  myCmd->cmd(m_rad | ((uint32_t)(m_options) << 16));
  myCmd->cmd(hour | ((uint32_t)(min) << 16));
  myCmd->cmd(sec);
  myCmd->cmd_end();
}



/********************************************/
/*  FT81xComponentGuage Class               */
/********************************************/

FT81xComponentGuage::~FT81xComponentGuage() {

}

FT81xComponentGuage::FT81xComponentGuage(RegisterOperation* reg, CommandOperation* cmd, FT81xDisplay *disp)
 : FT81xGuiComponent(reg, cmd, disp) {
  mType = FT81xGuage;
  m_major = m_minor = m_range = 0;
  m_x = m_y = 0;
  m_value = 0;
  m_rad = 0;
  m_fgcolor = 0;
  m_bgcolor = 0;
  m_ncolor = 0;
  m_options = OPT_3D;
}

void FT81xComponentGuage::draw() {
  myCmd->cmd_start();
  myCmd->cmd(COLOR(m_fgcolor));
  myCmd->cmd(CMD_BGCOLOR);
  myCmd->cmd(m_bgcolor);
  myCmd->cmd(CMD_GAUGE);
  myCmd->cmd(m_x | ((uint32_t)(m_y) << 16));
  myCmd->cmd(m_rad | (uint32_t)(OPT_NOPOINTER) << 16);
  myCmd->cmd(m_major | ((uint32_t)(m_minor) << 16));
  myCmd->cmd(m_value | ((uint32_t)(m_range) << 16));
  myCmd->cmd_end();

  myCmd->cmd_start();
  myCmd->cmd(COLOR(m_ncolor));
  myCmd->cmd(CMD_BGCOLOR);
  myCmd->cmd(m_bgcolor);  
  myCmd->cmd(CMD_GAUGE);
  myCmd->cmd(m_x | (uint32_t)(m_y) << 16);
  myCmd->cmd(m_rad | (uint32_t)(OPT_NOBACK | OPT_NOTICKS) << 16);
  myCmd->cmd(m_major | (uint32_t)(m_minor) << 16);
  myCmd->cmd(m_value | (uint32_t)(m_range) << 16);
  myCmd->cmd_end();

}

void FT81xComponentGuage::draw(const uint16_t value) {
  m_value = value;
  draw();
}




/********************************************/
/*  FT81xComponentProgressbar Class               */
/********************************************/

FT81xComponentProgressbar::~FT81xComponentProgressbar() {

}

FT81xComponentProgressbar::FT81xComponentProgressbar(RegisterOperation* reg, CommandOperation* cmd, FT81xDisplay *disp)
 : FT81xGuiComponent(reg, cmd, disp) {
  mType = FT81xProgressbar;
  m_range = 0;
  m_x = m_y = 0;
  m_value = 0;
  m_fgcolor = 0;
  m_bgcolor = 0;
  m_options = OPT_3D;
}

void FT81xComponentProgressbar::draw() {
  myCmd->cmd_start();
  myCmd->cmd(COLOR(m_fgcolor));
  myCmd->cmd(CMD_BGCOLOR);
  myCmd->cmd(m_bgcolor);
  myCmd->cmd(CMD_PROGRESS);
  myCmd->cmd(m_x  | ((uint32_t)(m_y) << 16));
  myCmd->cmd(m_w | ((uint32_t)(m_h) << 16));
  myCmd->cmd(m_options | ((uint32_t)(m_value) << 16));
  myCmd->cmd(m_range);
  myCmd->cmd_end();
}

void FT81xComponentProgressbar::draw(const uint16_t value) {
  m_value = value;
  draw();
}


/********************************************/
/*  FT81xComponentSpinner Class                  */
/********************************************/

FT81xComponentSpinner::~FT81xComponentSpinner() {

}

FT81xComponentSpinner::FT81xComponentSpinner(RegisterOperation* reg, CommandOperation* cmd, FT81xDisplay *disp)
 : FT81xGuiComponent(reg, cmd, disp) {
  mType = FT81xSpinner;
  m_x = m_y = 0;
  m_w = m_h = 0;
  m_options = OPT_3D;
  m_style = dot_circle;
  m_scale = 0;
  m_color = 0;
}

void FT81xComponentSpinner::draw() {
  myCmd->cmd_start();
  myCmd->cmd(COLOR(m_color));  
  myCmd->cmd(CMD_SPINNER);
  myCmd->cmd(m_x | ((uint32_t)(m_y) << 16));
  myCmd->cmd(m_style | ((uint32_t)(m_scale) << 16));
  myCmd->cmd_end();
}


/********************************************/
/*  FT81xInteractionComponent Class            */
/********************************************/  

FT81xInteractionComponent::FT81xInteractionComponent(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp)
 : FT81xGuiComponent(reg, cmd, disp) {
  mTag = 0;
  m_func = NULL;
}

FT81xInteractionComponent::~FT81xInteractionComponent() {
  myDisp->releaseTag(mTag);
}

bool FT81xInteractionComponent::setTag(uint8_t tag) {
  // TODO: need to think about avoiding confliction tags of keys.
  mTag = tag;
  myDisp->setTag(mTag, this);
  return true;
}

void FT81xInteractionComponent::releaseTag(uint8_t tag) {
  myDisp->releaseTag(mTag);
}

/********************************************/
/*  FT81xInteractionButton Class            */
/********************************************/  

FT81xInteractionButton::~FT81xInteractionButton() {
  if (m_text != NULL) {
    delete m_text;
    m_text = NULL;
  }
}

FT81xInteractionButton::FT81xInteractionButton(RegisterOperation* reg, CommandOperation* cmd, FT81xDisplay *disp)
 : FT81xInteractionComponent(reg, cmd, disp) {
  m_x = m_y = 0;
  m_w = m_h = 0;
  m_font = 20;
  m_color = COLOR_RGB(255, 255, 255);
  m_bgcolor = 0;
  m_options = OPT_3D;
  m_pushed = 0;
  m_text = NULL;
}

void FT81xInteractionButton::draw() {
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

void FT81xInteractionButton::setFont(const uint8_t font) {
  if (font > 31) {
    m_font = 31;
  } else {
    m_font = font;
  }
}

void FT81xInteractionButton::setText(const char text[]) {
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

void FT81xInteractionButton::doInteraction_(const uint32_t tag) {
  m_pushed = true;
  if (m_func != NULL) {
    m_func(tag);
  }
}

bool FT81xInteractionButton::pushed() {
  bool value = m_pushed;
  m_pushed = false; // once the value is read, reset the value;
  return value; 
}

/********************************************/
/*  FT81xInteractionKeys Class            */
/********************************************/  

FT81xInteractionKeys::~FT81xInteractionKeys() {
  if (m_keymap != NULL) {
    uint16_t length = strlen(m_keymap);
    for (int n = 0; n < length; ++n) {
      this->releaseTag(m_keymap[n]);
    }    
    delete m_keymap;
    m_keymap = NULL;
  }
}

FT81xInteractionKeys::FT81xInteractionKeys(RegisterOperation* reg, CommandOperation* cmd, FT81xDisplay *disp)
 : FT81xInteractionComponent(reg, cmd, disp) {
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

void FT81xInteractionKeys::draw() {
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

void FT81xInteractionKeys::setFont(const uint8_t font) {
  if (font > 31) {
    m_font = 31;
  } else {
    m_font = font;
  }
}

void FT81xInteractionKeys::setKeyMap(const char keymap[]) {
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

void FT81xInteractionKeys::doInteraction_(const uint32_t tag) {
  m_key = (char)tag;
  Serial.println("Keys::doInteruction_ is called");
  if (m_func != NULL) {
    m_func(tag);
  }
}

char FT81xInteractionKeys::readKey() {
  char value = m_key;
  m_key = 0;
  return value;
}


/********************************************/
/*  FT81xInteractionDial Class            */
/********************************************/  

FT81xInteractionDial::~FT81xInteractionDial() {
  
}

FT81xInteractionDial::FT81xInteractionDial(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp)
 : FT81xInteractionComponent(reg, cmd, disp) {
  m_x = m_y = 0;
  m_rad = 0;
  m_color = COLOR_RGB(255, 255, 255);
  m_bgcolor = COLOR_RGB(0, 0, 0);
  m_options = OPT_3D;
  m_value = 0;
}

void FT81xInteractionDial::draw() {
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


void FT81xInteractionDial::doInteraction_(const uint32_t value) {
  m_value = value;
  m_normValue = (value*100)/(0xffff);
  Serial.println("Dial::doInteruction_ is called");
  if (m_func != NULL) {
    m_func(m_normValue);
  }
}


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

/********************************************/
/*  FT81xInteractionToggle Class            */
/********************************************/  

FT81xInteractionToggle::~FT81xInteractionToggle() {
  if (m_text != NULL) {
    delete m_text;
    m_text = NULL;
  }
}

FT81xInteractionToggle::FT81xInteractionToggle(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp)
 : FT81xInteractionComponent(reg, cmd, disp) {
  m_x = m_y = 0;
  m_w = 0;
  m_font = 20;
  m_color = COLOR_RGB(255, 255, 255);
  m_bgcolor = COLOR_RGB(0, 0, 0);
  m_options = OPT_3D;
  m_state = 0;
  m_text = NULL;
}

void FT81xInteractionToggle::setFont(const uint8_t font) {
  if (font > 31) {
    m_font = 31;
  } else {
    m_font = font;
  }
}

void FT81xInteractionToggle::setText(const char text0[], const char text1[]) {
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


void FT81xInteractionToggle::draw() {
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

void FT81xInteractionToggle::doInteraction_(const uint32_t value) {
  static bool state = false;
  state = state ? false : true; 
  if (state) m_state = 0xffff;
  else m_state = 0x0000;
  Serial.println("Toggle::doInteruction_ is called");
  if (m_func != NULL) {
    m_func(state);
  }  
}



/********************************************/
/*  FT81xInteractionScrollbar Class            */
/********************************************/  

FT81xInteractionScrollbar::FT81xInteractionScrollbar(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp)
 : FT81xInteractionComponent(reg, cmd, disp) {
  m_x = m_y = 0;
  m_w = m_h = 0;
  m_color = COLOR_RGB(255, 255, 255);
  m_bgcolor = COLOR_RGB(0, 0, 0);
  m_options = OPT_3D;
  m_value = 0;
  m_size = 30;
  m_range = 100;
}

void FT81xInteractionScrollbar::draw() {
  myCmd->cmd_start();
  myCmd->cmd(TAG_MASK(1));
  myCmd->cmd(TAG(mTag));    
  myCmd->cmd(CMD_FGCOLOR);
  myCmd->cmd(m_color);
  myCmd->cmd(CMD_BGCOLOR);
  myCmd->cmd(m_bgcolor);
  myCmd->cmd(CMD_SCROLLBAR);
  myCmd->cmd(m_x | ((uint32_t)(m_y) << 16));
  myCmd->cmd(m_w | ((uint32_t)(m_h) << 16));
  myCmd->cmd(m_options | ((uint32_t)(m_value) << 16));
  myCmd->cmd(m_size | ((uint32_t)(m_range) << 16));
  myCmd->cmd(TAG_MASK(0));
  myCmd->cmd_end();

  myCmd->cmd_start();
  myCmd->cmd(CMD_TRACK);
  myCmd->cmd(m_x | ((uint32_t)(m_y) << 16));
  myCmd->cmd(m_w | ((uint32_t)(m_h) << 16));
  myCmd->cmd(mTag);
  myCmd->cmd_end();
}

void FT81xInteractionScrollbar::doInteraction_(const uint32_t value) {
  m_value = (uint16_t)(value*m_range/0xffff);
  Serial.println("Scrollbar::doInteruction_ is called " + String(m_value) + "(" + String(value) + ")") ;
  if (m_func != NULL) {
    m_func(value);
  }  
}



/********************************************/
/*  FT81xMediaCompoennt Class               */
/********************************************/


FT81xMediaComponent::~FT81xMediaComponent() {
  if (mTransBuf != NULL) {
    memset(mTransBuf, NULL, mChunk*sizeof(uint8_t));
    free(mTransBuf);
    mTransBuf = NULL;
  }
}

FT81xMediaComponent::FT81xMediaComponent(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp, MediaOperation *mem, const uint32_t chunk)
 : FT81xGuiComponent(reg, cmd, disp) {
  myMem = mem;
  mChunk = chunk;
  if (chunk > 0) {
    mTransBuf = (uint8_t*)malloc(mChunk*sizeof(uint8_t));
    if (mTransBuf != NULL) {
      memset(mTransBuf, 0, mChunk*sizeof(uint8_t));
    }
  } else {
    mTransBuf = NULL;
  }
 }



/********************************************/
/*  FT81xMediaImage Class                   */
/********************************************/

FT81xMediaImage::~FT81xMediaImage() {
  // myDisp->releaseHandle(mHandle);
}

FT81xMediaImage::FT81xMediaImage(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp,  MediaOperation *mem, const uint32_t chunk) 
 : FT81xMediaComponent(reg, cmd, disp, mem, chunk)  {
  mAddress = 0;
  mHandle = 0;
  m_x = m_y = 0;
  m_w = m_h = 0;
  m_scale = 1;
  m_rot = 0;
} 

void FT81xMediaImage::draw() {
  myCmd->cmd_start();
  myCmd->cmd(BITMAP_HANDLE(mHandle));
  myCmd->cmd(BITMAP_SOURCE(RAM_G + mAddress));
  myCmd->cmd(BITMAP_LAYOUT(RGB565, m_w*2, m_h));
  myCmd->cmd(BITMAP_SIZE(NEAREST, BORDER, BORDER, m_w, m_h));
  myCmd->cmd(BEGIN(BITMAPS));
  myCmd->cmd(CMD_LOADIDENTITY);
  myCmd->cmd(CMD_SCALE);
  myCmd->cmd(m_scale*65536);
  myCmd->cmd(m_scale*65536);
  myCmd->cmd(CMD_TRANSLATE);
  myCmd->cmd(65536*(m_w/2));
  myCmd->cmd(65536*(m_h/2));
  myCmd->cmd(CMD_ROTATE);
  myCmd->cmd(m_rot*65536/360);
  myCmd->cmd(CMD_TRANSLATE);
  myCmd->cmd(65536*(-m_w/2));
  myCmd->cmd(65536*(-m_h/2));
  myCmd->cmd(CMD_SETMATRIX);
  myCmd->cmd(VERTEX2II(m_x, m_y, mHandle, 0));
  myCmd->cmd(DISPLAY());
  myCmd->cmd(END());
  myCmd->cmd_end();
}

void  FT81xMediaImage::setupMemory(uint32_t addr) {
  mAddress = addr;
  myMem->setupMediaFifo();
  myCmd->cmd_start();
  myCmd->cmd(CMD_LOADIMAGE);
  myCmd->cmd(RAM_G + mAddress);
  myCmd->cmd(OPT_NODL | OPT_MEDIAFIFO);
  myCmd->cmd_end();
}

void FT81xMediaImage::setImage(const uint8_t buf[], const uint32_t size) {
  uint32_t chunk = this->getChunkSize();
  uint8_t *trans_buf = this->getTransBuf();

  uint8_t *ptr = (uint8_t*)buf;
  int32_t length = size;
  length = (length + 3) & ~3;  // force 32bit alignment    
  while (length > 0) {
    uint16_t trans_size = min(chunk, length);
    memset(trans_buf, 0, chunk*sizeof(uint8_t));
    memcpy(trans_buf, ptr, trans_size);
    myMem->writeMediaFifo(trans_buf, chunk);

    ptr += trans_size;
    length -= (int32_t)trans_size;
  }
}

bool FT81xMediaImage::setImage(File &file) {
  uint32_t image_size = file.size();
  if (image_size == 0) {
    Serial.println("ERROR: Image File is 0 byte");
    return false;
  }
  Serial.println("image size = " + String(image_size));

  static const uint32_t buf_size = 1024;
  uint8_t *part_image = (uint8_t*)malloc(buf_size*sizeof(uint8_t));
  memset(part_image, 0, buf_size*sizeof(uint8_t));

  int32_t length = (int32_t)image_size;
  while (length > 0) {
    uint32_t read_size = min(buf_size, length);
    if (file.read(part_image, read_size) < 0) {
      Serial.println("End of Image File");
      break;
    } 

    this->setImage(part_image, read_size);

    length -= (int32_t)read_size;
  }

  Serial.println("finsh storing image");
  file.close();

  // cleanup memory
  memset(part_image, NULL, buf_size*sizeof(uint8_t));
  free(part_image); 
  part_image = NULL;

  return true;
}


/********************************************/
/*  FT81xMediaSynth Class                   */
/********************************************/

FT81xMediaSynth::FT81xMediaSynth(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp, MediaOperation *mem, const uint32_t chunk)
 : FT81xMediaComponent(reg, cmd, disp, mem, chunk)  {
  m_volume = 128;
  m_note = NOTE_C1;
  m_effect = BEEP;
}
 
void FT81xMediaSynth::draw() {
 this->play();
}

void FT81xMediaSynth::play() {
  myReg->wr8(REG_VOL_SOUND, m_volume);
  myReg->wr16(REG_SOUND, ((uint16_t)(m_note) << 8) | m_effect);
  myReg->wr8(REG_PLAY, 1);
}

void FT81xMediaSynth::stop() {
  myReg->wr8(REG_VOL_SOUND, 0);
  myReg->wr16(REG_SOUND, 0);
  myReg->wr8(REG_PLAY, 1);
}  

void FT81xMediaSynth::setVolume(uint8_t vol) {
  if (vol > 255) vol = 255;
  m_volume = vol; 
}

bool FT81xMediaSynth::setNote(uint8_t note) {
  if (note < NOTE_A0 || note > NOTE_C8) return false;
  if ((m_effect >= SQURE_WAVE && m_effect <= CAROUSEL)
   || (m_effect >= PIP01 && m_effect <= PIP16)
   || (m_effect >= HARP && m_effect <= BELL)
  ) {
    m_note = note;
    return true;
  }
  return false;
}

bool FT81xMediaSynth::setEffect(uint8_t effect) {
  if ((effect > CAROUSEL && effect < PIP01)
   || (effect > PIP16 && effect < DTMFA)
   || (effect > DTMFB && effect < DTMF0)   
   || (effect > DTMF9 && effect < HARP)  
   || (effect > BELL && effect < CLICK)  
   || (effect > CHACK && effect < MUTE)  
   || (effect > UNMUTE)         
  ) {
    return false;
  }
  m_effect = effect;
  return true;
}

bool FT81xMediaSynth::isContinuous(uint8_t effect) {
  if ((effect >= SILENCE && effect <= CAROUSEL)
   || (effect >= DTMFA && effect <= DTMFB)
   || (effect >= DTMF0 && effect <= DTMF9)
  ) {
    return true;
  }
  return false;
}
 


/********************************************/
/*  FT81xMediaAudio Class                   */
/********************************************/

FT81xMediaAudio::~FT81xMediaAudio() {

}

FT81xMediaAudio::FT81xMediaAudio(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp, MediaOperation *mem, const uint32_t chunk)
 : FT81xMediaComponent(reg, cmd, disp, mem, chunk) {
  mAddress = 0;
  m_audio_size = 0;
  m_volume = 128;
  m_sampling_rate = 8000;
  m_format = ULAW_SAMPLES;
  m_loop = false;
  setupMemory(mAddress);
}

void FT81xMediaAudio::draw() {
  this->play();
}

void FT81xMediaAudio::setupMemory(uint32_t addr) {
  mAddress = RAM_G + addr;
  myMem->setupPlaybackMemory(mAddress);
}

void FT81xMediaAudio::play() {
  myReg->wr8(REG_VOL_PB, m_volume);
  myReg->wr32(REG_PLAYBACK_START, mAddress);
  myReg->wr32(REG_PLAYBACK_LENGTH, m_audio_size);
  myReg->wr16(REG_PLAYBACK_FREQ, m_sampling_rate);
  myReg->wr8(REG_PLAYBACK_FORMAT, m_format);
  myReg->wr8(REG_PLAYBACK_LOOP, m_loop);
  myReg->wr8(REG_PLAYBACK_PLAY, 1);
}

void FT81xMediaAudio::stop() {
  myReg->wr32(REG_PLAYBACK_LENGTH, 0);
  myReg->wr8(REG_PLAYBACK_PLAY, 1);
}

void FT81xMediaAudio::setAudio(uint8_t buf[], uint32_t size) {
  uint32_t chunk = this->getChunkSize();
  uint8_t *trans_buf = this->getTransBuf();

  uint8_t *ptr = (uint8_t*)buf;
  int32_t length = (int32_t)size;
  while (length > 0) {
 
    uint32_t write_address = myMem->getPlaybackWriteAddress();
    Serial.println("write_address: " + String(write_address));
    /*
    uint32_t diff = write_address - read_address;
    if (diff < chunk) continue; // waiting for trasfering data to memory
    */
    uint16_t trans_size = min(chunk, length);
    memset(trans_buf, 0, chunk*sizeof(uint8_t));
    memcpy(trans_buf, ptr, trans_size);
    uint16_t aligned_trans_size = (trans_size + 3) & ~3;  // force 32bit alignment

    myMem->writePlaybackMemory(trans_buf, aligned_trans_size);

    ptr += trans_size;
    length -= (int32_t)trans_size;
  }
}

bool FT81xMediaAudio::setAudio(File &file) {
  // set write address to the initial address
  myMem->resetPlaybackMemory();

  uint32_t audio_size = file.size();
  if (audio_size == 0) {
    Serial.println("ERROR: Audio File is 0 byte");
    return false; 
  }
  Serial.println("audio size = " + String(audio_size));
  m_audio_size = audio_size;

  static const uint32_t buf_size = 1024;
  uint8_t *part_audio = (uint8_t*)malloc(buf_size*sizeof(uint8_t));
  memset(part_audio, 0, buf_size*sizeof(uint8_t));

  int32_t length = (int32_t)audio_size;
  while (length > 0) {
    uint32_t read_size = min(buf_size, length);
    if (file.read(part_audio, read_size) < 0) {
      Serial.println("End of Audio File");
      break;
    } 

    this->setAudio(part_audio, read_size);

    length -= (int32_t)read_size;
  }

  Serial.println("finsh storing audio");
  file.close();

  // cleanup memory
  memset(part_audio, NULL, buf_size*sizeof(uint8_t));
  free(part_audio); 
  part_audio = NULL;

  return true;  
}

bool FT81xMediaAudio::setFormat(uint8_t format) {
  if (format != LINEAR_SAMPLES || format != ULAW_SAMPLES || format != ADPCM_SAMPLES)
    return false;
  m_format = format;
  return true;
}

bool FT81xMediaAudio::setSamplingRate(uint16_t sampling_rate) {
  // 8kHz - 48kHz
  if (sampling_rate < 8000 || sampling_rate > 48000) return false;
  m_sampling_rate = sampling_rate;
  return true;
} 
void FT81xMediaAudio::setVolume(uint8_t vol) {
  if (vol > 255) vol = 255;
  m_volume = vol;
}



/********************************************/
/*  FT81xMediaMJpeg Class               */
/********************************************/

FT81xMediaMJpeg::~FT81xMediaMJpeg() {

}

FT81xMediaMJpeg::FT81xMediaMJpeg(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp, MediaOperation *mem, const uint32_t chunk)
 : FT81xMediaComponent(reg, cmd, disp, mem, chunk)  {
  //mAddress = 0;
  m_options = OPT_MEDIAFIFO;
}

void FT81xMediaMJpeg::draw() {  Serial.println("ERROR: FT81xMediaMJpeg not support draw() function."); }

void FT81xMediaMJpeg::setupMemory(uint32_t addr) {
  UNUSED(addr);
  myMem->setupMediaFifo();
}

void FT81xMediaMJpeg::play() {
  //myMem->resetMediaFifo(); 
  myCmd->cmd_start();
  myCmd->cmd(CMD_PLAYVIDEO);
  myCmd->cmd(m_options);
  myCmd->cmd_end();    
}

// play video on the fly : tentative implementation
void FT81xMediaMJpeg::setMJpeg(const uint8_t buf[], const uint32_t size) {
  uint32_t chunk = this->getChunkSize();
  uint8_t *trans_buf = this->getTransBuf();
  uint32_t media_fifo_size = myMem->getMediaFifoSize();

  uint8_t *ptr = (uint8_t*)buf;
  int32_t length = (int32_t)size;
  length = (length + 3) & ~3;  // force 32bit alignment  
  while (length > 0) {

    // check fifo fullness
    uint32_t media_write_address = myMem->getMediaFifoWriteAddress();
    uint32_t media_read_address  = myMem->getMediaFifoReadAddress();
    uint32_t fifo_fullness = (media_write_address - media_read_address) & (media_fifo_size - 1);

    while (fifo_fullness < media_fifo_size - chunk) {
      uint32_t trans_size = min(chunk, length);
      memset(trans_buf, 0, chunk*sizeof(uint8_t));
      memcpy(trans_buf, ptr, trans_size);

      myMem->writeMediaFifo(trans_buf, chunk);

      ptr += trans_size;
      length -= (int32_t)trans_size;
      if (length <= 0) break;

      // recheck fifo fullness whether movie can be transferred
      media_write_address = myMem->getMediaFifoWriteAddress();
      media_read_address  = myMem->getMediaFifoReadAddress();
      fifo_fullness = (media_write_address - media_read_address) & (media_fifo_size - 1); 
    }
  }  
}

bool FT81xMediaMJpeg::play(File &file) {

  this->play();

  uint32_t chunk = this->getChunkSize();
  uint8_t *trans_buf = this->getTransBuf();
  memset(trans_buf, 0, chunk*sizeof(uint8_t));

  uint32_t length = file.size();

  uint32_t media_fifo_size = myMem->getMediaFifoSize();
  uint32_t total_transfered_data = 0;
  while (total_transfered_data < length) {
    // check fifo fullness
    uint32_t media_write_address = myMem->getMediaFifoWriteAddress();
    uint32_t media_read_address  = myMem->getMediaFifoReadAddress();
    uint32_t fifo_fullness = (media_write_address - media_read_address) & (media_fifo_size - 1);
    // Serial.println("media_write_address: " + String(media_write_address));
    // Serial.println("media_read_address : " + String(media_read_address));
    // Serial.println("fifo_fullness      : " + String(fifo_fullness));

    while (fifo_fullness < media_fifo_size - chunk) {

      // write a chunk of AVI data to the media fifo
      memset(trans_buf, 0, chunk*sizeof(uint8_t));
      int read_size = file.read(trans_buf, chunk);
      if (read_size < 0) {
        Serial.println("End of Movie file");
        break;
      }

      uint32_t trans_size = min(chunk, read_size);
      myMem->writeMediaFifo(trans_buf, chunk);

      // recheck fifo fullness whether movie can be transferred
      media_write_address = myMem->getMediaFifoWriteAddress();
      media_read_address  = myMem->getMediaFifoReadAddress();
      fifo_fullness = (media_write_address - media_read_address) & (media_fifo_size - 1);
      total_transfered_data += trans_size;
      if (total_transfered_data >= length) break;
    } 
  } 
  Serial.println("finsh playing movie");

  return true;
}


