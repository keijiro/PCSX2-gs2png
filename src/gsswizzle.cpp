// Minimal PS2 GS VRAM swizzling implementation for PSMCT32
// Extracted and simplified from PCSX2 (GPL-3.0+)
#include "gsswizzle.h"

// Block swizzle table for 32-bit format (4x8)
static const u8 blockTable32_data[4][8] =
{
    {  0,  1,  4,  5, 16, 17, 20, 21},
    {  2,  3,  6,  7, 18, 19, 22, 23},
    {  8,  9, 12, 13, 24, 25, 28, 29},
    { 10, 11, 14, 15, 26, 27, 30, 31}
};

// Column lookup table
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
static constexpr int PAGE_WIDTH = 64;
static constexpr int PAGE_HEIGHT = 32;
static constexpr int BLOCK_WIDTH = 8;
static constexpr int BLOCK_HEIGHT = 8;

void InitSwizzleTables()
{
    // Tables are static const, no initialization needed
}

u32 PixelAddress32(int x, int y, u32 bp, u32 bw)
{
    // bp = base pointer in 256-byte blocks
    // bw = buffer width in 64-pixel units

    // Convert bp from blocks to pixels (256 bytes = 64 pixels at 32bpp)
    u32 base = bp * 64;

    // Page coordinates
    int page_x = x / PAGE_WIDTH;
    int page_y = y / PAGE_HEIGHT;

    // Position within page
    int px = x % PAGE_WIDTH;
    int py = y % PAGE_HEIGHT;

    // Block coordinates within page
    int block_x = px / BLOCK_WIDTH;
    int block_y = py / BLOCK_HEIGHT;

    // Pixel coordinates within block
    int bx = px % BLOCK_WIDTH;
    int by = py % BLOCK_HEIGHT;

    // Page offset: each page is 64x32 = 2048 pixels
    // Pages are laid out horizontally first, then vertically
    u32 page_num = page_y * bw + page_x;
    u32 page_offset = page_num * (PAGE_WIDTH * PAGE_HEIGHT);

    // Block offset within page
    // Blocks are swizzled using blockTable32
    u32 block_num = blockTable32_data[block_y % 4][block_x];
    u32 block_offset = block_num * (BLOCK_WIDTH * BLOCK_HEIGHT);

    // Pixel offset within block
    // Pixels are swizzled using columnTable32
    u32 pixel_offset = columnTable32[by][bx];

    // Final address
    return base + page_offset + block_offset + pixel_offset;
}

u32 ReadPixel32(const u8* vram, int x, int y, u32 bp, u32 bw)
{
    u32 addr = PixelAddress32(x, y, bp, bw);

    // VRAM is stored as 32-bit values
    const u32* vram32 = reinterpret_cast<const u32*>(vram);
    return vram32[addr];
}
