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
static constexpr int PAGE_WIDTH = 64;
static constexpr int PAGE_HEIGHT = 32;
static constexpr int BLOCK_WIDTH = 8;
static constexpr int BLOCK_HEIGHT = 8;
static constexpr int GS_MAX_PAGES = 512;

// Pre-computed column offset table (32 entries for height of page)
static int pixelColOffset32[32];

// Pre-computed row offset table (64 entries for width of page)
static int pixelRowOffset32[64];

// Helper function to calculate pixel offset using block and column tables
static int pxOffset(int x, int y)
{
    constexpr int blockSize = 64;     // 8x8 pixels per block
    constexpr int pageSize = 2048;    // 64x32 pixels per page
    constexpr int pageWidth = 64;     // 64 pixels wide

    int pageX = x / pageWidth;
    int subpageX = x % pageWidth;
    int blockID = blockTable32[y / BLOCK_HEIGHT][subpageX / BLOCK_WIDTH];
    int sublockOffset = columnTable32[y % BLOCK_HEIGHT][subpageX % BLOCK_WIDTH];

    return pageX * pageSize + blockID * blockSize + sublockOffset;
}

void InitSwizzleTables()
{
    // Generate column offset table (for y coordinates within a page)
    for (int y = 0; y < 32; y++)
    {
        pixelColOffset32[y] = pxOffset(0, y);
    }

    // Generate row offset table (for x coordinates within a page)
    int base = pxOffset(0, 0);
    for (int x = 0; x < 64; x++)
    {
        pixelRowOffset32[x] = pxOffset(x % 2048, 0) - base;
    }
}

u32 PixelAddress32(int x, int y, u32 bp, u32 bw)
{
    // bp = base pointer in 256-byte blocks
    // bw = buffer width in 64-pixel units

    // Page shift values for PSMCT32: pageShiftX=6, pageShiftY=5
    // This means pages are 2^6 = 64 pixels wide, 2^5 = 32 pixels high
    constexpr int pageShiftX = 6;
    constexpr int pageShiftY = 5;
    constexpr int pageMaskY = (1 << pageShiftY) - 1;  // 31

    // Calculate base address from bp
    // bp is in 256-byte blocks, shift by (6+5-5) = 6 to convert to pixels
    int base = static_cast<int>(bp) << (pageShiftX + pageShiftY - 5);

    // Add offset for pages above current row
    base += ((y & ~pageMaskY) * bw) << pageShiftX;

    // Wrap within VRAM bounds
    base &= (GS_MAX_PAGES << (pageShiftX + pageShiftY)) - 1;

    // Add column offset within page
    base += pixelColOffset32[y & pageMaskY];

    // Add row offset
    int result = base + pixelRowOffset32[x % 64];

    // Handle multiple pages horizontally
    result += (x / 64) * (PAGE_WIDTH * PAGE_HEIGHT);

    return static_cast<u32>(result);
}

u32 ReadPixel32(const u8* vram, int x, int y, u32 bp, u32 bw)
{
    u32 addr = PixelAddress32(x, y, bp, bw);

    // VRAM is stored as 32-bit values
    const u32* vram32 = reinterpret_cast<const u32*>(vram);
    return vram32[addr];
}
