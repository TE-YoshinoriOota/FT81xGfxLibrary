#include "FT81MediaSynth.h"


/********************************************/
/*  FT81xMediaSynth Class                   */
/********************************************/

FT81xMediaSynth::FT81xMediaSynth(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp, MediaOperation *mem, const uint32_t chunk)
 : FT81xMediaComponent(reg, cmd, disp, mem, chunk)  {
  m_volume = 128;
  m_note = NOTE_C1;
  m_effect = BEEP;
}
 
void FT81xMediaSynth::play() {
  myReg->wr8(REG_VOL_SOUND, m_volume);
  myReg->wr16(REG_SOUND, ((uint16_t)(m_note) << 8) | m_effect);
  myReg->wr8(REG_PLAY, 1);
}

void FT81xMediaSynth::stop() {
  myReg->wr8(REG_VOL_SOUND, 0);
  myReg->wr16(REG_SOUND, 0);
  myReg->wr8(REG_PLAY, 1);
}  

void FT81xMediaSynth::setVolume(uint8_t vol) {
  if (vol > 255) vol = 255;
  m_volume = vol; 
}

bool FT81xMediaSynth::setNote(uint8_t note) {
  if (note < NOTE_A0 || note > NOTE_C8) return false;
  if ((m_effect >= SQURE_WAVE && m_effect <= CAROUSEL)
   || (m_effect >= PIP01 && m_effect <= PIP16)
   || (m_effect >= HARP && m_effect <= BELL)
  ) {
    m_note = note;
    return true;
  }
  return false;
}

bool FT81xMediaSynth::setEffect(uint8_t effect) {
  if ((effect > CAROUSEL && effect < PIP01)
   || (effect > PIP16 && effect < DTMFA)
   || (effect > DTMFB && effect < DTMF0)   
   || (effect > DTMF9 && effect < HARP)  
   || (effect > BELL && effect < CLICK)  
   || (effect > CHACK && effect < MUTE)  
   || (effect > UNMUTE)         
  ) {
    return false;
  }
  m_effect = effect;
  return true;
}

bool FT81xMediaSynth::isContinuous(uint8_t effect) {
  if ((effect >= SILENCE && effect <= CAROUSEL)
   || (effect >= DTMFA && effect <= DTMFB)
   || (effect >= DTMF0 && effect <= DTMF9)
  ) {
    return true;
  }
  return false;
}
