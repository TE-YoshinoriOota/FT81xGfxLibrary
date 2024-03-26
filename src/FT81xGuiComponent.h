#ifndef FT81XINTERACTIONCOMPONENT_HEADER_GUARD
#define FT81XINTERACTIONCOMPONENT_HEADER_GUARD

#include "FT81xGfxComponent.h"
#include "RegisterOperation.h"
#include "CommandOperation.h"
#include "FT81xCore.h"

class FT81xDisplay;

/********************************************/
/*  FT81xGuiCompoent Class                  */
/********************************************/

class FT81xGuiComponent : public FT81xGfxComponent {
public:
  virtual ~FT81xGuiComponent();
  virtual void draw() = 0;
  uint8_t getTag() { return mTag; }
  void setCallback(void (*func)(const uint32_t)) { m_func = func; }
  bool setTag(const uint8_t tag);
  void releaseTag(const uint8_t tag);

protected:
  friend class FT81xDisplay;
  FT81xGuiComponent(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp);
  virtual void doInteraction_(const uint32_t value) = 0;
  virtual bool isTracker() = 0;

protected:
  // In case of InteructionKeys, mTag holds the most height tag number of the keymap.
  // For example, if the keymap is "12345", the mTag of interactionKey object is "0x35 (ascii 5)"
  uint8_t mTag;  
  void (*m_func)(const uint32_t);
};

#endif
