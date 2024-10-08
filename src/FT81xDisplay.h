#ifndef FT81XDISPLAY_HEADER_GUARD
#define FT81XDISPLAY_HEADER_GUARD


#include <SPI.h>
#include "RegisterOperation.h"
#include "CommandOperation.h"
#include "MediaOperation.h"
#include "FT81xComponent.h"
#include "FT81xGfxComponent.h"
#include "FT81xGuiComponent.h"
#include "FT81xCore.h"


/********************************************/
/*  FT81xDisplay Class                      */
/********************************************/

struct DisplayProperty {
  uint16_t  FT_DispWidth;
  uint16_t 	FT_DispHeight;
  uint16_t 	FT_DispHCycle;
  uint16_t 	FT_DispHOffset;
  uint16_t 	FT_DispHSync0;
  uint16_t 	FT_DispHSync1;
  uint16_t 	FT_DispVCycle;
  uint16_t 	FT_DispVOffset;
  uint16_t 	FT_DispVSync0;
  uint16_t 	FT_DispVSync1;
  uint16_t 	FT_DispPCLK;
  uint16_t 	FT_DispSwizzle;
  uint16_t 	FT_DispPCLKPol;
  uint16_t 	FT_DispCSpread;
  uint16_t 	FT_DispDither;
};



enum DisplaySize {
   DISPLAY_RESOLUTION_WVGA
  ,DISPLAY_RESOLUTION_WQVGA
  ,DISPLAY_RESOLUTION_QVGA
};


enum SPI_CHANNEL {
   SPI_CH1
  ,SPI_CH3
  ,SPI_CH5
};

class FT81xDisplay {
public:
  static FT81xDisplay* Instance(enum SPI_CHANNEL spi_ch);
  void begin(enum DisplaySize disp);
  void readyCanvas();
  void clear(const uint8_t r, const uint8_t g, const uint8_t b);
  bool senseGuiComponents();
  void updateCanvas();
  void swap();

  uint16_t getDisplayWidth() { return mDispProp.FT_DispWidth; }
  uint16_t getDisplayHeight() { return mDispProp.FT_DispHeight; }
  uint32_t getBgColor() { return m_bgcolor;  }

public:
  FT81xComponent* Create(enum FT81xComponents elem);
  void Destroy(FT81xComponent *elem);


public:
  bool doTouchCalibration();
  bool isTouchCalibration();
  void forceTouchCalibration();
  void removeTouchCalibration();

  bool isTouched();
  bool isTouched(uint16_t *x, uint16_t *y);

  void setTouchSensitivity(uint16_t sensitivity);

protected:
  friend class FT81xGfxComponent;
  friend class FT81xMediaImage;
  void addGfxComponent(uint16_t id, FT81xComponent *gfx);
  void releaseGfxComponent(uint16_t id);

protected:
  friend class FT81xGuiComponent;
  void setTag(const uint8_t t, FT81xGuiComponent *elem);
  void releaseTag(const uint8_t t);


private:
  char m_prop_file[16];
  void _restoreTouchCalibration();
  uint8_t _read_tag();
  uint32_t _read_tracker_value(const uint8_t tag);

private:
  FT81xDisplay(SPIClass *spi);
  ~FT81xDisplay();
  struct DisplayProperty mDispProp;
  static FT81xDisplay *theDisplay1;
  static FT81xDisplay *theDisplay3;
  static FT81xDisplay *theDisplay5;
  uint32_t m_bgcolor;
  SPIClass *mySpi;
  static RegisterOperation *myReg;
  static CommandOperation *myCmd;
  static MediaOperation *myMem;

private:
  std::map<uint8_t, FT81xGuiComponent*> mGuiComponents;
  std::map<uint16_t, FT81xComponent*> mComponents;
};




#endif
