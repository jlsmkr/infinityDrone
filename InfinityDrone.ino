#include "DaisyDuino.h"
#include "Wire.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include "Synth.h"
#include "Pad.h"
#include "Presets.h"

#define PAD_COUNT 2

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C

#define SDA_PIN 12
#define SCL_PIN 11

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

Pad::Config current_config = deep;
Pad::Config next_config = deep;

float sr;
int next_midi = 69, current_midi = 0;
bool active_synth = false;
bool set_type = true; // true: midi; false: pad
bool is_stopped = true;
int active_pad = 0;
const char* NOTE_NAMES[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};

Pad *pad1, *pad2;
static ReverbSc reverb;

Switch start_button, stop_button, set_button, prev_button, next_button;

void AudioCallback(float **in, float **out, size_t size) {
  for (size_t i = 0; i < size; i++) {
    float l = 0.0f;
    float r = 0.0f;

    pad1->Process(l, r);
    pad2->Process(l, r);

    float rev_l, rev_r;
    reverb.Process(l, r, &rev_l, &rev_r);

    out[0][i] = l * 0.25f + rev_l * 0.75f;
    out[1][i] = r * 0.25f + rev_r * 0.75f;
  }
}

void getNoteName(int midi, char* buffer) {
    if (midi < 0) {
        strcpy(buffer, " ");
        return;
    }
    // Daisy/MIDI Standard: 60 = C4. Deine Logik startete bei 61.
    int note_idx = midi % 12;
    strcpy(buffer, NOTE_NAMES[note_idx]);
}

void drawUi() {
  // create key char
  char current_key[3];
  char next_key[3];
  getNoteName(is_stopped ? -1 : current_midi, current_key);
  getNoteName(next_midi, next_key);
  
  // display stuff
  display.clearDisplay();

  int mid_x = 64;
  int split_y = 21;

  // top left
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20, 7);
  if (is_stopped) {
    display.print("    ");
  } else {
    display.print(current_config.name);
  }

  // top right
  if (!set_type) {
    display.fillRect(mid_x, 0, 64, 21, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
  }
  display.setCursor(mid_x + 20, 7);
  display.print(next_config.name);
  display.setTextColor(SSD1306_WHITE);

  // bottom left
  display.setTextSize(3);

  int16_t x1, y1;
  uint16_t w, h;
  int pos_x, pos_y;

  display.getTextBounds(current_key, 0, 0, &x1, &y1, &w, &h);

  pos_x = (mid_x / 2) - (w / 2);
  pos_y = split_y + ((64 - split_y) / 2) - (h / 2);

  display.setCursor(pos_x, pos_y);
  display.print(current_key);

  // bottom right
  if (set_type) {
    display.fillRect(mid_x, split_y, 64, 43, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
  }
  display.getTextBounds(next_key, 0, 0, &x1, &y1, &w, &h);

  pos_x = mid_x + 32 - (w / 2);
  pos_y = split_y + ((64 - split_y) / 2) - (h / 2);

  display.setCursor(pos_x, pos_y);
  display.print(next_key);

  display.display();
}

void SwitchPad() {
  if (active_synth) {
    // no 1
    pad2->SetGate(false);
    pad1->ApplyConfig(next_config, next_midi);
    pad1->SetGate(true);
  } else {
    // no 2
    pad1->SetGate(false);
    pad2->ApplyConfig(next_config, next_midi);
    pad2->SetGate(true);
  }
  current_config = next_config;
  current_midi = next_midi;
  is_stopped = false;
  drawUi();
}

void setup() {
  // init buttons
  start_button.Init(1000, true, 15, INPUT_PULLUP);
  stop_button.Init(1000, true, 16, INPUT_PULLUP);
  set_button.Init(1000, true, 17, INPUT_PULLUP);
  prev_button.Init(1000, true, 18, INPUT_PULLUP);
  next_button.Init(1000, true, 19, INPUT_PULLUP);

  // init display
  Wire.begin(SDA_PIN, SCL_PIN);
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    while (true); 
  }

  // init daisy
  DAISY.init(DAISY_SEED, AUDIO_SR_48K);
  sr = DAISY.get_samplerate();

  static Pad p1 DSY_SDRAM_BSS;
  static Pad p2 DSY_SDRAM_BSS;

  pad1 = &p1;
  pad2 = &p2;

  pad1->Init(sr);
  pad2->Init(sr);

  reverb.Init(sr);

  drawUi();

  DAISY.begin(AudioCallback);
}

void loop() {
  // debounce all buttons
  start_button.Debounce();
  stop_button.Debounce();
  set_button.Debounce();
  prev_button.Debounce();
  next_button.Debounce();

  // handle button press
  bool do_ui_change = false;

  if (start_button.RisingEdge()) {
    active_synth = !active_synth; // just for testing purposes
    SwitchPad();
  }
  if (stop_button.RisingEdge()) {
    // stop everything
    pad1->SetGate(false);
    pad2->SetGate(false);
    is_stopped = true;
    drawUi();
  }
  if (set_button.RisingEdge()) {
    set_type = !set_type;
    do_ui_change = true;
  }
  if (prev_button.RisingEdge()) {
    if (set_type) {
      // from C#4 to C5
      if (next_midi > 61) {
        next_midi--;
      } else {
        next_midi = 72;
      }
    } else {
      if (active_pad < (PAD_COUNT -1)) {
        active_pad++;
      } else {
        active_pad = 0;
      }
    }
    do_ui_change = true;
  }
  if (next_button.RisingEdge()) {
    if (set_type) {
      // from C#4 to C5
      if (next_midi < 72) {
        next_midi++;
      } else {
        next_midi = 61;
      }
    } else {
      if (active_pad > 0) {
        active_pad--;
      } else {
        active_pad = PAD_COUNT - 1;
      }
    }
    do_ui_change = true;
  }

  if (do_ui_change) {
    switch (active_pad) {
      case 0: next_config = deep; break;
      default: next_config = verby; break;
    }
    drawUi();
  }

  delay(5);
}