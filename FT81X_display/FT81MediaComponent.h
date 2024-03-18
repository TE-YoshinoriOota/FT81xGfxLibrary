#ifndef FT81MEDIACOMPONENT_HEADER_GUARD
#define FT81MEDIACOMPONENT_HEADER_GUARD


#include "FT81Core.h"
#include "FT81Component.h"
#include "FT81RegisterOperation.h"
#include "FT81CommandOperation.h"
#include "FT81MediaOperation.h"
#include "FT81Display.h"


/********************************************/
/*  FT81xMediaCompoennt Class               */
/********************************************/

class FT81xMediaComponent : public FT81xComponent {
public:
  virtual ~FT81xMediaComponent();
  virtual void setupMemory(uint32_t addr) = 0;
  uint32_t getChunkSize() { return mChunk; }
  uint8_t* getTransBuf() { return mTransBuf; }

protected:
  friend class FT81xDisplay;
  FT81xMediaComponent(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp, MediaOperation *mem, const uint32_t chunk);
 
protected:
  MediaOperation* myMem;
  RegisterOperation *myReg;
  CommandOperation *myCmd;
  FT81xDisplay *myDisp;

private:
  uint32_t mChunk;
  uint8_t *mTransBuf;
};


#endif // FT81MEDIACOMPONENT_HEADER_GUARD
