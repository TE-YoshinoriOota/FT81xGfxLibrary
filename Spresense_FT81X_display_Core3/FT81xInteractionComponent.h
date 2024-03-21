#ifndef FT81XINTERACTIONCOMPONENT_HEADER_GUARD
#define FT81XINTERACTIONCOMPONENT_HEADER_GUARD

#include "FT81xGuiComponent.h"
#include "RegisterOperation.h"
#include "CommandOperation.h"
#include "FT81Core.h"

class FT81xDisplay;

/********************************************/
/*  FT81xInteractionCompoent Class          */
/********************************************/

class FT81xInteractionComponent : public FT81xGuiComponent {
public:
  virtual ~FT81xInteractionComponent();
  virtual void draw() = 0;
  uint8_t getTag() { return mTag; }
  void setCallback(void (*func)(const uint32_t)) { m_func = func; }
  bool setTag(const uint8_t tag);
  void releaseTag(const uint8_t tag);

protected:
  friend class FT81xDisplay;
  FT81xInteractionComponent(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp);
  virtual void doInteraction_(const uint32_t value) = 0;
  virtual bool isTracker() = 0;

protected:
  // In case of InteructionKeys, mTag holds the most height tag number of the keymap.
  // For example, if the keymap is "12345", the mTag of interactionKey object is "0x35 (ascii 5)"
  uint8_t mTag;  
  void (*m_func)(const uint32_t);
};









/********************************************/
/*  FT81xInteractionScrollbar Class            */
/********************************************/  

class FT81xInteractionScrollbar : public FT81xInteractionComponent {
public:
  virtual ~FT81xInteractionScrollbar() {}
  virtual void draw();
  void setColor(const uint8_t r, const uint8_t g, const uint8_t b) { m_color = COLOR_RGB(r, g, b); }
  void setScrollbarColor(const uint8_t r, const uint8_t g, const uint8_t b) { m_bgcolor = COLOR_RGB(r, g, b); }
  void setPosition(const uint16_t x, const uint16_t y) { m_x = x; m_y = y; }
  void setSize(const uint16_t w, const uint16_t h) { m_w = w; m_h = h; }
  void setBarSize(const uint16_t size) { m_size = size; }
  void setRange(const uint16_t range) { m_range = range; }
  uint16_t readValue() { return m_value; }

protected:
  friend class FT81xDisplay;
  FT81xInteractionScrollbar(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp);
  virtual void doInteraction_(const uint32_t value);
  virtual bool isTracker() { return true; }
  
private:
  uint16_t m_x;
  uint16_t m_y;
  uint16_t m_w;
  uint16_t m_h;
  uint32_t m_color;
  uint32_t m_bgcolor;
  uint16_t m_options;
  uint16_t m_value;
  uint16_t m_size;
  uint16_t m_range;
};



#endif