#include "FT81xCore.h"
#include "RegisterOperation.h"


/********************************************/
/*  RegisterOperation class                 */
/********************************************/

RegisterOperation::RegisterOperation(SPIClass* spi) {
  mySpi = spi;
}

RegisterOperation::~RegisterOperation() {

}

uint8_t  RegisterOperation::rd8(const uint32_t  addr) {
  uint8_t value;
  digitalWrite(CS_PIN, LOW);
  mySpi->transfer(addr >> 16);
  mySpi->transfer(highByte(addr));
  mySpi->transfer(lowByte(addr));
  mySpi->transfer(0); //Dummy Read Byte
  value = mySpi->transfer(0);
  digitalWrite(CS_PIN, HIGH);
  return value;
}

uint16_t RegisterOperation::rd16(const uint32_t addr) {
  uint16_t value;
  uint16_t byte0;
  uint16_t byte1;
  
  digitalWrite(CS_PIN, LOW);
  mySpi->transfer(addr >> 16);
  mySpi->transfer(highByte(addr));
  mySpi->transfer(lowByte(addr));
  mySpi->transfer(0); //Dummy Read Byte
  byte0 = mySpi->transfer(0);
  byte1 = mySpi->transfer(0);
  value = (byte1 << 8) | byte0;
  digitalWrite(CS_PIN, HIGH);
  return value;
}

uint32_t RegisterOperation::rd32(const uint32_t addr) {
  uint32_t value;
  uint32_t byte0;
  uint32_t byte1;
  uint32_t byte2;
  uint32_t byte3;

  digitalWrite(CS_PIN, LOW);
  mySpi->transfer(addr >> 16);
  mySpi->transfer(highByte(addr));
  mySpi->transfer(lowByte(addr));
  mySpi->transfer(0); //Dummy Read Byte
  byte0 = mySpi->transfer(0);
  byte1 = mySpi->transfer(0);
  byte2 = mySpi->transfer(0);
  byte3 = mySpi->transfer(0);
  value = (byte3 << 24) | (byte2 << 16) | (byte1 << 8) | byte0;
  digitalWrite(CS_PIN, HIGH);
  return value;
}

void RegisterOperation::wr8(const uint32_t  addr, const uint8_t value) {
  digitalWrite(CS_PIN, LOW);
  mySpi->transfer(0x80 | (addr >> 16));
  mySpi->transfer(highByte(addr));
  mySpi->transfer(lowByte(addr));
  mySpi->transfer(value);
  digitalWrite(CS_PIN, HIGH);
}

void RegisterOperation::wr16(const uint32_t addr, const uint16_t value) {
  digitalWrite(CS_PIN, LOW);
  mySpi->transfer(0x80 | (addr >> 16));
  mySpi->transfer(highByte(addr));
  mySpi->transfer(lowByte(addr));
  mySpi->transfer(value & 0xFF);//LSB first
  mySpi->transfer((value >> 8) & 0xFF);
  digitalWrite(CS_PIN, HIGH);
}

void RegisterOperation::wr32(const uint32_t addr, const uint32_t value) {
  digitalWrite(CS_PIN, LOW);
  mySpi->transfer(0x80 | (addr >> 16));
  mySpi->transfer(highByte(addr));
  mySpi->transfer(lowByte(addr));
  mySpi->transfer(value & 0xFF);//LSB first
  mySpi->transfer((value >>  8) & 0xFF);  // 2Byte
  mySpi->transfer((value >> 16) & 0xFF);  // 3Byte
  mySpi->transfer((value >> 24) & 0xFF);  // 4Byte
  digitalWrite(CS_PIN, HIGH);
}

void RegisterOperation::host_command(const uint8_t cmd) {
  digitalWrite(CS_PIN, LOW);
  mySpi->transfer(cmd);
  mySpi->transfer(0);
  mySpi->transfer(0);
  digitalWrite(CS_PIN, HIGH);  
}
