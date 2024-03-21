#include "FT81xGfxTriangle.h"
#include "FT81xDisplay.h"

/********************************************/
/*  FT81xGfxTriangle Class                    */
/********************************************/

FT81xGfxTriangle::~FT81xGfxTriangle() {

}

FT81xGfxTriangle::FT81xGfxTriangle(RegisterOperation* reg, CommandOperation* cmd, FT81xDisplay *disp)
 : FT81xGuiComponent(reg, cmd, disp) {
  mType = FT81xTriangle;
  m_x1 = m_y1 = 0;
  m_x2 = m_y2 = 0;
  m_x3 = m_y3 = 0;    
  m_fgcolor = 0;
  m_bgcolor = 0;
}

void FT81xGfxTriangle::setPoints(const uint16_t x1, const uint16_t y1, const uint16_t x2, const uint16_t y2, const uint16_t x3, const uint16_t y3) {
  if ((y1 <= y2) && (y1 <= y3)) {  // point 1 is highest on screen
    m_x1 = x3; m_y1 = y3;
    m_x2 = x1; m_y2 = y1;
    m_x3 = x2; m_y3 = y2;
  } else if ((y2 <= y3) && (y2 <= y1)) {  // point 2 is highest
    m_x1 = x1; m_y1 = y1;
    m_x2 = x2; m_y2 = y2;
    m_x3 = x3; m_y3 = y3;
  } else {  // point 3 highest
    m_x1 = x2; m_y1 = y2;
    m_x2 = x3; m_y2 = y3;
    m_x3 = x1; m_y3 = y1;
  }
}

void FT81xGfxTriangle::draw() {
  m_bgcolor = myDisp->getBgColor();
  myCmd->cmd_start();
  if (m_x2 <= m_x1) {  // one colour wipe (2-3), two bg wipes
    myCmd->cmd(COLOR(m_fgcolor));
    myCmd->cmd(LINE_WIDTH(1*16));
    myCmd->cmd(BEGIN(EDGE_STRIP_B));
    myCmd->cmd(VERTEX2F(m_x2*16, m_y2*16));
    myCmd->cmd(VERTEX2F(m_x3*16, m_y3*16));
    myCmd->cmd(COLOR(m_bgcolor));
    myCmd->cmd(LINE_WIDTH(1*16));
    myCmd->cmd(BEGIN(EDGE_STRIP_B));
    myCmd->cmd(VERTEX2F(m_x3*16, m_y3*16));
    myCmd->cmd(VERTEX2F(m_x1*16, m_y1*16));
    myCmd->cmd(VERTEX2F(m_x2*16, m_y2*16));
  } else if (m_x2 >= m_x3) {  // one colour wipe (1-2), two bg wipes
    myCmd->cmd(COLOR(m_fgcolor));
    myCmd->cmd(LINE_WIDTH(1*16));
    myCmd->cmd(BEGIN(EDGE_STRIP_B));
    myCmd->cmd(VERTEX2F(m_x1*16, m_y1*16));
    myCmd->cmd(VERTEX2F(m_x2*16, m_y2*16));
    myCmd->cmd(COLOR(m_bgcolor));
    myCmd->cmd(LINE_WIDTH(1*16));
    myCmd->cmd(BEGIN(EDGE_STRIP_B));
    myCmd->cmd(VERTEX2F(m_x2*16, m_y2*16));
    myCmd->cmd(VERTEX2F(m_x3*16, m_y3*16));
    myCmd->cmd(VERTEX2F(m_x1*16, m_y1*16));
  } else {  // two colour wipes, one bg wipe
    myCmd->cmd(COLOR(m_fgcolor));
    myCmd->cmd(LINE_WIDTH(1*16));
    myCmd->cmd(BEGIN(EDGE_STRIP_B));
    myCmd->cmd(VERTEX2F(m_x1*16, m_y1*16));
    myCmd->cmd(VERTEX2F(m_x2*16, m_y2*16));
    myCmd->cmd(VERTEX2F(m_x3*16, m_y3*16));
    myCmd->cmd(COLOR(m_bgcolor));
    myCmd->cmd(LINE_WIDTH(1*16));
    myCmd->cmd(BEGIN(EDGE_STRIP_B));
    myCmd->cmd(VERTEX2F(m_x3*16, m_y3*16));
    myCmd->cmd(VERTEX2F(m_x1*16, m_y1*16));
  }
  myCmd->cmd(END());
  myCmd->cmd_end();
}

