#include "FT81xMediaComponent.h"
#include "FT81xDisplay.h"
#include "MediaOperation.h"


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






