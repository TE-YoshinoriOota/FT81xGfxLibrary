#include "FT81xCore.h"
#include "FT81xDisplay.h"
#include "FT81xGfxComponent.h"

FT81xGfxComponent::FT81xGfxComponent(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp) 
 : FT81xComponent() { 
  myReg = reg; 
  myCmd = cmd; 
  myDisp = disp; 
  mVisible = true; 
  myDisp->addGfxComponent(mId, this);
};

FT81xGfxComponent::~FT81xGfxComponent() {
  mVisible = false;
  myDisp->releaseGfxComponent(mId);
}
