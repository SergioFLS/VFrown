#include "userSettings.h"

typedef struct {
  const char* name;
  const char* value;
} DefaultEntry_t;

static UserSettings_t this;

static const char* settingsPath = "userSettings.ini";

static const DefaultEntry_t defaults[] = {
  // Theme //
  { "textColor",   "D2D2D2"     },
  { "headerColor", "333333"     },
  { "borderColor", "2E2E2E"     },
  { "windowColor", "394347"     },
  { "buttonColor", "30536F"     },
  { "c0up",        "K0B1097FFF" }, // Up key
  { "c0down",      "K0B1087FFF" }, // Down key
  { "c0left",      "K0B1077FFF" }, // Left key
  {"c0right",      "K0B1067FFF" }, // Right key
  {"c0red",        "K0B05A7FFF" }, // Z key
  {"c0yellow",     "K0B0587FFF" }, // X key
  {"c0blue",       "K0B0437FFF" }, // C key
  {"c0green",      "K0B0567FFF" }, // V key
  {"c0enter",      "K0B0207FFF" }, // Space
  {"c0help",       "K0B0417FFF" }, // A key
  {"c0exit",       "K0B0537FFF" }, // S key
  {"c0abc",        "K0B0447FFF" }, // D key
  {"c1up",         ""           },
  {"c1down",       ""           },
  {"c1left",       ""           },
  {"c1right",      ""           },
  {"c1red",        ""           },
  {"c1yellow",     ""           },
  {"c1blue",       ""           },
  {"c1green",      ""           },
  {"c1enter",      ""           },
  {"c1help",       ""           },
  {"c1exit",       ""           },
  {"c1abc",        ""           },

};
// 109, 108, 107,106, 05a,058,043,056,041,053,044,020

bool UserSettings_Init() {
  return false;
}


void UserSettings_Cleanup() {
}

void UserSettings_WriteString(const char* name, char* value, uint32_t size) {
}


bool UserSettings_ReadString(const char* name, char* value, uint32_t bufferSize) {
  return false;
}
