#include "input.h"
#include "userSettings.h"
#include <3ds.h>

#define KEYDEF(keycode) \
  (Mapping_t){\
    .deviceType=INPUT_DEVICETYPE_KEYBOARD,\
    .deviceID=0,\
    .inputType=INPUT_INPUTTYPE_BUTTON,\
    .inputID=keycode,\
    .value=0x7fff,\
  }

static Input_t this;

static const char* mappingStrings[2][NUM_INPUTS] = {
  {
    "c0up",    "c0down",   "c0left", "c0right",
    "c0red",   "c0yellow", "c0blue", "c0green",
    "c0enter", "c0help",   "c0exit", "c0abc",
  },
  {
    "c1up",    "c1down",   "c1left", "c1right",
    "c1red",   "c1yellow", "c1blue", "c1green",
    "c1enter", "c1help",   "c1exit", "c1abc",
  },
};

bool Input_Init() {
  this.gamepadID[0] = -1; // why -1 if this is an unsigned int?
  this.gamepadID[1] = -1;
  this.inputEnabled = true;
  return true;
}


void Input_Cleanup() {
}


void Input_Update() {
  printf("chb? %08X inpe? %d\n", Input_GetChangedButtons(0), this.inputEnabled);
  if (this.inputEnabled) {
    if (Input_GetChangedButtons(0)) {
      Controller_UpdateButtons(0, this.curr[0]);
      printf("Called Input_Update() %08X\n", this.curr[0]);
    }
  }

  this.prev[0] = this.curr[0];

  u32 kHeld = hidKeysHeld();
  uint32_t buttons = 0;

  if (kHeld & KEY_UP) buttons |= (1 << INPUT_UP);
  if (kHeld & KEY_DOWN) buttons |= (1 << INPUT_DOWN);
  if (kHeld & KEY_LEFT) buttons |= (1 << INPUT_LEFT);
  if (kHeld & KEY_RIGHT) buttons |= (1 << INPUT_RIGHT);
  if (kHeld & KEY_X) buttons |= (1 << INPUT_ENTER);

  this.curr[0] = buttons;
  printf("w? %08X %08X %08X %08X\n", buttons, this.curr[0], this.prev[0], Input_GetChangedButtons(0));
}


uint32_t Input_GetChangedButtons(uint8_t ctrlNum) {
  return this.curr[ctrlNum] ^ this.prev[ctrlNum];
}


void Input_SetControlsEnable(bool isEnabled) {
  this.inputEnabled = isEnabled;
}


void Input_KeyboardMouseEvent(void* event) {
}


// If event is mapped to input, then set virtual controller button
static void _Input_CheckInput(
  uint8_t deviceType, uint8_t deviceID, uint8_t inputType, uint16_t inputID, int16_t value
) {

  // printf(
  //   "deviceType=%d, deviceID=%d, inputType=%d, inputID=%d, value=%d\n",
  //   deviceType, deviceID, inputType, inputID, value
  // );

  Mapping_t event = {
    .deviceType = deviceType,
    .deviceID   = deviceID,
    .inputType  = inputType,
    .inputID    = inputID,
    .value      = value,
  };
  this.lastEvent = event;

  for (uint32_t p = 0; p < 2; p++) {
    for (uint32_t i = 0; i < NUM_INPUTS; i++) {
      Mapping_t mapping = this.controllerMappings[p][i];
      // printf("%08x, %08x\n", mapping.raw, event.raw);
      if ((mapping.raw & 0xffff) == (event.raw & 0xffff)) {
        int16_t threshold = mapping.value;
        if (((threshold < 0) && (value <= threshold)) || ((threshold > 0) && value >= threshold)) {
          this.curr[p] |=  (1 << i);
        } else {
          this.curr[p] &= ~(1 << i);
        }
      }
    }
  }

}

void Input_SaveMappings() {
}