#!/bin/bash
# Script to convert PNG to ICNS (macOS icon format)
# Run this on a macOS system or use an online converter
#
# Usage: ./create-icns.sh <input.png> <output.icns>
#
# The input PNG should be at least 1024x1024 pixels for best quality.

set -e

INPUT="${1:-assets/nsbaci-512x512.png}"
OUTPUT="${2:-assets/nsbaci.icns}"

if [[ ! -f "$INPUT" ]]; then
    echo "Error: Input file '$INPUT' not found"
    echo "Usage: $0 <input.png> <output.icns>"
    exit 1
fi

# Create temporary iconset directory
ICONSET="nsbaci.iconset"
mkdir -p "$ICONSET"

# Generate all required sizes
# macOS requires specific sizes for the iconset
sips -z 16 16     "$INPUT" --out "$ICONSET/icon_16x16.png"
sips -z 32 32     "$INPUT" --out "$ICONSET/icon_16x16@2x.png"
sips -z 32 32     "$INPUT" --out "$ICONSET/icon_32x32.png"
sips -z 64 64     "$INPUT" --out "$ICONSET/icon_32x32@2x.png"
sips -z 128 128   "$INPUT" --out "$ICONSET/icon_128x128.png"
sips -z 256 256   "$INPUT" --out "$ICONSET/icon_128x128@2x.png"
sips -z 256 256   "$INPUT" --out "$ICONSET/icon_256x256.png"
sips -z 512 512   "$INPUT" --out "$ICONSET/icon_256x256@2x.png"
sips -z 512 512   "$INPUT" --out "$ICONSET/icon_512x512.png"
sips -z 1024 1024 "$INPUT" --out "$ICONSET/icon_512x512@2x.png" 2>/dev/null || \
    cp "$INPUT" "$ICONSET/icon_512x512@2x.png"

# Convert iconset to icns
iconutil -c icns "$ICONSET" -o "$OUTPUT"

# Cleanup
rm -rf "$ICONSET"

echo "Created $OUTPUT successfully!"
