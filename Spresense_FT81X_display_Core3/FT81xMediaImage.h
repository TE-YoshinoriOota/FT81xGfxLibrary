#ifndef FT81XMEDIAIMAGE_HEADER_GUARD
#define FT81XMEDIAIMAGE_HEADER_GUARD

#include "FT81xMediaComponent.h"
#include "RegisterOperation.h"
#include "CommandOperation.h"

class FT81xDisplay;


/********************************************/
/*  FT81xImageCompoennt Class               */
/********************************************/

class FT81xMediaImage : public FT81xMediaComponent {
public:
  virtual ~FT81xMediaImage();
  virtual void draw();
  virtual void setupMemory(uint32_t addr);
  bool setImage(File &file);
  void setImage(const uint8_t buf[], const uint32_t size);
  void setPosition(uint16_t x, uint16_t y) { m_x = x; m_y = y; }
  void setSize(uint16_t w, uint16_t h) { m_w = w; m_h = h; }
  void setScale(uint8_t scale) { m_scale = scale; }
  void setRotation(uint16_t rot) { m_rot = rot; }

protected:
  friend class FT81xDisplay;
  void setHandle(uint32_t handle) { mHandle = handle; }
  FT81xMediaImage(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp, MediaOperation *mem, const uint32_t chunk = 512);
 
private:
  uint32_t mHandle;
  uint32_t mAddress;
  uint16_t m_x;
  uint16_t m_y;
  uint16_t m_w;
  uint16_t m_h;
  uint8_t  m_scale;
  uint16_t m_rot;
};



#endif