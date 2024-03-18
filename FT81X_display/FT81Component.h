#ifndef FT81COMPONENT_HEADER_GUARD
#define FT81COMPONENT_HEADER_GUARD


#include "FT81Core.h"


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
