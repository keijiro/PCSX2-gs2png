// gs2png - Convert PCSX2 GS Dump VRAM to PNG
#include "gsdump.h"
#include "gsswizzle.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

void PrintUsage(const char* prog)
{
    printf("Usage: %s <input.gs> <output.png> [options]\n", prog);
    printf("\n");
    printf("Options:\n");
    printf("  -w, --width <pixels>    Output image width (default: 640)\n");
    printf("  --force-alpha           Force alpha channel to 255 (prevents transparency)\n");
    printf("  -h, --help              Show this help message\n");
    printf("\n");
    printf("Examples:\n");
    printf("  %s input.gs output.png\n", prog);
    printf("  %s input.gs output.png --width 1024\n", prog);
    printf("  %s input.gs output.png -w 512 --force-alpha\n", prog);
    printf("\n");
}

int main(int argc, char** argv)
{
    if (argc < 3)
    {
        PrintUsage(argv[0]);
        return 1;
    }

    const char* input_file = argv[1];
    const char* output_file = argv[2];
    int width = 640;
    bool force_alpha = false;

    // Parse command line options
    for (int i = 3; i < argc; i++)
    {
        if (strcmp(argv[i], "-w") == 0 || strcmp(argv[i], "--width") == 0)
        {
            if (i + 1 < argc)
            {
                width = atoi(argv[++i]);
                if (width <= 0)
                {
                    fprintf(stderr, "Error: Width must be positive\n");
                    return 1;
                }
            }
            else
            {
                fprintf(stderr, "Error: --width requires an argument\n");
                return 1;
            }
        }
        else if (strcmp(argv[i], "--force-alpha") == 0)
        {
            force_alpha = true;
        }
        else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
        {
            PrintUsage(argv[0]);
            return 0;
        }
        else
        {
            fprintf(stderr, "Error: Unknown option: %s\n", argv[i]);
            PrintUsage(argv[0]);
            return 1;
        }
    }

    // Open GS dump file
    printf("Reading VRAM from: %s\n", input_file);

    GSDumpFile dump;
    if (!dump.Open(input_file))
    {
        fprintf(stderr, "Error: Failed to open GS dump file: %s\n", input_file);
        return 1;
    }

    printf("VRAM loaded successfully\n");

    // Initialize swizzle tables
    InitSwizzleTables();

    // VRAM parameters
    constexpr u32 VRAM_SIZE = 4 * 1024 * 1024;
    constexpr u32 BYTES_PER_PIXEL = 4;
    const u32 total_pixels = VRAM_SIZE / BYTES_PER_PIXEL;
    const int height = (total_pixels + width - 1) / width;

    printf("Image dimensions: %dx%d\n", width, height);
    if (force_alpha)
        printf("Alpha channel: Forced to 255\n");

    // Allocate output image buffer (RGBA)
    std::vector<u8> image(width * height * 4);

    // Buffer width in 64-pixel units
    // For full VRAM dump, we assume framebuffer width of 1024
    const u32 buffer_width = 1024 / 64;  // = 16

    // Deswizzle VRAM to image
    printf("Deswizzling VRAM...\n");

    const u8* vram = dump.GetVRAM();

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            // Calculate linear position
            int pixel_index = y * width + x;

            // Check if we're within VRAM bounds
            if (pixel_index < (int)total_pixels)
            {
                // Read pixel from swizzled VRAM
                u32 pixel = ReadPixel32(vram, x, y, 0, buffer_width);

                // Extract RGBA components
                u8 r = (pixel >> 0) & 0xFF;
                u8 g = (pixel >> 8) & 0xFF;
                u8 b = (pixel >> 16) & 0xFF;
                u8 a = (pixel >> 24) & 0xFF;

                // Force alpha if requested
                if (force_alpha)
                    a = 255;

                // Write to output buffer
                int out_index = pixel_index * 4;
                image[out_index + 0] = r;
                image[out_index + 1] = g;
                image[out_index + 2] = b;
                image[out_index + 3] = a;
            }
            else
            {
                // Fill remaining pixels with black
                int out_index = pixel_index * 4;
                image[out_index + 0] = 0;
                image[out_index + 1] = 0;
                image[out_index + 2] = 0;
                image[out_index + 3] = 255;
            }
        }
    }

    // Write PNG
    printf("Writing PNG to: %s\n", output_file);

    if (!stbi_write_png(output_file, width, height, 4, image.data(), width * 4))
    {
        fprintf(stderr, "Error: Failed to write PNG file: %s\n", output_file);
        return 1;
    }

    printf("Successfully saved PNG\n");

    return 0;
}
