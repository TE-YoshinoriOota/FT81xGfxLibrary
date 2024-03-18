#ifndef FT81MEDIAMOVIE_HEADER_GUARD
#define FT81MEDIAMOVIE_HEADER_GUARD


#include "FT81MediaComponent.h"


/********************************************/
/*  FT81xMediaMovie Class               */
/********************************************/

class FT81xMediaMovie : public FT81xMediaComponent {
public:
  virtual ~FT81xMediaMovie();
  virtual void setupMemory(uint32_t addr);
  bool play(File &file);
  void play();
  void setMJpeg(const uint8_t buf[], const uint32_t size);
  void setFullscreen() { m_options |= OPT_FULLSCREEN; }
  void enableSound() { m_options |= OPT_SOUND;}


protected:
  friend class FT81xDisplay;
  FT81xMediaMovie(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp, MediaOperation *mem, const uint32_t chunk = 512);
 
private:
  //uint32_t mAddress;
  uint32_t m_options;
};


#endif // FT81MEDIAMOVIE_HEADER_GUARD
