#include "backend.h"
#include "font.xpm" // TODO: remove
#include "userSettings.h"

static Backend_t this;

bool Backend_Init() {
  return false;
}


void Backend_Cleanup() {
}


void Backend_Update() {
}


// Find and store the title of the current ROM
void Backend_GetFileName(const char* path) {
  if (!path)
    return;

  // Get the end of the file name before the '.' of the file extension
  char* end = (char*)(path + strlen(path));
  while (end != path && (*end) != PATH_CHAR && (*end) != '.') {
    end--;
  }
  // If the found character is not '.', then there's probably no file extension
  if (end == path || (*end) != '.')
    end = (char*)(path + strlen(path));

  // Start searching for the start of the title without the directory
  char* start = end;
  while (start != path && (*start) != PATH_CHAR) {
    start--;
  }
  start++; // Moved one too far

  int32_t size = (int32_t)(end - start);
  if (size > 256)
    size = 256;

  for (int i = 0; i < size; i++)
    this.title[i] = start[i];
  this.title[255] = '\0';
}


const char* Backend_OpenFileDialog(const char* title) {
  return "";
}


void Backend_OpenMessageBox(const char* title, const char* message) {
}


void Backend_WriteSave(void* data, uint32_t size) {
  if (!this.saveFile)
    return;

  uint8_t* bytes = (uint8_t*)data;
  for (int32_t i = 0; i < size; i++)
    fputc(bytes[i], this.saveFile);

  uint32_t alignmentSize = (size + 0xf) & ~0xf;
  for (int32_t i = 0; i < (alignmentSize - size); i++)
    fputc(0x00, this.saveFile);

}


void Backend_ReadSave(void* data, uint32_t size) {
  if (!this.saveFile)
    return;

  uint8_t* bytes = (uint8_t*)data;
  for (int32_t i = 0; i < size; i++)
    bytes[i] = fgetc(this.saveFile);

  uint32_t alignmentSize = (size + 0xf) & ~0xf;
  for (int32_t i = 0; i < (alignmentSize - size); i++)
    fgetc(this.saveFile);
}


void Backend_SaveState() {
  char path[256];
  snprintf((char*)&path, 256, "savestates/%s.vfss", (const char*)&this.title);

  VSmile_Log("Saving state from '%s'...", (const char*)&path);
  this.saveFile = fopen(path, "wb+");
  if (!this.saveFile) {
    VSmile_Warning("Save state failed!");
    return;
  }

  Bus_SaveState();
  CPU_SaveState();
  PPU_SaveState();
  SPU_SaveState();
  VSmile_SaveState();
  Controller_SaveState();
  DMA_SaveState();
  GPIO_SaveState();
  Misc_SaveState();
  Timers_SaveState();
  UART_SaveState();

  Backend_WriteSave(&this.currButtons, sizeof(uint32_t));
  Backend_WriteSave(&this.prevButtons, sizeof(uint32_t));

  fclose(this.saveFile);

  VSmile_Log("State saved!");
}


void Backend_LoadState() {
  char path[256];
  snprintf((char*)&path, 256, "savestates/%s.vfss", (const char*)&this.title);

  VSmile_Log("Loading state from '%s'...", (const char*)&path);
  this.saveFile = fopen(path, "rb");
  if (!this.saveFile) {
    VSmile_Warning("Load state failed!");
    return;
  }

  Bus_LoadState();
  CPU_LoadState();
  PPU_LoadState();
  SPU_LoadState();
  VSmile_LoadState();
  Controller_LoadState();
  DMA_LoadState();
  GPIO_LoadState();
  Misc_LoadState();
  Timers_LoadState();
  UART_LoadState();

  Backend_ReadSave(&this.currButtons, sizeof(uint32_t));
  Backend_ReadSave(&this.prevButtons, sizeof(uint32_t));

  fclose(this.saveFile);

  VSmile_Log("State loaded!");
}


float Backend_GetSpeed() {
  return this.emulationSpeed;
}


void Backend_SetSpeed(float newSpeed) {
  this.emulationSpeed = newSpeed;
}


void Backend_SetControlsEnable(bool isEnabled) {
  this.controlsEnabled = isEnabled;
}


uint32_t* Backend_GetScanlinePointer(uint16_t scanlineNum) {
  return PPU_GetPixelBuffer() + (320*scanlineNum);
}


bool Backend_RenderScanline() {
  return !this.oscilloscopeEnabled;
}


bool Backend_GetInput() {
  return true;
}


uint32_t Backend_GetChangedButtons() {
  return this.currButtons ^ this.prevButtons;
}


uint8_t Backend_SetLedStates(uint8_t state) {
  this.currLed = state;
  return this.currLed;
}

void Backend_RenderLeds() {
  if (!this.showLeds)
    return;

  uint8_t alpha = 128;

  if (this.currLed & (1 << VF_LED_RED))
    Backend_SetDrawColor(255, 0, 0, alpha);
  else
    Backend_SetDrawColor(0, 0, 0, alpha);

  Backend_DrawCircle(16, 224, 10);

  if (this.currLed & (1 << VF_LED_YELLOW))
    Backend_SetDrawColor(255, 255, 0, alpha);
  else
    Backend_SetDrawColor(0, 0, 0, alpha);

  Backend_DrawCircle(46, 224, 10);

  if (this.currLed & (1 << VF_LED_BLUE))
    Backend_SetDrawColor(0, 0, 255, alpha);
  else
    Backend_SetDrawColor(0, 0, 0, alpha);

  Backend_DrawCircle(76, 224, 10);

  if (this.currLed & (1 << VF_LED_GREEN))
    Backend_SetDrawColor(0, 255, 0, alpha);
  else
    Backend_SetDrawColor(0, 0, 0, alpha);

  Backend_DrawCircle(106, 224, 10);
}


void Backend_ShowLeds(bool shouldShowLeds) {
  this.showLeds = shouldShowLeds;
}


void Backend_PushAudioSample(float leftSample, float rightSample) {
  if (!this.samplesEmpty && this.sampleHead == this.sampleTail) // Buffer is full
    return;

  this.sampleBuffer[this.sampleHead++] = leftSample;
  this.sampleHead %= MAX_SAMPLES;

  this.sampleBuffer[this.sampleHead++] = rightSample;
  this.sampleHead %= MAX_SAMPLES;

  // this.prevL = (this.prevL+leftSample) * 0.5f;
  // this.sampleBuffer[this.sampleHead++] = this.prevL;
  // this.sampleHead %= MAX_SAMPLES;

  // this.prevR = (this.prevR+rightSample)*0.5f;
  // this.sampleBuffer[this.sampleHead++] = this.prevR;
  // this.sampleHead %= MAX_SAMPLES;

  this.samplesEmpty = false;
}

void Backend_AudioCallback(float* buffer, int numFrames, int numChannels) {
  if (numChannels != 2) {
    VSmile_Error("Audio callback expects 2 channels, %d recieved", numChannels);
    return;
  }

  if (VSmile_GetPaused()) {
    for (int i = 0; i < numFrames; i++) {
      buffer[(i<<1)  ] = 0.0f;
      buffer[(i<<1)+1] = 0.0f;
    }
  } else {
    for (int i = 0; i < numFrames; i++) {
      // Left channel
      if (!this.samplesEmpty) {
        this.filterL = (this.filterL + this.sampleBuffer[this.sampleTail++]) * 0.5f;
        this.sampleTail %= MAX_SAMPLES;
        this.samplesEmpty = (this.sampleHead == this.sampleTail);
      } else {
        this.filterL *= 0.5f;
      }

      // Right channel
      if (!this.samplesEmpty) {
        this.filterR = (this.filterR + this.sampleBuffer[this.sampleTail++]) * 0.5f;
        this.sampleTail %= MAX_SAMPLES;
        this.samplesEmpty = (this.sampleHead == this.sampleTail);
      } else {
        this.filterR *= 0.5f;
      }

      buffer[(i<<1)  ] = this.filterL * 2.0f;
      buffer[(i<<1)+1] = this.filterR * 2.0f;

    }
  }
}

static const uint16_t channelColors[] = {
  0x7c00, 0x83e0, 0x001f, 0xffe0,
  0x7c1f, 0x83ff, 0xfe24, 0xffff,
  0x7c00, 0x83e0, 0x001f, 0xffe0,
  0x7c1f, 0x83ff, 0xfe24, 0xffff
};


void Backend_PushOscilloscopeSample(uint8_t ch, int16_t sample) {
  if (!this.oscilloscopeEnabled)
    return;

  sample = sample / 1200;

  if (this.currSampleX[ch] % 10 == 0) {

    int32_t x = (ch & 0x3) * 80;
    int32_t y = ((ch >> 2) * 60) + 30;

    x += (this.currSampleX[ch] / 10);

    int32_t start, end;
    if (sample > this.prevSample[ch]) {
      start = this.prevSample[ch];
      end = sample;
    } else if (sample < this.prevSample[ch]){
      start = sample;
      end = this.prevSample[ch];
    } else {
      start = sample;
      end = sample+1;
    }

    Backend_SetDrawColor32(RGB5A1_TO_RGBA8(channelColors[ch]));

    for (int32_t i = start; i < end; i++) {
      Backend_SetPixel(x, y+i);
    }

    this.prevSample[ch] = sample;
  }

  this.currSampleX[ch]++;
}


bool Backend_GetOscilloscopeEnabled() {
  return this.oscilloscopeEnabled;
}


void Backend_SetOscilloscopeEnabled(bool shouldShow) {
  this.oscilloscopeEnabled = shouldShow;
}


void Backend_SetScreenFilter(uint8_t filterMode) {
  this.currScreenFilter = filterMode;
}

// Old SDL keycodes
// case SDLK_BACKQUOTE: SDLBackend_ToggleFullscreen(); break;
// case SDLK_1: PPU_ToggleLayer(0); break; // Layer 0
// case SDLK_2: PPU_ToggleLayer(1); break; // Layer 1
// case SDLK_3: PPU_ToggleLayer(2); break; // Sprites
// case SDLK_4: VSmile_TogglePause(); break;
// case SDLK_5: VSmile_Step(); break;
// case SDLK_6: PPU_ToggleSpriteOutlines(); break;
// case SDLK_7: PPU_ToggleFlipVisual(); break;
// case SDLK_8: this.isOscilloscopeView = !this.isOscilloscopeView; break;
// case SDLK_0: VSmile_Reset(); break;
// case SDLK_F10: running = false; break; // Exit
// case SDLK_F1: this.showLed ^=1; break;
// case SDLK_h: this.showHelp ^=1; break;
// case SDLK_g: this.showRegisters ^=1; break;
//
// case SDLK_p: SDLBackend_SetVSync(!this.isVsyncEnabled); break;
//
// case SDLK_UP:    this.currButtons |= (1 << INPUT_UP);     break;
// case SDLK_DOWN:  this.currButtons |= (1 << INPUT_DOWN);   break;
// case SDLK_LEFT:  this.currButtons |= (1 << INPUT_LEFT);   break;
// case SDLK_RIGHT: this.currButtons |= (1 << INPUT_RIGHT);  break;
// case SDLK_SPACE: this.currButtons |= (1 << INPUT_ENTER);  break;
// case SDLK_z:     this.currButtons |= (1 << INPUT_RED);    break;
// case SDLK_x:     this.currButtons |= (1 << INPUT_YELLOW); break;
// case SDLK_v:     this.currButtons |= (1 << INPUT_BLUE);   break;
// case SDLK_c:     this.currButtons |= (1 << INPUT_GREEN);  break;
// case SDLK_a:     this.currButtons |= (1 << INPUT_HELP);   break;
// case SDLK_s:     this.currButtons |= (1 << INPUT_EXIT);   break;
// case SDLK_d:     this.currButtons |= (1 << INPUT_ABC);    break;


void Backend_HandleInput(int32_t keycode, int32_t eventType) {
}


void Backend_UpdateButtonMapping(const char* buttonName, char* mappingText, uint32_t mappingLen) {

}


void Backend_SetDrawColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
  this.drawColor = (a << 24) | (b << 16) | (g << 8) | r;
}


void Backend_SetDrawColor32(uint32_t color) {
  this.drawColor = color;
}


void Backend_SetPixel(int32_t x, int32_t y) {
  // if (x >= 0 && x < 320 && y >= 0 && y < 240)
  //   this.pixelBuffer[y][x] = this.drawColor;
}


void Backend_DrawCircle(int32_t x, int32_t y, uint32_t radius) {
  int32_t offsetx, offsety, d;

  offsetx = 0;
  offsety = radius;
  d = radius - 1;

  while (offsety >= offsetx) {
    Backend_SetPixel(x + offsetx, y + offsety);
    Backend_SetPixel(x + offsety, y + offsetx);
    Backend_SetPixel(x - offsetx, y + offsety);
    Backend_SetPixel(x - offsety, y + offsetx);

    Backend_SetPixel(x + offsetx, y - offsety);
    Backend_SetPixel(x + offsety, y - offsetx);
    Backend_SetPixel(x - offsetx, y - offsety);
    Backend_SetPixel(x - offsety, y - offsetx);

    if (d >= 2*offsetx) {
      d -= 2*offsetx + 1;
      offsetx += 1;
    } else if (d < 2 * (radius - offsety)) {
      d += 2 * offsety - 1;
      offsety -= 1;
    } else {
      d += 2 * (offsety - offsetx -1);
      offsety -= 1;
      offsetx += 1;
    }
  }
}


void Backend_DrawText(int32_t x, int32_t y, const char* text, ...) {
  if (!text)
    return;

  char buffer[256];
  va_list args;
  va_start(args, text);
  vsnprintf(buffer, sizeof(buffer), text, args);
  va_end(args);

  buffer[255] = '\0';

  size_t len = strlen(buffer);
  for(int32_t i = 0 ; i < len; i++) {
    Backend_DrawChar(x, y, buffer[i]);
    x += 11;
  }
}


void Backend_DrawChar(int32_t x, int32_t y, char c) {
  char p;
  for(int32_t i = 0; i < 11; i++)
    for(int32_t j = 0; j < 17; j++) {
      p = font[j+3][((c-32)*11)+i];
      if (p == ' ')
        Backend_SetDrawColor(0xff, 0xff, 0xff, 0xff);
      else
        Backend_SetDrawColor(0x00, 0x00, 0x00, 0x80);

      Backend_SetPixel(x+i, y+j);
    }
}


void Backend_SetKeepAspectRatio(bool keepAR) {
  this.keepAspectRatio = keepAR;
}
