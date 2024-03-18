#include "FT81MediaOperation.h"


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

