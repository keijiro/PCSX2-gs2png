// GS Dump file format parsing
#pragma once

#include "types.h"
#include <cstdio>
#include <cstring>

#pragma pack(push, 4)
struct GSDumpHeader
{
    u32 state_version;
    u32 state_size;
    u32 serial_offset;
    u32 serial_size;
    u32 crc;
    u32 screenshot_width;
    u32 screenshot_height;
    u32 screenshot_offset;
    u32 screenshot_size;
};
#pragma pack(pop)

class GSDumpFile
{
public:
    GSDumpFile();
    ~GSDumpFile();

    bool Open(const char* filename);
    void Close();

    const u8* GetVRAM() const { return m_vram; }
    bool IsValid() const { return m_vram != nullptr; }

private:
    static constexpr u32 VRAM_SIZE = 4 * 1024 * 1024;  // 4MB
    static constexpr u32 VRAM_METADATA_SIZE = 509;      // Bytes before VRAM in freezeData

    u8* m_vram;
};
