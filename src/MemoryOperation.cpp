#include "FT81xCore.h"
#include "MemoryOperation.h"


/********************************************/
/*  MemoryOperation class                  */
/********************************************/

MemoryOperation::MemoryOperation(SPIClass *spi, RegisterOperation *reg, CommandOperation *cmd) {
  mySpi = spi;
  CmdSPISettings = SPISettings(SPI_FREQ_RUNING, MSBFIRST, SPI_MODE0);
  myCmd = cmd;
  myReg = reg;
}


void MemoryOperation::memWrite(uint32_t address, uint8_t buf[], uint32_t size) {
  uint32_t addr = address | WRITE;
  mySpi->beginTransaction(CmdSPISettings);
  digitalWrite(CS_PIN, LOW);  
  mySpi->transfer((addr >> 16) & 0xFF);
  mySpi->transfer((addr >> 8) & 0xFF);
  mySpi->transfer(addr & 0xFF);
  mySpi->send(buf, size);
  digitalWrite(CS_PIN, HIGH);
  mySpi->endTransaction();  
}

