#ifndef FT81XGUICOMPONENT_HEADER_GUARD
#define FT81XGUICOMPONENT_HEADER_GUARD

#include "FT81xComponent.h"
#include "RegisterOperation.h"
#include "CommandOperation.h"
#include "MediaOperation.h"
#include "FT81xCore.h"

class FT81xDisplay;


/********************************************/
/*  FT81xGfxComponent Class                 */
/********************************************/


class FT81xGfxComponent : public FT81xComponent {
public:
  virtual ~FT81xGfxComponent();
  virtual void draw() = 0;
  void setVisible(bool visible) { mVisible = visible; }
  bool getVisible() { return mVisible; }

protected:
  friend class FT81xDisplay;
  FT81xGfxComponent(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp);

protected:
  RegisterOperation *myReg;
  CommandOperation *myCmd;
  FT81xDisplay *myDisp;
  bool mVisible;
};


#endif
