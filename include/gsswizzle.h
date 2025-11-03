#pragma once

#include "types.h"

u32 PixelAddress32(int x, int y, u32 bp, u32 bw);
u32 ReadPixel32(const u8* vram, int x, int y, u32 bp, u32 bw);
