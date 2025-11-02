#!/usr/bin/env python3
"""
PCSX2 GS Dump to PNG Converter

Extracts VRAM data from PCSX2 GS dump files and converts them to PNG images.
"""

import argparse
import sys
from PIL import Image


def extract_vram_to_png(input_file, output_file, width=640, force_alpha=False):
    """
    Extract VRAM data from a GS dump file and save it as a PNG image.

    Args:
        input_file: Path to the input .gs file
        output_file: Path to the output .png file
        width: Image width in pixels (default: 640)
        force_alpha: Force alpha channel to 255 for all pixels (default: False)
    """
    vram_size = 4 * 1024 * 1024  # 4MB PS2 VRAM
    vram_offset = 54  # Fixed offset where VRAM data starts
    bytes_per_pixel = 4  # RGBA

    # Calculate dimensions
    total_pixels = vram_size // bytes_per_pixel
    height = (total_pixels + width - 1) // width  # Round up

    print(f"Reading VRAM from: {input_file}")
    print(f"VRAM offset: {vram_offset} bytes (0x{vram_offset:X})")
    print(f"Image dimensions: {width}x{height}")
    if force_alpha:
        print(f"Alpha channel: Forced to 255")

    try:
        with open(input_file, 'rb') as f:
            # Seek to VRAM data
            f.seek(vram_offset)
            vram_data = f.read(vram_size)

            if len(vram_data) < vram_size:
                print(f"Warning: File contains less VRAM data than expected", file=sys.stderr)
                print(f"  Expected: {vram_size} bytes", file=sys.stderr)
                print(f"  Got: {len(vram_data)} bytes", file=sys.stderr)
                # Pad with zeros
                vram_data += b'\x00' * (vram_size - len(vram_data))

        # Create image
        img = Image.new('RGBA', (width, height), (0, 0, 0, 255))
        pixels = []

        # Convert VRAM bytes to RGBA pixels
        for i in range(0, vram_size, bytes_per_pixel):
            if i + 3 < len(vram_data):
                r, g, b, a = vram_data[i:i+4]
                # Force alpha to 255 if requested
                if force_alpha:
                    a = 255
                pixels.append((r, g, b, a))
            else:
                # Fill remaining pixels with black
                pixels.append((0, 0, 0, 255))

        img.putdata(pixels)
        img.save(output_file)

        print(f"Successfully saved PNG to: {output_file}")

    except FileNotFoundError:
        print(f"Error: Input file not found: {input_file}", file=sys.stderr)
        sys.exit(1)
    except IOError as e:
        print(f"Error: Failed to read/write file: {e}", file=sys.stderr)
        sys.exit(1)


def main():
    parser = argparse.ArgumentParser(
        description='Convert PCSX2 GS dump VRAM to PNG image',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog='''
Examples:
  %(prog)s input.gs output.png
  %(prog)s input.gs output.png --width 1024
  %(prog)s input.gs output.png -w 512 --force-alpha
        '''
    )

    parser.add_argument('input', help='Input GS dump file (.gs)')
    parser.add_argument('output', help='Output PNG file (.png)')
    parser.add_argument(
        '-w', '--width',
        type=int,
        default=640,
        help='Image width in pixels (default: 640)'
    )
    parser.add_argument(
        '--force-alpha',
        action='store_true',
        help='Force alpha channel to 255 for all pixels (prevents transparency)'
    )

    args = parser.parse_args()

    # Validate width
    if args.width <= 0:
        print(f"Error: Width must be positive (got {args.width})", file=sys.stderr)
        sys.exit(1)

    extract_vram_to_png(args.input, args.output, args.width, args.force_alpha)


if __name__ == '__main__':
    main()
