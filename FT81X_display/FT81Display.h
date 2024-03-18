#ifndef FT81DISPLAY_HEADER_GUARD
#define FT81DISPLAY_HEADER_GUARD

#include "FT81Core.h"
#include "FT81RegisterOperation.h"
#include "FT81CommandOperation.h"
#include "FT81MemoryOperation.h"
#include "FT81MediaOperation.h"
#include "FT81GfxComponent.h"
#include "FT81GfxCircle.h"
#include "FT81GfxRectangle.h"
#include "FT81GfxTriangle.h"
#include "FT81GfxLine.h"
#include "FT81GfxGradient.h"
#include "FT81GfxText.h"
#include "FT81GfxClock.h"
#include "FT81GfxProgressbar.h"
#include "FT81GfxSpinner.h"
#include "FT81GfxGauge.h"
#include "FT81GuiComponent.h"
#include "FT81GuiButton.h"
#include "FT81GuiDial.h"
#include "FT81GuiKeys.h"
#include "FT81GuiScrollbar.h"
#include "FT81GuiSlider.h"
#include "FT81GuiToggle.h"
#include "FT81MediaComponent.h"
#include "FT81MediaAudio.h"
#include "FT81MediaImage.h"
#include "FT81MediaMovie.h"
#include "FT81MediaSynth.h"
#include <map>


/********************************************/
/*  FT81xDisplay Class                      */
/********************************************/

struct DisplayProperty {
  uint16_t  FT_DispWidth;
  uint16_t  FT_DispHeight;
  uint16_t  FT_DispHCycle;
  uint16_t  FT_DispHOffset;
  uint16_t  FT_DispHSync0;
  uint16_t  FT_DispHSync1;
  uint16_t  FT_DispVCycle;
  uint16_t  FT_DispVOffset;
  uint16_t  FT_DispVSync0;
  uint16_t  FT_DispVSync1;
  uint16_t  FT_DispPCLK;
  uint16_t  FT_DispSwizzle;
  uint16_t  FT_DispPCLKPol;
  uint16_t  FT_DispCSpread;
  uint16_t  FT_DispDither;
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
  void setupCanvas();
  void clear(const uint8_t r, const uint8_t g, const uint8_t b);
  bool senseGuiComponents();
  void swap();

  uint16_t getDisplayWidth() { return mDispProp.FT_DispWidth; }
  uint16_t getDisplayHeight() { return mDispProp.FT_DispHeight; }
  uint32_t getBgColor() { return m_bgcolor;  }

public:
  FT81xGuiComponent* Create(enum GuiComponents elem);

protected:
  friend class FT81xGuiComponent;
  void setTag(const uint8_t t, FT81xGuiComponent *elem);
  void releaseTag(const uint8_t t);

protected:
  friend class FT81GfxComponent;
  void releaseGfxComponent(const uint32_t uid);

public:
  bool doTouchCalibration();
  bool isTouchCalibration();
  void forceTouchCalibration();
  void removeTouchCalibration();

  bool isTouched();
  bool isTouched(uint16_t *x, uint16_t *y);

  void setTouchSensitivity(uint16_t sensitivity);

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
  std::map<uint32_t, FT81xMediaImage*> mImages;
  std::map<uint32_t, FT81xGfxComponent*> mGfxComponents;
  uint32_t mImageHandle;
};

#endif // FT81DISPLAY_HEADER_GUARD
