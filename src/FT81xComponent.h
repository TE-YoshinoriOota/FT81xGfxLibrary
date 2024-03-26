#ifndef FT81XCOMPONENT_HEADER_GUARD
#define FT81XCOMPONENT_HEADER_GUARD

#include "FT81xCore.h"


/********************************************/
/*  FT81xComponent Class                    */
/********************************************/

class FT81xComponent {
public:
  virtual ~FT81xComponent();
  enum FT81xComponents type() { return mType; }
  uint16_t getId() { return mId; }

protected:
  friend class FT81xDisplay;
  FT81xComponent();

protected:
  enum FT81xComponents mType;
  uint16_t mId;
};


#endif
