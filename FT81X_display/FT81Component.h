#ifndef FT81COMPONENT_HEADER_GUARD
#define FT81COMPONENT_HEADER_GUARD


#include "FT81Core.h"

enum GuiComponents {
   FT81xCircle
  ,FT81xRectangle
  ,FT81xTriangle
  ,FT81xLine
  ,FT81xGradient
  ,FT81xClock
  ,FT81xText
  ,FT81xSpinner
  ,FT81xGauge
  ,FT81xProgressbar
  ,FT81xMovie
  ,FT81xButton
  ,FT81xScrollbar
  ,FT81xSlider
  ,FT81xDial
  ,FT81xToggle
  ,FT81xKeys
  ,FT81xImage
  ,FT81xAudio
  ,FT81xSynth
};


/********************************************/
/*  FT81xComponent Class                 */
/********************************************/

class FT81xComponent {
public:
  virtual ~FT81xComponent() {};
  FT81xComponent() { mUid = random(0,0xffffffff); }
  enum FT81Components type() { return mType; }
  uint32_t getUid() { return mUid; }

protected:
  enum FT81Components mType;

private:
  uint32_t mUid;
};

#endif // FT81COMPOENENTS_HEADER_GUARD
