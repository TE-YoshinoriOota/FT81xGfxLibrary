#include "FT81xComponent.h"

#define MAX_ID_POOL (256)
static bool gIdPool[MAX_ID_POOL] = {0};

FT81xComponent::FT81xComponent() {
  uint16_t id = 0;
  for (; id < MAX_ID_POOL; ++id) {
    if (gIdPool[id] == 0) {
      gIdPool[id] = 1;
      break;
    }
  }
  mId = id;
}

FT81xComponent::~FT81xComponent() {
  gIdPool[mId] = 0;
}