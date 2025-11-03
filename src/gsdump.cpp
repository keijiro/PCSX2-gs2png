// GS Dump file format parsing implementation
#include "gsdump.h"
#include <cstdlib>

GSDumpFile::GSDumpFile()
    : m_vram(nullptr)
{
}

GSDumpFile::~GSDumpFile()
{
    Close();
}

bool GSDumpFile::Open(const char* filename)
{
    Close();

    FILE* fp = fopen(filename, "rb");
    if (!fp)
        return false;

    // Read fake CRC
    u32 fake_crc;
    if (fread(&fake_crc, sizeof(u32), 1, fp) != 1 || fake_crc != 0xFFFFFFFF)
    {
        fclose(fp);
        return false;
    }

    // Read header size
    u32 header_size;
    if (fread(&header_size, sizeof(u32), 1, fp) != 1)
    {
        fclose(fp);
        return false;
    }

    // Read GSDumpHeader
    GSDumpHeader header;
    if (fread(&header, sizeof(GSDumpHeader), 1, fp) != 1)
    {
        fclose(fp);
        return false;
    }

    // Calculate freezeData offset
    // freezeData starts after: fake_crc (4) + header_size_field (4) + header_size
    u32 freeze_data_offset = 4 + 4 + header_size;

    // VRAM starts at freezeData + metadata
    u32 vram_offset = freeze_data_offset + VRAM_METADATA_SIZE;

    // Seek to VRAM
    if (fseek(fp, vram_offset, SEEK_SET) != 0)
    {
        fclose(fp);
        return false;
    }

    // Allocate and read VRAM
    m_vram = (u8*)malloc(VRAM_SIZE);
    if (!m_vram)
    {
        fclose(fp);
        return false;
    }

    size_t read = fread(m_vram, 1, VRAM_SIZE, fp);
    if (read < VRAM_SIZE)
    {
        // Pad with zeros if file is short
        memset(m_vram + read, 0, VRAM_SIZE - read);
    }

    fclose(fp);
    return true;
}

void GSDumpFile::Close()
{
    if (m_vram)
    {
        free(m_vram);
        m_vram = nullptr;
    }
}
