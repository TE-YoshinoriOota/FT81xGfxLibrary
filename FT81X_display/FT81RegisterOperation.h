#ifndef FT81REGISTEROPERATION_HEADER_GUARD
#define FT81REGISTEROPERATION_HEADER_GUARD


#include "FT81Core.h"


/********************************************/
/*  RegisterOperation class                 */
/********************************************/

class RegisterOperation {
public:
  RegisterOperation(SPIClass *spi);
  ~RegisterOperation();

  uint8_t  rd8(const uint32_t  addr);
  uint16_t rd16(const uint32_t addr);
  uint32_t rd32(const uint32_t addr);
  void wr8(const uint32_t  addr, const uint8_t value);
  void wr16(const uint32_t addr, const uint16_t value);
  void wr32(const uint32_t addr, const uint32_t value);
  void host_command(const uint8_t cmd) ;

private:
  SPIClass* mySpi;
};

#endif // FT81REGISTEROPERATION_HEADER_GUARD
