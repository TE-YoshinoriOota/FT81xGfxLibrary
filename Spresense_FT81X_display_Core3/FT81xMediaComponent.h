#ifndef FT81XMEDIACOMPONENT_HEADER_GUARD
#define FT81XMEDIACOMPONENT_HEADER_GUARD

#include "FT81xGuiComponent.h"
#include "RegisterOperation.h"
#include "CommandOperation.h"

FT81xDisplay;


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


#endif