#include "FT81xMediaMovie.h"
#include "FT81xDisplay.h"



/********************************************/
/*  FT81xMediaMovie Class                   */
/********************************************/

FT81xMediaMovie::~FT81xMediaMovie() {

}

FT81xMediaMovie::FT81xMediaMovie(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp, MediaOperation *mem, const uint32_t chunk)
 : FT81xMediaComponent(reg, cmd, disp, mem, chunk)  {
  //mAddress = 0;
  m_options = OPT_MEDIAFIFO;
}

void FT81xMediaMovie::draw() {  Serial.println("ERROR: FT81xMediaMovie not support draw() function."); }

void FT81xMediaMovie::setupMemory(uint32_t addr) {
  UNUSED(addr);
  myMem->setupMediaFifo();
}

void FT81xMediaMovie::play() {
  //myMem->resetMediaFifo(); 
  myCmd->cmd_start();
  myCmd->cmd(CMD_PLAYVIDEO);
  myCmd->cmd(m_options);
  myCmd->cmd_end();    
}

// play video on the fly : tentative implementation
void FT81xMediaMovie::setMovie(const uint8_t buf[], const uint32_t size) {
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

bool FT81xMediaMovie::play(File &file) {

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


