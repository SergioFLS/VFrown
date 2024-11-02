#ifndef USERSETTINGS_H
#define USERSETTINGS_H

#include "../common.h"
#include "../core/vsmile.h"
#include "backend.h"
#include "libs.h"

typedef struct {
  void* ini;
} UserSettings_t;

bool UserSettings_Init();
void UserSettings_Cleanup();

bool UserSettings_ReadString(const char* name, char* value, uint32_t bufferSize);

void UserSettings_WriteString(const char* name, char* value, uint32_t size);

#endif // USERSETTINGS_H
