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
  FT81xGfxComponent(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp);
  bool isHide() { return mHide; };

protected:
  FT81xDisplay *myDisp;
  RegisterOperation *myReg;
  CommandOperation *myCmd;
  bool mHide;
};

#endif // FT81GFXCOMPOENENTS_HEADER_GUARD

