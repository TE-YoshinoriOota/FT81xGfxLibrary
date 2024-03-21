#include "FT81Core.h"
#include "CommandOperation.h"


/********************************************/
/*  CommandOperation class                  */
/********************************************/

CommandOperation::CommandOperation(SPIClass *spi, RegisterOperation *reg) {
  mySpi = spi;
  CmdSPISettings = SPISettings(SPI_FREQ_RUNING, MSBFIRST, SPI_MODE0);
  myReg = reg;
}

CommandOperation::~CommandOperation() {

}


void CommandOperation::cmd_start() {
  waitForCmdFiFoEmpty();
  cmd_write_address = myReg->rd16(REG_CMD_WRITE);
  uint32_t addr = (RAM_CMD + cmd_write_address) | WRITE;
  mySpi->beginTransaction(CmdSPISettings);
  digitalWrite(CS_PIN, LOW);

  mySpi->transfer(addr >> 16);
  mySpi->transfer(addr >> 8);
  mySpi->transfer(addr);
}

void CommandOperation::cmd(const uint32_t cmd) {
  mySpi->transfer(cmd);
  mySpi->transfer(cmd >> 8);
  mySpi->transfer(cmd >> 16);
  mySpi->transfer(cmd >> 24);
  incCmdWriteAddress(4);  
}
void CommandOperation::cmd_end() {
  digitalWrite(CS_PIN, HIGH);
  mySpi->endTransaction();
  updateCmdWriteAddress();  
}

void CommandOperation::waitForCmdFiFoEmpty() {
  while(myReg->rd16(REG_CMD_READ) != myReg->rd16(REG_CMD_WRITE)) {
    #if 0
    static uint32_t last_cmd_write_address = cmd_write_address;
    cmd_write_address = rd16(REG_CMD_WRITE);
    if (cmd_write_address != last_cmd_write_address) {
      Serial.println("Wait for command fifo empty");
      last_cmd_write_address = cmd_write_address;
    }
    #else
      __asm__ volatile("nop");
    #endif
  }
}

void CommandOperation::checkCmdFiFoBuffer(const uint32_t count) {
  uint16_t getfreespace;
  do {
    getfreespace = checkCmdFiFoFreespace();
  } while (getfreespace < count); 
}

uint16_t CommandOperation::checkCmdFiFoFreespace() {
  uint16_t fullness = 0;
  uint16_t retval = 0;
  fullness = (cmd_write_address - myReg->rd16(REG_CMD_READ)) & 4095;
  retval = (FT_CMD_FIFO_SIZE - 4) - fullness;
  return retval;
}

void CommandOperation::waitCmdFiFoFreespace(uint32_t need_freespace) {
  uint16_t freespace;
  do {
    freespace = checkCmdFiFoFreespace(); 
  } while (freespace < need_freespace);
}

void CommandOperation::incCmdWriteAddress(uint16_t delta) {
  cmd_write_address = (cmd_write_address + delta) % 4096; 
  cmd_write_address = (cmd_write_address + 3) & 0xffc; // 4 byte alignment
}

void CommandOperation::updateCmdWriteAddress() {
  myReg->wr16(REG_CMD_WRITE, cmd_write_address);   
}

