#include "FT81MediaAudio.h"


/********************************************/
/*  FT81xMediaAudio Class                   */
/********************************************/

FT81xMediaAudio::~FT81xMediaAudio() { }

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

