#include "FT81Core.h"
#include <SDHCI.h>
SDClass SD;

#include "FT81xDisplay.h"
#include "FT81xGuiComponent.h"
#include "FT81xGfxCircle.h"
#include "FT81xGfxRectangle.h"
#include "FT81xGfxTriangle.h"
#include "FT81xGfxLine.h"
#include "FT81xGfxGradient.h"
#include "FT81xGfxText.h"
#include "FT81xComponentGauge.h"
#include "FT81xComponentClock.h"
#include "FT81xComponentProgressbar.h"
#include "FT81xComponentSpinner.h"

#include "FT81xInteractionComponent.h"
#include "FT81xInteractionButton.h"
#include "FT81xInteractionKeys.h"
#include "FT81xInteractionDial.h"
#include "FT81xInteractionSlider.h"
#include "FT81xInteractionToggle.h"
#include "FT81xInteractionScrollbar.h"

#include "FT81xMediaComponent.h"
#include "FT81xMediaImage.h"
#include "FT81xMediaSynth.h"
#include "FT81xMediaAudio.h"
#include "FT81xMediaMovie.h"

uint8_t g_button_pushed = 0;
uint8_t g_button_id = 0;

uint8_t g_key_pushed = 0;
String g_key_strings;



void setup() {
  pinMode(21, OUTPUT);
  digitalWrite(21, OUTPUT);

  FT81xDisplay* display = FT81xDisplay::Instance(SPI_CH1);

  Serial.println("SPI1 settings");

  display->begin(DISPLAY_RESOLUTION_WVGA);


  display->startDisplayList();
  display->clear(255, 255, 255);

  // circle test
  FT81xGfxCircle* circle0 = (FT81xGfxCircle*)display->Create(FT81xCircle);
  FT81xGfxCircle* circle1 = (FT81xGfxCircle*)display->Create(FT81xCircle);
  circle0->setPosition(200, 200);
  circle0->setSize(30);
  circle0->setColor(255, 255, 0); 
  circle0->draw();
  circle1->setPosition(100, 100);
  circle1->setSize(20);
  circle1->setColor(0, 255, 0);
  circle1->draw();

  // rectangle test
  FT81xGfxRectangle* rect0 = (FT81xGfxRectangle*)display->Create(FT81xRectangle);
  FT81xGfxRectangle* rect1 = (FT81xGfxRectangle*)display->Create(FT81xRectangle);
  rect0->setPosition(300, 200);
  rect0->setSize(30, 40);
  rect0->setColor(255, 0, 255);
  rect0->draw();
  rect1->setPosition(400, 100);
  rect1->setSize(40, 15);
  rect1->setColor(0, 255, 255);
  rect1->setCorner(10);
  rect1->draw();

  // triangle test
  FT81xGfxTriangle* tri0 = (FT81xGfxTriangle*)display->Create(FT81xTriangle);
  FT81xGfxTriangle* tri1 = (FT81xGfxTriangle*)display->Create(FT81xTriangle);
  tri0->setColor(128, 128, 128);
  tri0->setPoints(500, 200, 550, 100, 600, 200);
  tri0->draw();
  tri1->setColor(0, 128, 128);
  tri1->setPoints(600, 80, 680, 100, 580, 220);
  tri1->draw();

  // Line test
  FT81xGfxLine* line0 = (FT81xGfxLine*)display->Create(FT81xLine);
  FT81xGfxLine* line1 = (FT81xGfxLine*)display->Create(FT81xLine);
  line0->setColor(128, 0, 128);
  line0->setWidth(10);
  line0->setPoints(600, 300, 700, 400);
  line0->draw();
  line1->setColor(30, 30, 128);
  line1->setWidth(20);
  line1->setPoints(650, 100, 750, 450);
  line1->draw();

  // Gradient Test
  FT81xGfxGradient* grad0 = (FT81xGfxGradient*)display->Create(FT81xGradient);
  FT81xGfxGradient* grad1 = (FT81xGfxGradient*)display->Create(FT81xGradient);
  grad0->setStartColor(0, 0, 255);
  grad0->setEndColor(255, 255, 255);
  grad0->setPosition(100, 300);
  grad0->setSize(100, 50);
  grad0->draw();
  grad1->setStartColor(0, 0, 255);
  grad1->setEndColor(255, 0, 0);
  grad1->setPosition(200, 350);
  grad1->setSize(200, 80);
  grad1->draw();

  // Text Test
  FT81xGfxText* text0 = (FT81xGfxText*)display->Create(FT81xText);
  FT81xGfxText* text1 = (FT81xGfxText*)display->Create(FT81xText);
  text0->setFont(31);
  text0->setPosition(100, 200);
  text0->setColor(0, 0, 255);
  text0->setText("Hello EVE!");
  text0->draw();

  text1->setFont(32);
  text1->setPosition(500, 400);
  text1->setColor(255, 0, 0);
  text1->set3dEffect();
  text1->draw("OK EVE!!");

  // Clock Test
  FT81xComponentClock* clock = (FT81xComponentClock*)display->Create(FT81xClock);
  clock->setTime(10, 10, 0);
  clock->setColor(255, 255, 255);
  clock->setBgColor(128, 128, 200);
  clock->setPosition(400, 200);
  clock->setSize(50);
  clock->draw();

  // Gauge Test
  FT81xComponentGauge* Gauge = (FT81xComponentGauge*)display->Create(FT81xGauge);
  Gauge->setMeter(6, 4, 60);
  Gauge->setValue(0);
  Gauge->setColor(255, 255, 255);
  Gauge->setBgColor(20, 20, 20);
  Gauge->setNeedleColor(255, 128, 128);
  Gauge->setPosition(500, 300);
  Gauge->setSize(60);
  Gauge->draw();

  // Progressbar Test
  FT81xComponentProgressbar* progress = (FT81xComponentProgressbar*)display->Create(FT81xProgressbar);
  progress->setValue(0);
  progress->setColor(250, 200, 200);
  progress->setBgColor(70, 20, 20);
  progress->setPosition(100, 10);
  progress->setSize(600, 15);
  progress->setRange(60);
  progress->draw();

  // display
  display->swap();

  delay(2000);

#if 1
  for (int n = 0; n <= 10; ++n) {
    display->startDisplayList();
    display->clear(255, 255, 255);

    circle0->draw();
    circle1->draw();
    rect0->draw();
    rect1->draw();
    tri0->draw();
    tri1->draw();
    line0->draw();
    line1->draw();
    grad0->draw();
    grad1->draw();
    text0->draw();
    char buf[10] = {0};
    if (n == 60) {
      sprintf(buf, "10:11:00");
    } else {
      sprintf(buf, "10:10:%02d", n);
    }
    text1->draw(buf);
    clock->draw(10, 10, n);
    Gauge->draw(n);
    progress->draw(n);

    display->swap();
    delay(1000);
  }


  delete circle0; circle0 = NULL;
  // delete circle1; circle1 = NULL;
  delete rect0; rect0 = NULL;
  delete rect1; rect1 = NULL;
  delete tri0; tri0 = NULL;
  delete tri1; tri1 = NULL;
  delete line0; line0 = NULL;
  delete line1; line1 = NULL;
  // delete grad0; grad0 = NULL;
  delete grad1; grad1 = NULL;
  // delete text0; text0 = NULL;
  // delete text1; text1 = NULL; // reuse text1 below procedures
  delete clock; clock = NULL;
  delete Gauge; Gauge = NULL;

  // Spinner Test
  FT81xComponentSpinner* spinner = (FT81xComponentSpinner*)display->Create(FT81xSpinner);
  spinner->setColor(200, 200, 200);
  spinner->setPosition(400, 240);
  spinner->setSize(100, 100);
  spinner->setCircleStyle();

  text1->setColor(100, 100, 100);
  text1->setPosition(320, 80);

  display->startDisplayList();
  display->clear(255, 255, 255);
  text1->draw("Circle Style"); // Text should be first
  spinner->draw();
  display->swap();
  delay(3000);

  display->startDisplayList();
  display->clear(255, 255, 255);
  text1->draw("Clock Style"); // Text should be first
  spinner->setClockStyle();
  spinner->draw();
  display->swap();
  delay(3000);

  display->startDisplayList();
  display->clear(255, 255, 255);
  text1->draw("Line Style"); // Text should be first
  spinner->setLineStyle();
  spinner->draw();
  display->swap();
  delay(3000);

  display->startDisplayList();
  display->clear(255, 255, 255);
  text1->draw("Dot Style"); // Text should be first
  spinner->setDotStyle();
  spinner->draw();
  display->swap();
  delay(3000);

  delete spinner; spinner = NULL;


  // Touch Test
  display->startDisplayList();
  display->clear(255, 255, 255);  
  display->forceTouchCalibration();
  //display->doTouchCalibration();
  
  uint16_t x, y;
  uint16_t num = 0;
  display->startDisplayList();
  display->clear(255, 255, 255);  
  text1->setText("Touch Display");  
  text1->draw();  
  display->swap();
  while (num < 10) {
    display->startDisplayList();
    if (display->isTouched(&x, &y)) {
      Serial.println("x = " + String(x) + " y = " + String(y));
      String xy = "(" + String(x) + ", " + String(y) + ")";
      text1->setText(xy.c_str());
      circle1->setPosition(x, y);
      ++num;
    }
    display->clear(255, 255, 255);  
    text1->draw();  
    circle1->draw();
    display->swap();
  }

  // Interaction Button Test
  FT81xInteractionButton *button0 = (FT81xInteractionButton*)display->Create(FT81xButton);
  button0->setPosition(300, 200);
  button0->setFont(21);
  button0->setSize(100, 50);
  button0->setColor(255,255,255);
  button0->setButtonColor(255, 0, 0);
  button0->setText("Button 1");
  button0->setTag(1);
  button0->setCallback(&button_func);
  FT81xInteractionButton *button1 = (FT81xInteractionButton*)display->Create(FT81xButton);
  button1->setPosition(300, 300);
  button1->setFont(21);
  button1->setSize(100, 50);
  button1->setColor(255,255,255);
  button1->setButtonColor(0, 0, 255);
  button1->setText("Button 2");
  button1->setTag(2);
  button1->setCallback(&button_func);

  text1->setText("Touch buttons");
  num = 0;
  while (num < 10) {
    display->startDisplayList();
    display->clear(255, 255, 255);    
    display->senseInteractionComponents();
    if (button0->pushed()) {
      text1->setText("Button 1 is pushed!");
      ++num;
    }
    if (button1->pushed()) {
      text1->setText("Button 2 is pushed!");
      ++num;
    }
    button0->draw();
    button1->draw();
    text1->draw();
    display->swap();
    delay(100);
  }

  delete button0; button0 = NULL;
  delete button1; button1 = NULL;


  // Interaction Keys test
  
  FT81xInteractionKeys *keys = (FT81xInteractionKeys*)display->Create(FT81xKeys);
  keys->setPosition(300, 200);
  keys->setFont(21);
  keys->setSize(200, 50);
  keys->setColor(255,255,255);
  keys->setKeysColor(128, 128, 128);
  keys->setKeyMap("12345");
  keys->setCallback(&key_func);

  text1->setText("Touch number");
  num = 0;
  String input;
  while (num < 10) {
    display->startDisplayList();
    display->clear(255, 255, 255);    
    display->senseInteractionComponents();
    char value = keys->readKey();
    if (value != 0) {
      input += value;
      text1->setText(input.c_str());
      ++num;
    }
    text1->draw();
    keys->draw();
    display->swap();
    delay(150);
  }

  delete keys; keys = NULL;

  display->setTouchSensitivity(0xffff);

  // Interaction Dial test and Slider test
  // Interaction Dial
  FT81xInteractionDial *dial = (FT81xInteractionDial*)display->Create(FT81xDial);
  dial->setPosition(400, 240);
  dial->setSize(50);
  dial->setColor(255,255,255);
  dial->setDialColor(255, 0, 0);
  dial->setCallback(&dial_func);
  dial->setTag(3);
  // Interaction Slider
  FT81xInteractionSlider *slider = (FT81xInteractionSlider*)display->Create(FT81xSlider);
  slider->setPosition(200, 40);
  slider->setSize(400, 30);
  slider->setColor(0,0,255);
  slider->setSliderColor(0, 0, 128);
  slider->setRange(100);
  slider->setCallback(&dial_func);
  slider->setTag(4);
  // Interaction Toggle
  FT81xInteractionToggle *toggle = (FT81xInteractionToggle*)display->Create(FT81xToggle);
  toggle->setFont(28);
  toggle->setPosition(100, 240);
  toggle->setSize(50);
  toggle->setColor(255, 255, 255);
  toggle->setToggleColor(128, 128, 128);
  toggle->setText("NO","YES");
  toggle->setCallback(&toggle_func);
  toggle->setTag(5);

  text1->setPosition(400, 400);
  text1->setText("Touch Dial");
  text0->setPosition(200, 100);
  text0->setText("Touch Slider");

  uint16_t d_value = 0;
  uint16_t s_value = 0;
  while (d_value < 90) {
    display->startDisplayList();
    bool ret = display->senseInteractionComponents();
    if (ret) {
      d_value = dial->readValue();
      s_value = slider->readValue();
      //Serial.println("readValue: " + String(value, HEX));
      String d_str = "Dial value is " + String(d_value) + "%";
      text1->setText(d_str.c_str());
      String s_str = "Slide value is " + String(s_value);
      text0->setText(s_str.c_str());
    }

    display->clear(255, 255, 255);    
    text1->draw();
    text0->draw();
    dial->draw();
    slider->draw();
    toggle->draw();
    display->swap();
    delay(30);
  }

  delete keys; keys = NULL;
  delete slider; slider = NULL;
  delete toggle; toggle = NULL;

  // Interaction Scrollbar
  FT81xInteractionScrollbar *scroll0 = (FT81xInteractionScrollbar*)display->Create(FT81xScrollbar);
  scroll0->setPosition(100, 40);
  scroll0->setSize(600, 20);
  scroll0->setBarSize(10);
  scroll0->setColor(0, 200, 0);
  scroll0->setScrollbarColor(0, 128, 0);
  //scroll0->setCallback(&scroll_func);
  scroll0->setRange(100);
  scroll0->setTag(6);

  FT81xInteractionScrollbar *scroll1 = (FT81xInteractionScrollbar*)display->Create(FT81xScrollbar);
  scroll1->setPosition(730, 80);
  scroll1->setSize(20, 300);
  scroll1->setBarSize(10);
  scroll1->setColor(0, 0, 200);
  scroll1->setScrollbarColor(0, 0, 128);
  //scroll1->setCallback(&scroll_func);
  scroll1->setRange(100);
  scroll1->setTag(7);

  uint16_t x_value = 0;
  uint16_t y_value = 0;

  const uint16_t base_x = 500;
  const uint16_t base_y = 300;
  text0->setPosition(base_x, base_y);
  text0->setText("move scrollbar");
  text1->setText("scrollbar values");  
  while (x_value < 100) {
    display->startDisplayList();
    bool ret = display->senseInteractionComponents();
    if (ret) {
      x_value = scroll0->readValue();
      y_value = scroll1->readValue();
      //Serial.println("readValue: " + String(value, HEX));
      uint16_t update_x = base_x - x_value*2;
      uint16_t update_y = base_y - y_value*2;
      String pos_str = "(" + String(update_x) + ", " + String(update_y) + ")";
      text0->setPosition(update_x, update_y);
      text0->setText(pos_str.c_str());
      String log_str = "scroll(" + String(x_value) + ", " + String(y_value) + ")";
      text1->setText(log_str.c_str());
    }

    display->clear(255, 255, 255);    
    text1->draw();
    text0->draw();
    scroll0->draw();
    scroll1->draw();
    display->swap();
    delay(30);
  }
#endif

  // Image Test
  Serial.println("Image Test");
  FT81xMediaImage *image = (FT81xMediaImage*)display->Create(FT81xImage);
  uint32_t addr = 0x00000000;
  image->setupMemory(addr);
  image->setPosition(0, 0);
  image->setSize(500, 372);

  while (!SD.begin()) {}
  File myImage = SD.open("img0.jpg");
  if (myImage == NULL) {
    Serial.println("File not found");
    delete image;
    return;
  }
  image->setImage(myImage);
  myImage.close();

  // moving test
  uint16_t dx = 0;
  uint16_t dy = 0;
  const uint16_t goal_x = 150; // (screen_width - image_width)/2
  const uint16_t goal_y =  54; // (screen_height - image_height)/2
  while (dx <= goal_x) {
    dx += 2;
    if (dy <= goal_y) dy += 1;
    image->setPosition(dx, dy);
    display->startDisplayList();
    display->clear(255, 255, 255);
    image->draw();
    display->swap();
    delay(10);
  }
  // rotation test
  uint16_t dr = 0;
  while (dr <= 360) {
    ++dr;
    image->setRotation(dr);
    display->startDisplayList();
    display->clear(255, 255, 255);
    image->draw();
    display->swap();
    delay(10);
  }

  delete image;

#if 1
  // test synth
  Serial.println("Synth Test");
  FT81xMediaSynth *synth = (FT81xMediaSynth*)display->Create(FT81xSynth);
  synth->setVolume(255);
  text0->setPosition(200, 200);
  /*
  synth->setEffect(BELL);
  for (uint8_t note = NOTE_A0; note <= NOTE_C8; ++note) {
    String str = "Playing BELL (" + String(note) + ")";
    text0->setText(str.c_str());
    synth->setNote(note);
    display->startDisplayList();
    display->clear(255, 255, 255);   
    synth->play();
    text0->draw();
    display->swap();
    delay(1000);
  }
  synth->stop();
  */
  synth->setEffect(XYLOPHONE);
  for (uint8_t note = NOTE_A0; note <= NOTE_C8; ++note) {
    String str = "Playing XYLOPHONE (" + String(note) + ")";
    text0->setText(str.c_str());
    synth->setNote(note);
    display->startDisplayList();
    display->clear(255, 255, 255);   
    synth->play();
    text0->draw();
    display->swap();
    delay(1000);
  }
  synth->stop();
  synth->setEffect(WARBLE);
  for (uint8_t note = NOTE_A0; note <= NOTE_C8; ++note) {
    String str = "Playing WARBLE (" + String(note) + ")";
    text0->setText(str.c_str());
    synth->setNote(note);
    display->startDisplayList();
    display->clear(255, 255, 255);   
    synth->play();
    text0->draw();
    display->swap();
    delay(1000);
  }
  synth->stop();
  delete synth;


  // Test Audio
  Serial.println("Audio Test");
  FT81xMediaAudio *audio = (FT81xMediaAudio*)display->Create(FT81xAudio);

  // ADPCM
  File myAudio = SD.open("LINEAR.wav");
  if (myAudio == NULL) {
    Serial.println("LINEAR.wav not found");
    return;
  }
  audio->setupMemory(0x00000000);
  audio->setVolume(255);
  audio->setFormat(LINEAR_SAMPLES);
  audio->setSamplingRate(16000);
  audio->setAudio(myAudio);
  myAudio.close();

  String audioStr = "Playing LINEAR audio : no loop";
  text0->setText(audioStr.c_str());

  display->startDisplayList();
  display->clear(255, 255, 255); 
  text0->draw();
  audio->play();
  display->swap();
  delay(5000);
  audio->stop();

  // ULAW 
  myAudio = SD.open("ULAW.wav");
  if (myAudio == NULL) {
    Serial.println("ULAW.wav not found");
    return;
  }  

  audio->setFormat(ULAW_SAMPLES);
  audio->setLoop(true);
  audio->setAudio(myAudio);
  myAudio.close();

  audioStr = "Playing ULAW audio : loop";
  text0->setText(audioStr.c_str());

  display->startDisplayList();
  display->clear(255, 255, 255); 
  text0->draw();
  audio->play();
  display->swap();
  delay(5000);
  audio->stop();

  // ADPCM
  myAudio = SD.open("ADPCM.wav");
  if (myAudio == NULL) {
    Serial.println("ADPCM.wav not found");
    return;
  }  

  audio->setFormat(ADPCM_SAMPLES);
  audio->setLoop(false);
  audio->setAudio(myAudio);
  myAudio.close();

  audioStr = "Playing ADPCM audio : no loop";
  text0->setText(audioStr.c_str());

  display->startDisplayList();
  display->clear(255, 255, 255); 
  text0->draw();
  audio->play();
  display->swap();
  delay(5000);
  audio->stop();

  delete audio;
#endif

  // Test Motion Jpeg
  Serial.println("Motion Jpeg Test");
  FT81xMediaMovie *movie = (FT81xMediaMovie*)display->Create(FT81xMovie);
  movie->setupMemory(0);
  //movie->setFullscreen();
  movie->enableSound();

  File myMovie = SD.open("domo0a.avi");
  if (myMovie == NULL) {
    Serial.println("domo0a.avi not found");
    return;    
  }
  movie->play(myMovie);
  myMovie.seek(0);
  Serial.println("loop play test");
  movie->play(myMovie); // loop test
  myMovie.close();

  delay(1000);


  // Test memory play
  Serial.println("--memory play test--");
  myMovie = SD.open("domo0a.avi");
  if (myMovie == NULL) {
    Serial.println("test0.avi not found");
    return;    
  }

  static const uint32_t movie_buf_size = 16384;
  static uint8_t movie_buf[movie_buf_size];

  int32_t movie_size = (int32_t)myMovie.size();
 
  movie->play();
  while (movie_size > 0) {
    int read_size = myMovie.read(movie_buf, movie_buf_size);
    if (read_size > 0) {
      movie->setMovie(movie_buf, read_size);
      movie_size -= read_size;
    } else {
      Serial.println("End of file");
      break;
    }
    // do something. 
    //In the case of 16384 with 800x450 video, you can get 20msec for other tasks
    delay(20);
  }
  myMovie.close();
  Serial.println("finish playing memory movie");

  delay(1000);

  // Test play2
  Serial.println("Replay from avi file");
  myMovie = SD.open("test0.avi");
  movie->play(myMovie);
  myMovie.close();

  delete movie;

  grad0->setStartColor(0, 0, 255);
  grad0->setEndColor(255, 255, 255);
  grad0->setPosition(0, 0);
  grad0->setSize(800, 480);

  text0->setPosition(100, 200);
  text0->setColor(255, 255, 255);
  text0->setText("Congrats! all tests have passed!");

  display->startDisplayList();
  display->clear(255, 255, 255);
  grad0->draw();
  text0->draw();
  display->swap();

  delete grad0;
  delete text0;
  delete text1;
}

void loop() {

}


// interaction functions
void button_func(const uint32_t tag) {
  Serial.println(String(tag) + " Button is pushed!!");
}

void key_func(const uint32_t tag) {
  Serial.println(String(tag) + " key is pushed!!");
}

void dial_func(const uint32_t value) {
  Serial.println(String(value) + " dial is operated!!");
}


void toggle_func(const uint32_t value) {
  Serial.println(String(value) + " toggle is operated!!");
}