#include <SDHCI.h>
SDClass SD;

#include "FT81xGfxLibrary.h"

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

  display->readyCanvas();
  display->clear(255, 255, 255);

  // circle test
  FT81xGfxCircle* circle0 = (FT81xGfxCircle*)display->Create(FT81xCircle);
  FT81xGfxCircle* circle1 = (FT81xGfxCircle*)display->Create(FT81xCircle);
  circle0->setPosition(200, 200);
  circle0->setSize(30);
  circle0->setColor(255, 255, 0); 
  circle1->setPosition(100, 100);
  circle1->setSize(20);
  circle1->setColor(0, 255, 0);

  // rectangle test
  FT81xGfxRectangle* rect0 = (FT81xGfxRectangle*)display->Create(FT81xRectangle);
  FT81xGfxRectangle* rect1 = (FT81xGfxRectangle*)display->Create(FT81xRectangle);
  rect0->setPosition(300, 200);
  rect0->setSize(30, 40);
  rect0->setColor(255, 0, 255);
  rect1->setPosition(400, 100);
  rect1->setSize(40, 15);
  rect1->setColor(0, 255, 255);
  rect1->setCorner(10);

  // triangle test
  FT81xGfxTriangle* tri0 = (FT81xGfxTriangle*)display->Create(FT81xTriangle);
  FT81xGfxTriangle* tri1 = (FT81xGfxTriangle*)display->Create(FT81xTriangle);
  tri0->setColor(128, 128, 128);
  tri0->setPoints(500, 200, 550, 100, 600, 200);
  tri1->setColor(0, 128, 128);
  tri1->setPoints(600, 80, 680, 100, 580, 220);

  // Line test
  FT81xGfxLine* line0 = (FT81xGfxLine*)display->Create(FT81xLine);
  FT81xGfxLine* line1 = (FT81xGfxLine*)display->Create(FT81xLine);
  line0->setColor(128, 0, 128);
  line0->setWidth(10);
  line0->setPoints(600, 300, 700, 400);
  line1->setColor(30, 30, 128);
  line1->setWidth(20);
  line1->setPoints(650, 100, 750, 450);

  // Clock Test
  // Memo: Clock must be before Gradient. Otherwise, Clock will not display.
  //       I'm not sure why this happens. Need to investigate.
  FT81xGfxClock* clock = (FT81xGfxClock*)display->Create(FT81xClock);
  clock->setTime(10, 10, 0);
  clock->setColor(255, 255, 255);
  clock->setBgColor(128, 128, 200);
  clock->setPosition(400, 200);
  clock->setSize(40);

  // Gradient Test
  FT81xGfxGradient* grad0 = (FT81xGfxGradient*)display->Create(FT81xGradient);
  FT81xGfxGradient* grad1 = (FT81xGfxGradient*)display->Create(FT81xGradient);
  grad0->setStartColor(0, 0, 255);
  grad0->setEndColor(255, 255, 255);
  grad0->setPosition(100, 300);
  grad0->setSize(100, 50);
  grad1->setStartColor(0, 0, 255);
  grad1->setEndColor(255, 0, 0);
  grad1->setPosition(200, 350);
  grad1->setSize(200, 80);

  // Text Test
  FT81xGfxText* text0 = (FT81xGfxText*)display->Create(FT81xText);
  FT81xGfxText* text1 = (FT81xGfxText*)display->Create(FT81xText);
  text0->setFont(31);
  text0->setPosition(100, 200);
  text0->setColor(0, 0, 255);
  text0->setText("Hello EVE!");
  text1->setFont(32);
  text1->setPosition(500, 400);
  text1->setColor(255, 0, 0);
  text1->set3dEffect();
  text1->setText("OK EVE!!");

  // Gauge Test
  FT81xGfxGauge* Gauge = (FT81xGfxGauge*)display->Create(FT81xGauge);
  Gauge->setMeter(6, 4, 60);
  Gauge->setValue(0);
  Gauge->setColor(255, 255, 255);
  Gauge->setBgColor(20, 20, 20);
  Gauge->setNeedleColor(255, 128, 128);
  Gauge->setPosition(500, 300);
  Gauge->setSize(60);

  // Progressbar Test
  FT81xGfxProgressbar* progress = (FT81xGfxProgressbar*)display->Create(FT81xProgressbar);
  progress->setValue(0);
  progress->setColor(250, 200, 200);
  progress->setBgColor(70, 20, 20);
  progress->setPosition(100, 10);
  progress->setSize(600, 15);
  progress->setRange(60);

  // display
  display->updateCanvas();
  delay(2000);

  // Widget value change test
  const uint16_t count = 60;
  for (int n = 0; n <= count; ++n) {

    display->readyCanvas();
    display->clear(255, 255, 255);

    char buf[10] = {0};
    if (n == 60) {
      sprintf(buf, "10:11:00");
    } else {
      sprintf(buf, "10:10:%02d", n);
    }
    text1->setText(buf);
    clock->setTime(10, 10, n);
    progress->setValue(n);
    Gauge->setValue(n);

    display->updateCanvas();
    delay(1000);
  }

  display->Destroy(circle0);
  //display->Destroy(circle1);  // reuse this later
  display->Destroy(rect0);
  display->Destroy(rect1);
  display->Destroy(tri0);
  display->Destroy(tri1);  
  display->Destroy(line0);    
  display->Destroy(line1);    
  //display->Destroy(grad0);    // reuse this later 
  grad0->setVisible(false);     // set unvisible
  display->Destroy(grad1);     
  //display->Destroy(text0);    // reuse this later 
  //display->Destroy(text1);    // reuse this later
  text0->setVisible(false);     // set unvisible 
  display->Destroy(clock);  
  display->Destroy(Gauge);  
  display->Destroy(progress);

  // Spinner Test
  FT81xGfxSpinner* spinner = (FT81xGfxSpinner*)display->Create(FT81xSpinner);
  spinner->setColor(200, 200, 200);
  spinner->setPosition(400, 240);
  spinner->setSize(100, 100);
  spinner->setCircleStyle();

  text1->setColor(100, 100, 100);
  text1->setPosition(300, 80);

  display->readyCanvas();
  display->clear(255, 255, 255);
  spinner->setCircleStyle();
  text1->draw("Circle Style");  // force draw, because spinner updates full screen
  display->updateCanvas();
  delay(3000);

  display->readyCanvas();
  display->clear(255, 255, 255);
  spinner->setClockStyle();
  text1->draw("Clock Style");   // force draw, because spinner updates full screen
  display->updateCanvas();
  delay(3000);

  display->readyCanvas();
  display->clear(255, 255, 255);
  spinner->setLineStyle();
  text1->draw("Line Style");    // force draw, because spinner updates full screen
  display->updateCanvas();
  delay(3000);

  display->readyCanvas();
  display->clear(255, 255, 255);
  spinner->setDotStyle();
  text1->draw("Dot Style");    // force draw, because spinner updates full screen
  display->updateCanvas();
  delay(3000);

  display->Destroy(spinner);

  // Touch Test
  display->readyCanvas();
  display->clear(255, 255, 255);  
  //display->forceTouchCalibration(); // if you want to calibrate the touch screen forcibly every time, use this.
  display->doTouchCalibration();
  
  uint16_t x, y;
  uint16_t num = 0;
  display->readyCanvas();
  display->clear(255, 255, 255);  
  text1->setText("Touch Display");  
  display->updateCanvas();

  while (num < 10) {
    display->readyCanvas();
    if (display->isTouched(&x, &y)) {
      Serial.println("x = " + String(x) + " y = " + String(y));
      String xy = "(" + String(x) + ", " + String(y) + ")";
      text1->setText(xy.c_str());
      circle1->setPosition(x, y);
      ++num;
    }
    display->clear(255, 255, 255);  
    display->updateCanvas();
  }
  circle1->setVisible(false);

  // Gui Button Test
  FT81xGuiButton *button0 = (FT81xGuiButton*)display->Create(FT81xButton);
  button0->setPosition(300, 200);
  button0->setFont(23);
  button0->setSize(150, 80);
  button0->setColor(255,255,255);
  button0->setButtonColor(255, 0, 0);
  button0->setText("Button 1");
  button0->setTag(1);
  button0->setCallback(&button_func);
  FT81xGuiButton *button1 = (FT81xGuiButton*)display->Create(FT81xButton);
  button1->setPosition(300, 300);
  button1->setFont(23);
  button1->setSize(150, 80);
  button1->setColor(255,255,255);
  button1->setButtonColor(0, 0, 255);
  button1->setText("Button 2");
  button1->setTag(2);
  button1->setCallback(&button_func);

  text1->setText("Touch buttons");
  num = 0;
  while (num < 10) {
    display->readyCanvas();
    display->clear(255, 255, 255);    
    display->senseGuiComponents();
    if (button0->pushed()) {
      text1->setText("Button 1");
      ++num;
    }
    if (button1->pushed()) {
      text1->setText("Button 2");
      ++num;
    }
    display->updateCanvas();
    delay(100);
  }

  display->Destroy(button0);
  display->Destroy(button1);

  // Gui Keys test
  FT81xGuiKeys *keys = (FT81xGuiKeys*)display->Create(FT81xKeys);
  keys->setPosition(200, 200);
  keys->setFont(23);
  keys->setSize(400, 100);
  keys->setColor(255,255,255);
  keys->setKeysColor(128, 128, 128);
  keys->setKeyMap("12345");
  keys->setCallback(&key_func);

  text1->setText("Touch number");
  num = 0;
  String input;
  while (num < 10) {
    display->readyCanvas();
    display->clear(255, 255, 255);    
    display->senseGuiComponents();
    char value = keys->readKey();
    if (value != 0) {
      input += value;
      text1->setText(input.c_str());
      ++num;
    }
    display->updateCanvas();
    delay(150);
  }

  display->Destroy(keys);
  display->setTouchSensitivity(0xffff); // max sensitivity

  // Gui Dial test
  FT81xGuiDial *dial = (FT81xGuiDial*)display->Create(FT81xDial);
  dial->setPosition(400, 240);
  dial->setSize(100);
  dial->setColor(255,255,255);
  dial->setDialColor(255, 0, 0);
  dial->setCallback(&dial_func);
  dial->setTag(3);

  // Gui Slider test
  FT81xGuiSlider *slider = (FT81xGuiSlider*)display->Create(FT81xSlider);
  slider->setPosition(200, 40);
  slider->setSize(400, 30);
  slider->setColor(0,0,255);
  slider->setSliderColor(0, 0, 128);
  slider->setRange(100);
  slider->setCallback(&dial_func);
  slider->setTag(4);

  // Gui Toggle test
  FT81xGuiToggle *toggle = (FT81xGuiToggle*)display->Create(FT81xToggle);
  toggle->setFont(30);
  toggle->setPosition(100, 240);
  toggle->setSize(100);
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
    display->readyCanvas();
    bool ret = display->senseGuiComponents();
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
    display->updateCanvas();
    delay(30);
  }

  display->Destroy(dial);
  display->Destroy(slider);
  display->Destroy(toggle);


  // Gui Scrollbar
  FT81xGuiScrollbar *scroll0 = (FT81xGuiScrollbar*)display->Create(FT81xScrollbar);
  scroll0->setPosition(100, 40);
  scroll0->setSize(600, 20);
  scroll0->setBarSize(10);
  scroll0->setColor(0, 200, 0);
  scroll0->setScrollbarColor(0, 128, 0);
  //scroll0->setCallback(&scroll_func);
  scroll0->setRange(100);
  scroll0->setTag(6);

  FT81xGuiScrollbar *scroll1 = (FT81xGuiScrollbar*)display->Create(FT81xScrollbar);
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
  text0->setVisible(true);
  text0->setPosition(base_x, base_y);
  text0->setText("move");
  text1->setText("scrollbar values");  
  while (x_value < 100) {
    display->readyCanvas();
    bool ret = display->senseGuiComponents();
    if (ret) {
      x_value = scroll0->readValue();
      y_value = scroll1->readValue();
      uint16_t update_x = base_x - x_value*2;
      uint16_t update_y = base_y - y_value*2;
      String pos_str = "(" + String(update_x) + ", " + String(update_y) + ")";
      text0->setPosition(update_x, update_y);
      text0->setText(pos_str.c_str());
      String log_str = "scroll(" + String(x_value) + ", " + String(y_value) + ")";
      text1->setText(log_str.c_str());
    }

    display->clear(255, 255, 255);    
    display->updateCanvas();
    delay(30);
  }

  text0->setVisible(false);
  text1->setVisible(false);
  display->Destroy(scroll0);
  display->Destroy(scroll1);

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
    display->readyCanvas();
    display->clear(255, 255, 255); 
    display->updateCanvas();
    delay(10);
  }
  // rotation test
  uint16_t dr = 0;
  while (dr <= 360) {
    ++dr;
    image->setRotation(dr);
    display->readyCanvas();
    display->clear(255, 255, 255);
    display->updateCanvas();
    delay(10);
  }

  display->Destroy(image);

  // test synth
  text0->setVisible(true);
  Serial.println("Synth Test");
  FT81xMediaSynth *synth = (FT81xMediaSynth*)display->Create(FT81xSynth);
  synth->setVolume(255);
  text0->setPosition(200, 200);

  synth->setEffect(XYLOPHONE);
  for (uint8_t note = NOTE_A0; note <= NOTE_C8; ++note) {
    String str = "XYLOPHONE (" + String(note) + ")";
    text0->setText(str.c_str());
    synth->setNote(note);
    display->readyCanvas();
    display->clear(255, 255, 255);   
    synth->play();
    display->updateCanvas();
    delay(1000);
  }
  synth->stop();

  synth->setEffect(WARBLE);
  for (uint8_t note = NOTE_A0; note <= NOTE_C8; ++note) {
    String str = "WARBLE (" + String(note) + ")";
    text0->setText(str.c_str());
    synth->setNote(note);
    display->readyCanvas();
    display->clear(255, 255, 255);   
    synth->play();
    display->updateCanvas();
    delay(1000);
  }
  synth->stop();

  display->Destroy(synth);

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

  // Open ULAW audio 
  myAudio = SD.open("ULAW.wav");
  if (myAudio == NULL) {
    Serial.println("ULAW.wav not found");
    return;
  }  

  audio->setFormat(ULAW_SAMPLES);
  audio->setLoop(true);
  audio->setAudio(myAudio); // transfer the audio data to memory
  myAudio.close();

  audio->play();

  display->readyCanvas();
  display->clear(255, 255, 255); 
  text0->setText("ULAW audio : loop");
  display->updateCanvas();
  delay(5000);

  audio->stop();

  display->Destroy(audio);


  text0->setVisible(false);

  // Movie media play test
  Serial.println("--movie media play test--");
  FT81xMediaMovie *movie = (FT81xMediaMovie*)display->Create(FT81xMovie);
  movie->setupMemory(0);
  movie->setFullscreen();
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


  // Movie memory play test
  Serial.println("--movie memory play test--");
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
  }
  myMovie.close();
  Serial.println("finish playing memory movie");

  delay(1000);

  // Test play2
  Serial.println("Replay from avi file");
  myMovie = SD.open("test0.avi");
  movie->play(myMovie);
  myMovie.close();

  display->Destroy(movie);

  grad0->setVisible(true);
  grad0->setStartColor(0, 0, 255);
  grad0->setEndColor(255, 255, 255);
  grad0->setPosition(0, 0);
  grad0->setSize(800, 480);

  text0->setVisible(true);
  text0->setPosition(100, 200);
  text0->setColor(255, 255, 255);
  text0->setText("Congrats! all tests have passed!");

  display->readyCanvas();
  display->clear(255, 255, 255);
  display->updateCanvas();

  display->Destroy(circle1);
  display->Destroy(grad0);
  display->Destroy(text0);
  display->Destroy(text1);
}

void loop() {

}


// Gui functions
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