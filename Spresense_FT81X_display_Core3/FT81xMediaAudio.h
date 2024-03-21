#ifndef FT81XMEDIAAUDIO_HEADER_GUARD
#define FT81XMEDIAAUDIO_HEADER_GUARD

#include "FT81xMediaComponent.h"
#include "RegisterOperation.h"
#include "CommandOperation.h"

class FT81xDisplay;



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



#endif