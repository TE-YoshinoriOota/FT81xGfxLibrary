#include "FT81GuiComponent.h"


/********************************************/
/*  FT81xGuiComponent Class                 */
/********************************************/  

FT81xGuiComponent::FT81xGuiComponent(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp)
 : FT81xGuiComponent(reg, cmd, disp) {
  mTag = 0;
  m_func = NULL;
}

FT81xGuiComponent::~FT81xGuiComponent() {
  myDisp->releaseTag(mTag);
}

bool FT81xGuiComponent::setTag(uint8_t tag) {
  // TODO: need to think about avoiding confliction tags of keys.
  mTag = tag;
  myDisp->setTag(mTag, this);
  return true;
}

void FT81xGuiComponent::releaseTag(uint8_t tag) {
  myDisp->releaseTag(mTag);
}
