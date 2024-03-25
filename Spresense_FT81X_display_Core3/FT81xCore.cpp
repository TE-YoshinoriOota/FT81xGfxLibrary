#include "FT81xCore.h"
#include <Flash.h>


uint32_t _send_text_helper_(const char text[], CommandOperation *cmd) {
  uint32_t data = 0xFFFFFFFF;
  for (uint8_t i = 0; (data >> 24) != 0; i += 4) {
    data = 0;
    if (text[i] != 0) {
      data |= text[i];
      if (text[i+1] != 0) {
        data |= text[i+1] << 8;
        if (text[i+2] != 0) {
          data |= (uint32_t)(text[i+2]) << 16;
          if (text[i+3] != 0) {
            data |= (uint32_t)(text[i+3]) << 24;
          }
        }
      }
    }
    if ((data >> 24) != 0) cmd->cmd(data);
    else  return data;
  }

  if ((data >> 24) != 0)  return 0;
  return data;  
}
