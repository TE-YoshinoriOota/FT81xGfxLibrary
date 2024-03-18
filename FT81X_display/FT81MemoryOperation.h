#ifndef FT81MEMORYOPERATION_HEADER_GUARD
#define FT81MEMORYOPERATION_HEADER_GUARD

#include "FT81Core.h"


/********************************************/
/*  MemoryOperation class                  */
/********************************************/

#define MEM_CHUNK (512)

class MemoryOperation {
public:
  MemoryOperation(SPIClass *spi, RegisterOperation *reg, CommandOperation *cmd);
  virtual ~MemoryOperation() {}
  void memWrite(uint32_t address, uint8_t buf[], uint32_t size);

protected:
  SPISettings CmdSPISettings;
  SPIClass *mySpi;
  CommandOperation *myCmd;
  RegisterOperation *myReg;
};

#endif // FT81MEMORYOPERATION_HEADER_GUARD
