#ifndef FT81XGUICOMPONENT_HEADER_GUARD
#define FT81XGUICOMPONENT_HEADER_GUARD

#include "RegisterOperation.h"
#include "CommandOperation.h"
#include "MediaOperation.h"
//#include "FT81xDisplay.h"
#include "FT81Core.h"

class FT81xDisplay;

/********************************************/
/*  FT81xGuiComponent Class                 */
/********************************************/


class FT81xGuiComponent {
public:
  virtual ~FT81xGuiComponent() {};
  enum GuiComponents type() { return mType; }
  virtual void draw() = 0;

protected:
  friend class FT81xDisplay;
  FT81xGuiComponent(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp) { myReg = reg; myCmd = cmd; myDisp = disp; };

protected:
  enum GuiComponents mType;
  FT81xDisplay *myDisp;
  RegisterOperation *myReg;
  CommandOperation *myCmd;
};





#endif