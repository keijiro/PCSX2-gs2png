// Complete PS2 GS VRAM swizzling implementation for PSMCT32
// Extracted from PCSX2 (GPL-3.0+)
#include "gsswizzle.h"

// Block swizzle table for 32-bit format (4x8)
static const u8 blockTable32[4][8] =
{
    {  0,  1,  4,  5, 16, 17, 20, 21},
    {  2,  3,  6,  7, 18, 19, 22, 23},
    {  8,  9, 12, 13, 24, 25, 28, 29},
    { 10, 11, 14, 15, 26, 27, 30, 31}
};

// Column lookup table for pixels within blocks
static const u8 columnTable32[8][8] =
{
    {  0,  1,  4,  5,  8,  9, 12, 13 },
    {  2,  3,  6,  7, 10, 11, 14, 15 },
    { 16, 17, 20, 21, 24, 25, 28, 29 },
    { 18, 19, 22, 23, 26, 27, 30, 31 },
    { 32, 33, 36, 37, 40, 41, 44, 45 },
    { 34, 35, 38, 39, 42, 43, 46, 47 },
    { 48, 49, 52, 53, 56, 57, 60, 61 },
    { 50, 51, 54, 55, 58, 59, 62, 63 },
};

// PSMCT32: Pages are 64x32, blocks are 8x8, pixels are 32-bit
static constexpr int BLOCK_WIDTH = 8;
static constexpr int BLOCK_HEIGHT = 8;

void InitSwizzleTables()
{
    // Tables are static const, no initialization needed for direct implementation
}

u32 PixelAddress32(int x, int y, u32 bp, u32 bw)
{
    // Pure pxOffset implementation with bp/bw support
    // PSMCT32: Pages are 64x32, blocks are 8x8, pixels are 32-bit
    constexpr int blockSize = 64;     // 8x8 pixels per block
    constexpr int pageSize = 2048;    // 64x32 pixels per page
    constexpr int pageWidth = 64;     // Page width in pixels

    // Calculate page coordinates
    int pageX = x / pageWidth;
    int pageY = y / 32;               // 32 is page height
    int subpageX = x % pageWidth;
    int subpageY = y % 32;

    // Get block ID (using y within page)
    int blockX = subpageX / BLOCK_WIDTH;
    int blockY = subpageY / BLOCK_HEIGHT;
    int blockID = blockTable32[blockY][blockX];

    // Get pixel offset (using position within block)
    int bx = subpageX % BLOCK_WIDTH;
    int by = subpageY % BLOCK_HEIGHT;
    int pixelOffset = columnTable32[by][bx];

    // Calculate page number considering bp and bw
    // bp is base pointer, bw is buffer width in pages
    int pageNum = static_cast<int>(bp) * 64 / pageSize;  // Convert bp to pages
    pageNum += pageY * bw + pageX;                         // Add 2D page offset

    // Calculate final address
    int addr = pageNum * pageSize + blockID * blockSize + pixelOffset;

    return static_cast<u32>(addr);
}

u32 ReadPixel32(const u8* vram, int x, int y, u32 bp, u32 bw)
{
    u32 addr = PixelAddress32(x, y, bp, bw);

    // VRAM is stored as 32-bit values
    const u32* vram32 = reinterpret_cast<const u32*>(vram);
    return vram32[addr];
}
