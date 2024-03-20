#include "FT81GfxComponent.h"

FT81xGfxComponent::FT81xGfxComponent(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp) :
 FT81xComponent() {
  myReg = reg; 
  myCmd = cmd; 
  myDisp = disp; 
  mHide = false; 
}
