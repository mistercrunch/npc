#!/bin/bash
# Clean up archives directory by removing binary files, media, and archives
# Keeps only text files and meta.yaml files

set -e

ARCHIVES_DIR="./archives"

echo "=== Archive Cleanup Script ==="
echo ""
echo "This will remove:"
echo "  - Executables (.exe, .com, .dll, .bin, .bat, .cmd, .vbs, .scr)"
echo "  - Archives (.zip, .tar, .gz, .bz2, .xz, .rar, .7z, .arj, .lzh, .arc)"
echo "  - Images (.jpg, .jpeg, .png, .gif, .bmp, .ico, .tif, .tiff, .pcx)"
echo "  - Documents (.pdf, .doc, .docx, .xls, .xlsx, .ppt, .pptx)"
echo "  - Audio (.mp3, .wav, .mid, .midi, .mod, .xm, .s3m, .it, .ogg, .flac, .au, .aif, .aiff)"
echo "  - Video (.avi, .mpg, .mpeg, .mp4, .mov, .wmv, .flv)"
echo "  - Other binary (.obj, .o, .a, .lib, .so, .dylib)"
echo ""
echo "This will PRESERVE:"
echo "  - Text files (.txt, .nfo, .diz, etc.)"
echo "  - meta.yaml files"
echo ""

# Count files before
before=$(find "$ARCHIVES_DIR" -type f | wc -l | tr -d ' ')
echo "Files before cleanup: $before"

# Calculate size before
size_before=$(du -sh "$ARCHIVES_DIR" | cut -f1)
echo "Size before cleanup: $size_before"
echo ""

read -p "Continue with cleanup? (y/N) " -n 1 -r
echo
if [[ ! $REPLY =~ ^[Yy]$ ]]; then
    echo "Cleanup cancelled."
    exit 0
fi

echo ""
echo "Removing unwanted files..."

# Remove all the file types (excluding meta.yaml)
find "$ARCHIVES_DIR" -type f \( \
    -iname "*.exe" -o \
    -iname "*.com" -o \
    -iname "*.dll" -o \
    -iname "*.bin" -o \
    -iname "*.bat" -o \
    -iname "*.cmd" -o \
    -iname "*.vbs" -o \
    -iname "*.scr" -o \
    -iname "*.zip" -o \
    -iname "*.tar" -o \
    -iname "*.gz" -o \
    -iname "*.bz2" -o \
    -iname "*.xz" -o \
    -iname "*.rar" -o \
    -iname "*.7z" -o \
    -iname "*.arj" -o \
    -iname "*.lzh" -o \
    -iname "*.arc" -o \
    -iname "*.jpg" -o \
    -iname "*.jpeg" -o \
    -iname "*.png" -o \
    -iname "*.gif" -o \
    -iname "*.bmp" -o \
    -iname "*.ico" -o \
    -iname "*.tif" -o \
    -iname "*.tiff" -o \
    -iname "*.pcx" -o \
    -iname "*.pdf" -o \
    -iname "*.doc" -o \
    -iname "*.docx" -o \
    -iname "*.xls" -o \
    -iname "*.xlsx" -o \
    -iname "*.ppt" -o \
    -iname "*.pptx" -o \
    -iname "*.mp3" -o \
    -iname "*.wav" -o \
    -iname "*.mid" -o \
    -iname "*.midi" -o \
    -iname "*.mod" -o \
    -iname "*.xm" -o \
    -iname "*.s3m" -o \
    -iname "*.it" -o \
    -iname "*.ogg" -o \
    -iname "*.flac" -o \
    -iname "*.au" -o \
    -iname "*.aif" -o \
    -iname "*.aiff" -o \
    -iname "*.avi" -o \
    -iname "*.mpg" -o \
    -iname "*.mpeg" -o \
    -iname "*.mp4" -o \
    -iname "*.mov" -o \
    -iname "*.wmv" -o \
    -iname "*.flv" -o \
    -iname "*.obj" -o \
    -iname "*.o" -o \
    -iname "*.a" -o \
    -iname "*.lib" -o \
    -iname "*.so" -o \
    -iname "*.dylib" \
\) ! -name "meta.yaml" -delete

echo "Cleanup complete!"
echo ""

# Count files after
after=$(find "$ARCHIVES_DIR" -type f | wc -l | tr -d ' ')
echo "Files after cleanup: $after"

# Calculate files removed
removed=$((before - after))
echo "Files removed: $removed"

# Calculate size after
size_after=$(du -sh "$ARCHIVES_DIR" | cut -f1)
echo "Size after cleanup: $size_after"
echo ""

# Remove empty directories
echo "Removing empty directories..."
find "$ARCHIVES_DIR" -type d -empty -delete
echo "Done!"
