#include "gsswizzle.h"

static const int blockTable32[32] =
{
     0,  1,  4,  5, 16, 17, 20, 21,
     2,  3,  6,  7, 18, 19, 22, 23,
     8,  9, 12, 13, 24, 25, 28, 29,
    10, 11, 14, 15, 26, 27, 30, 31
};

static const int columnTable16[16] =
{
    0,  1,  4,  5,  8,  9, 12, 13,
    2,  3,  6,  7, 10, 11, 14, 15
};

u32 PixelAddress32(int x, int y, u32 bp, u32 bw)
{
    int pageX = x >> 6;
    int pageY = y >> 5;
    int pageIdx = pageY * bw + pageX;

    int blockX = (x >> 3) & 7;
    int blockY = (y >> 3) & 3;
    int blockIdx = blockTable32[blockY * 8 + blockX];

    int columnX = x & 7;
    int columnY = y & 1;
    int columnIdx = (y & 7) >> 1;

    int pixelOffset = columnTable16[columnY * 8 + columnX];

    int addr = pageIdx * 64 * 32;
    addr += blockIdx * 8 * 8;
    addr += columnIdx * 8 * 2;
    addr += pixelOffset;

    return static_cast<u32>(addr);
}

u32 ReadPixel32(const u8* vram, int x, int y, u32 bp, u32 bw)
{
    const u32* vram32 = reinterpret_cast<const u32*>(vram);
    return vram32[PixelAddress32(x, y, bp, bw)];
}
