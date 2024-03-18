#ifndef FT81GFXCOMPONENT_HEADER_GUARD
#define FT81GFXCOMPONENT_HEADER_GUARD


#include "FT81Core.h"
#include "FT81Component.h"
#include "FT81RegisterOperation.h"
#include "FT81CommandOperation.h"
#include "FT81Display.h"


/********************************************/
/*  FT81xGfxComponent Class                 */
/********************************************/

class FT81xGfxComponent : public FT81xComponent {
public:
  virtual ~FT81xGfxComponent() {};
  virtual void draw() = 0;
  void hide(bool b) { mHide = b; }

protected:
  friend class FT81xDisplay;
  FT81xGuiComponent(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp) { myReg = reg; myCmd = cmd; myDisp = disp; mHide = false; };
  bool isHide() { return mHide; };

protected:
  FT81xDisplay *myDisp;
  RegisterOperation *myReg;
  CommandOperation *myCmd;
  mHide;
};

#endif // FT81GFXCOMPOENENTS_HEADER_GUARD

