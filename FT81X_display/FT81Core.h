#ifndef FT81GFXCORE_HEADER_GUARD
#define FT81GFXCORE_HEADER_GUARD

#include <Arduino.h>
#include <SPI.h>
#include "FT81_GPU.h"

#define SPI1_CS_PIN  (4)
#define SPI1_PDN_PIN (8)
#define SPI3_CS_PIN  (25)
#define SPI3_PDN_PIN (18)
#define SPI5_CS_PIN  (25)
#define SPI5_PDN_PIN (18)

#define SPI1_FREQ_LAUNCH   (8000000)
#define SPI1_FREQ_RUNING  (18000000)
#define SPI3_FREQ_LAUNCH   (4000000)
#define SPI3_FREQ_RUNING   (8000000)
#define SPI5_FREQ_LAUNCH   (4000000)
#define SPI5_FREQ_RUNING   (8000000)


#define READ  0x000000  // Bitmask address reading
#define WRITE 0x800000  // Bitmask for address writing

#define END_DL()         0x00  // End current display list
#define SWAP()           0xFFFFFF01  // Swap the current display list
#define COLOR(rgb)       ((0x04L << 24) | ((rgb)&0xFFFFFF))     
#define CLEAR_COLOR(rgb) ((0x02L << 24) | ((rgb)&0xFFFFFF))   


#define WVGA_FT_DispWidth     800
#define WVGA_FT_DispHeight    480
#define WVGA_FT_DispHCycle    928
#define WVGA_FT_DispHOffset   88
#define WVGA_FT_DispHSync0    0
#define WVGA_FT_DispHSync1    48
#define WVGA_FT_DispVCycle    525
#define WVGA_FT_DispVOffset   32
#define WVGA_FT_DispVSync0    0
#define WVGA_FT_DispVSync1    3
#define WVGA_FT_DispPCLK      2
#define WVGA_FT_DispSwizzle   0
#define WVGA_FT_DispPCLKPol   1
#define WVGA_FT_DispCSpread   0
#define WVGA_FT_DispDither    1


#define WQVGA_FT_DispWidth    480
#define WQVGA_FT_DispHeight   272
#define WQVGA_FT_DispHCycle   548
#define WQVGA_FT_DispHOffset  43
#define WQVGA_FT_DispHSync0   0
#define WQVGA_FT_DispHSync1   41
#define WQVGA_FT_DispVCycle   292
#define WQVGA_FT_DispVOffset  12
#define WQVGA_FT_DispVSync0   0
#define WQVGA_FT_DispVSync1   10
#define WQVGA_FT_DispPCLK     5
#define WQVGA_FT_DispSwizzle  0
#define WQVGA_FT_DispPCLKPol  1
#define WQVGA_FT_DispCSpread  1
#define WQVGA_FT_DispDither   1


#define QVGA_FT_DispWidth    320
#define QVGA_FT_DispHeight   240
#define QVGA_FT_DispHCycle   408
#define QVGA_FT_DispHOffset  70
#define QVGA_FT_DispHSync0   0
#define QVGA_FT_DispHSync1   10
#define QVGA_FT_DispVCycle   263
#define QVGA_FT_DispVOffset  13
#define QVGA_FT_DispVSync0   0
#define QVGA_FT_DispVSync1   2
#define QVGA_FT_DispPCLK     8
#define QVGA_FT_DispSwizzle  2
#define QVGA_FT_DispPCLKPol  0
#define QVGA_FT_DispCSpread  1
#define QVGA_FT_DispDither   1

enum FT81xComponents {
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

extern uint32_t SPI_FREQ_LAUNCH;
extern uint32_t SPI_FREQ_RUNING;
extern uint8_t  CS_PIN;
extern uint8_t  PDN_PIN;

#endif // FT81GFXCORE_HEADER_GUARD

