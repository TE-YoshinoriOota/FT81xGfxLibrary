#ifndef FT81MEDIASYNTH_HEADER_GUARD
#define FT81MEDIASYNTH_HEADER_GUARD


#include "FT81MediaComponent.h"


/********************************************/
/*  FT81xMediaSynth Class                   */
/********************************************/
/* Note definitions */
#define NOTE_A0 (21)
#define NOTE_A_SHARP_0 (22)
#define NOTE_B0 (23)
#define NOTE_C1 (24)
#define NOTE_C_SHARP_1 (25)
#define NOTE_D1 (26)
#define NOTE_D_SHARP_1 (27)
#define NOTE_E1 (28)
#define NOTE_F1 (29)
#define NOTE_F_SHARP_1 (30)
#define NOTE_G1 (31)
#define NOTE_G_SHARP_1 (32)
#define NOTE_A1 (33)
#define NOTE_A_SHARP_1 (34)
#define NOTE_B1 (35)
#define NOTE_C2 (36)
#define NOTE_C_SHARP_2 (37)
#define NOTE_D2 (38)
#define NOTE_D_SHARP_2 (39)
#define NOTE_E2 (40)
#define NOTE_F2 (41)
#define NOTE_F_SHARP_2 (42)
#define NOTE_G2 (43)
#define NOTE_G_SHARP_2 (44)
#define NOTE_A2 (45)
#define NOTE_A_SHARP_2 (46)
#define NOTE_B2 (47)
#define NOTE_C3 (48)
#define NOTE_C_SHARP_3 (49)
#define NOTE_D3 (50)
#define NOTE_D_SHARP_3 (51)
#define NOTE_E3 (52)
#define NOTE_F3 (53)
#define NOTE_F_SHARP_3 (54)
#define NOTE_G3 (55)
#define NOTE_G_SHARP_3 (56)
#define NOTE_A3 (57)
#define NOTE_A_SHARP_3 (58)
#define NOTE_B3 (59)
#define NOTE_C4 (60)
#define NOTE_C_SHARP_4 (61)
#define NOTE_D4 (62)
#define NOTE_D_SHARP_4 (63)
#define NOTE_E4 (64)
#define NOTE_F4 (65)
#define NOTE_F_SHARP_4 (66)
#define NOTE_G4 (67)
#define NOTE_G_SHARP_4 (68)
#define NOTE_A4 (69)
#define NOTE_A_SHARP_4 (70)
#define NOTE_B4 (71)
#define NOTE_C5 (72)
#define NOTE_C_SHARP_5 (73)
#define NOTE_D5 (74)
#define NOTE_D_SHARP_5 (75)
#define NOTE_E5 (76)
#define NOTE_F5 (77)
#define NOTE_F_SHARP_5 (78)
#define NOTE_G5 (79)
#define NOTE_G_SHARP_5 (80)
#define NOTE_A5 (81)
#define NOTE_A_SHARP_5 (82)
#define NOTE_B5 (83)
#define NOTE_C6 (84)
#define NOTE_C_SHARP_6 (85)
#define NOTE_D6 (86)
#define NOTE_D_SHARP_6 (87)
#define NOTE_E6 (88)
#define NOTE_F6 (89)
#define NOTE_F_SHARP_6 (90)
#define NOTE_G6 (91)
#define NOTE_G_SHARP_6 (92)
#define NOTE_A6 (93)
#define NOTE_A_SHARP_6 (94)
#define NOTE_B6 (95)
#define NOTE_C7 (96)
#define NOTE_C_SHARP_7 (97)
#define NOTE_D7 (98)
#define NOTE_D_SHARP_7 (99)
#define NOTE_E7 (100)
#define NOTE_F7 (101)
#define NOTE_F_SHARP_7 (102)
#define NOTE_G7 (103)
#define NOTE_G_SHARP_7 (104)
#define NOTE_A7 (105)
#define NOTE_A_SHARP_7 (106)
#define NOTE_B7 (107)
#define NOTE_C8 (108)

/* Sound Effects                   continous   pitch */
#define SILENCE (0x00)          //     Y          N
#define SQURE_WAVE (0x01)       //     Y          Y
#define SIN_WAVE (0x02)         //     Y          Y
#define SAW_WAVE (0x03)         //     Y          Y
#define TRIANGLE_WAVE (0x04)    //     Y          Y
#define BEEP (0x05)             //     Y          Y
#define ALARM (0x06)            //     Y          Y
#define WARBLE (0x07)           //     Y          Y
#define CAROUSEL (0x08)         //     Y          Y
#define PIP01 (0x10)            //     N          Y
#define PIP02 (0x11)            //     N          Y
#define PIP03 (0x12)            //     N          Y
#define PIP04 (0x13)            //     N          Y
#define PIP05 (0x14)            //     N          Y
#define PIP06 (0x15)            //     N          Y
#define PIP07 (0x16)            //     N          Y
#define PIP08 (0x17)            //     N          Y
#define PIP09 (0x18)            //     N          Y
#define PIP10 (0x19)            //     N          Y
#define PIP11 (0x1A)            //     N          Y
#define PIP12 (0x1B)            //     N          Y
#define PIP13 (0x1C)            //     N          Y
#define PIP14 (0x1D)            //     N          Y
#define PIP15 (0x1E)            //     N          Y
#define PIP16 (0x1F)            //     N          Y
#define DTMFA (0x23)            //     Y          N
#define DTMFB (0x2C)            //     Y          N
#define DTMF0 (0x30)            //     Y          N
#define DTMF1 (0x31)            //     Y          N
#define DTMF2 (0x32)            //     Y          N
#define DTMF3 (0x33)            //     Y          N
#define DTMF4 (0x34)            //     Y          N
#define DTMF5 (0x35)            //     Y          N
#define DTMF6 (0x36)            //     Y          N
#define DTMF7 (0x37)            //     Y          N
#define DTMF8 (0x38)            //     Y          N
#define DTMF9 (0x39)            //     Y          N
#define HARP (0x40)             //     N          Y
#define XYLOPHONE (0x41)        //     N          Y
#define TUBA (0x42)             //     N          Y
#define GLOCKENSPIEL (0x43)     //     N          Y
#define ORGAN (0x44)            //     N          Y
#define TRUNPET (0x45)          //     N          Y
#define PIANO (0x46)            //     N          Y
#define CHIMES (0x47)           //     N          Y
#define MUSICBOX (0x48)         //     N          Y
#define BELL (0x49)             //     N          Y
#define CLICK (0x50)            //     N          N
#define SWITCH (0x51)           //     N          N
#define COWBELL (0x52)          //     N          N
#define NOTCH (0x53)            //     N          N
#define HIHAT (0x54)            //     N          N
#define KICKDRUM (0x55)         //     N          N
#define POP (0x56)              //     N          N
#define CLACK (0x57)            //     N          N
#define CHACK (0x58)            //     N          N
#define MUTE (0x60)             //     N          N
#define UNMUTE (0x61)           //     N          N

class FT81xMediaSynth : public FT81xMediaComponent {
public:
  virtual ~FT81xMediaSynth() {}
  virtual void setupMemory(uint32_t addr) { /* no need */};
  void play();
  void stop();  
  void setVolume(uint8_t vol);
  bool setNote(uint8_t note);
  bool setEffect(uint8_t effect); 
  bool isContinuous(uint8_t effect);

protected:
  friend class FT81xDisplay;
  FT81xMediaSynth(RegisterOperation *reg, CommandOperation *cmd, FT81xDisplay *disp, MediaOperation *mem, const uint32_t chunk = 0);
 
private:
  uint8_t m_volume;
  uint8_t m_note;
  uint8_t m_effect;
};


#endif // FT81MEDIASYNTH_HEADER_GUARD
