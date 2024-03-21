#include "FT81xMediaImage.h"
#include "FT81xDisplay.h"


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

