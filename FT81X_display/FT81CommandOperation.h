#ifndef FT81COMMANDOPERATION_HEADER_GUARD
#define FT81COMMANDOPERATION_HEADER_GUARD

#include "FT81GfxCore.h"


/********************************************/
/*  CommandOperation class                  */
/********************************************/

class CommandOperation {
public:
  CommandOperation(SPIClass *spi, RegisterOperation *reg);
  ~CommandOperation();

public:
  void cmd_start();
  void cmd(const uint32_t cmd);
  void cmd_end();

  void waitForCmdFiFoEmpty();
  void checkCmdFiFoBuffer(const uint32_t count);
  uint16_t checkCmdFiFoFreespace();
  void waitCmdFiFoFreespace(uint32_t need_freespace);

private:
  void incCmdWriteAddress(uint16_t delta);
  void updateCmdWriteAddress();

  uint32_t cmd_write_address = 0;
  SPISettings CmdSPISettings;
  SPIClass *mySpi;
  RegisterOperation *myReg;
};

#endif // FT81COMMANDOPERATION_HEADER_GUARD
