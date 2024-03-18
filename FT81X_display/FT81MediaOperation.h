#ifndef FT81MEDIAOPERATION_HEADER_GUARD
#define FT81MEDIAOPERATION_HEADER_GUARD

#include "FT81Core.h"

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

#endif // FT81MEDIAOPERATION_HEADER_GUARD

