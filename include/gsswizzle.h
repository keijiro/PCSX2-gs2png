// Minimal PS2 GS VRAM swizzling implementation for PSMCT32
// Extracted from PCSX2 (GPL-3.0+)
#pragma once

#include "types.h"

// Block swizzle table for 32-bit pixels
struct BlockSwizzleTable32
{
    u8 value[4][8];
};

// Initialize swizzle tables for PSMCT32
void InitSwizzleTables();

// Get pixel address in VRAM for PSMCT32 format
// x, y: pixel coordinates
// bp: base pointer (usually 0 for full VRAM dump)
// bw: buffer width in 64-pixel units (e.g., 10 for 640 pixels)
u32 PixelAddress32(int x, int y, u32 bp, u32 bw);

// Read a 32-bit pixel from VRAM
// Returns RGBA8888 pixel value
u32 ReadPixel32(const u8* vram, int x, int y, u32 bp, u32 bw);
