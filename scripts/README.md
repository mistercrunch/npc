# Scripts Directory

This directory contains utility scripts for managing the PhileZ project.

## Archive Management

### cleanup-archives.sh
Removes binary files, media, and archives from the `archives/` directory to reduce repository size.

**What it removes:**
- Executables (.exe, .com, .dll, .bin, .bat, .cmd, .vbs, .scr)
- Archives (.zip, .tar, .gz, .bz2, .xz, .rar, .7z, .arj, .lzh, .arc)
- Images (.jpg, .jpeg, .png, .gif, .bmp, .ico, .tif, .tiff, .pcx)
- Documents (.pdf, .doc, .docx, .xls, .xlsx, .ppt, .pptx)
- Audio (.mp3, .wav, .mid, .midi, .mod, .xm, .s3m, .it, .ogg, .flac, .au, .aif, .aiff)
- Video (.avi, .mpg, .mpeg, .mp4, .mov, .wmv, .flv)
- Other binaries (.obj, .o, .a, .lib, .so, .dylib)

**What it preserves:**
- All text files (.txt, .nfo, .diz, numeric extensions, etc.)
- **meta.yaml files** (ezine metadata)

**Expected savings:** ~774MB (65% reduction from 1.2GB to ~400MB)

**Usage:**
```bash
bash scripts/cleanup-archives.sh
```

The script will:
1. Show statistics about what will be removed
2. Ask for confirmation
3. Delete the files
4. Show before/after comparison
5. Clean up empty directories

## Content Processing (Legacy - For Reference)

### convert.mjs
Converts NPC (Northern Phun Co.) issues from archives/ to pages/issues/.
This was used during the original Next.js setup and is kept for reference.

**Note:** The current Astro architecture loads content dynamically from `archives/`, so this script is no longer needed for normal operation.

### fetch-ezine.mjs
Downloads and processes ezines from autistici.org/ezine/.

**Features:**
- Downloads archives (.zip, .tar, .gz)
- Extracts and converts CP437 to UTF-8
- Creates folder structure in archives/
- Handles numeric file extensions (e.g., "file.002")
- Binary detection and filtering
- Does NOT delete existing files (including meta.yaml)

**Usage:**
```bash
node scripts/fetch-ezine.mjs <ezine-name>
# Example: node scripts/fetch-ezine.mjs phrack
```

**Note:** This script is retained for historical purposes. The archives are largely complete and we're moving toward checking them into git after cleanup.

## Important Notes

- **meta.yaml preservation:** All scripts are designed to preserve `meta.yaml` files
- **Binary files:** Already filtered during initial processing - cleanup removes stragglers
- **Git-friendly:** After cleanup, the archives directory will be suitable for version control
