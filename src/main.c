#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <3ds.h>

#include "core/vsmile.h"

static inline size_t xy2index_native(unsigned int x, unsigned int y) {
	return x+(y*240);
}

static inline size_t xy2index_rotated(unsigned int x, unsigned int y) {
	return (320*((240-1)-x))+y;
}

static void copyscreen(u8* dest, uint32_t* src) {
	for (size_t y = 0; y < 320; y++)
	{
		for (size_t x = 0; x < 240; x++)
		{
			dest[(xy2index_native(x, y)*3)+0] = (src[xy2index_rotated(x, y)] & 0x00FF0000) >> 16;
			dest[(xy2index_native(x, y)*3)+1] = (src[xy2index_rotated(x, y)] & 0x0000FF00) >> 8;
			dest[(xy2index_native(x, y)*3)+2] = (src[xy2index_rotated(x, y)] & 0x000000FF);
		}
		
	}	
}

int main(int argc, char* argv[])
{
	gfxInitDefault();
	consoleInit(GFX_TOP, NULL);
	VSmile_Error("VSmile_Error test");
	// Emulator core
	if (!VSmile_Init()) {
		VSmile_Error("Failed to initialize emulation core");
	}

	VSmile_LoadSysRom("/vfrown/sysrom/sysrom.bin");

	VSmile_LoadROM("/vfrown/abcpark.bin");
	VSmile_SetPause(false);
	VSmile_SetRegion(0xf);
	VSmile_SetIntroEnable(true);

	VSmile_Reset();

	printf("vforwn\n");

	// Main loop
	while (aptMainLoop())
	{
		u8* fb = gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT, NULL, NULL);

		copyscreen(fb, PPU_GetPixelBuffer());

		gspWaitForVBlank();
		gfxFlushBuffers();
		gfxSwapBuffers();
		hidScanInput();

		// Your code goes here
		VSmile_RunFrame();
		u32 kDown = hidKeysDown();
		if (kDown & KEY_START)
			break; // break in order to return to hbmenu
	}

	gfxExit();
	return 0;
}
