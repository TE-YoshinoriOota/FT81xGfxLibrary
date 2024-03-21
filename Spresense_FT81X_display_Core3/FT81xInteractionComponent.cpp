#include "FT81Core.h"
#include "FT81xInteractionComponent.h"
#include "FT81xDisplay.h"

/********************************************/
/*  FT81xInteractionComponent Class            */
/********************************************/  

FT81xInteractionComponent::FT81xInteractionComponent(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp)
 : FT81xGuiComponent(reg, cmd, disp) {
  mTag = 0;
  m_func = NULL;
}

FT81xInteractionComponent::~FT81xInteractionComponent() {
  myDisp->releaseTag(mTag);
}

bool FT81xInteractionComponent::setTag(uint8_t tag) {
  // TODO: need to think about avoiding confliction tags of keys.
  mTag = tag;
  myDisp->setTag(mTag, this);
  return true;
}

void FT81xInteractionComponent::releaseTag(uint8_t tag) {
  myDisp->releaseTag(mTag);
}



