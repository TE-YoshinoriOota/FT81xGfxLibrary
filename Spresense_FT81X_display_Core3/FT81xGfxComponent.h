#ifndef FT81XGUICOMPONENT_HEADER_GUARD
#define FT81XGUICOMPONENT_HEADER_GUARD

#include "RegisterOperation.h"
#include "CommandOperation.h"
#include "MediaOperation.h"
//#include "FT81xDisplay.h"
#include "FT81Core.h"

class FT81xDisplay;

/********************************************/
/*  FT81xGfxComponent Class                 */
/********************************************/


class FT81xGfxComponent {
public:
  virtual ~FT81xGfxComponent() {};
  enum FT81xComponents type() { return mType; }
  virtual void draw() = 0;

protected:
  friend class FT81xDisplay;
  FT81xGfxComponent(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp) { myReg = reg; myCmd = cmd; myDisp = disp; };

protected:
  enum FT81xComponents mType;
  FT81xDisplay *myDisp;
  RegisterOperation *myReg;
  CommandOperation *myCmd;
};


#endif
