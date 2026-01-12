# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

PhileZ is a static site preserving BBS-era underground ezines from the 1980s-2000s. Built with Next.js and Nextra, it converts legacy CP437 DOS-encoded text files to modern UTF-8 web format while preserving ASCII art.

**Scale**: Designed to handle ~400 ezines from autistici.org/ezine/ archive (40hex, Phrack, LOD, CUD, etc.)

**Original focus**: Northern Phun Co. (NPC) - Quebec's bilingual underground e-zine from 1992-1994, now expanded to preserve the entire underground ezine archive.

**Performance optimization**: Ezine content pages use minimal MDX wrappers (~120 bytes each) that load text content client-side from `public/data/`. This dramatically reduces build time and memory usage while maintaining the Nextra theme.

## Development Commands

```bash
# Install dependencies
npm install

# Start dev server (http://localhost:3000)
npm run dev

# Build static site (outputs to /out directory)
# Requires 64GB RAM allocated (60GB heap)
npm run build

# Quick test build with subset of ezines (~10 ezines, 4GB heap)
npm run build:test

# Serve built site locally
npm run serve

# Reprocess all downloaded ezines to HTML (skips download)
npm run reprocess
```

### Test/Dev Mode

For faster iteration, work with a small subset of ezines:

```bash
# ONE-TIME: Backup full pages/ to pages-full/
npm run backup:pages

# Switch to test mode (only 10 ezines from .ezines-test)
npm run setup:test

# Now dev/build will be fast
npm run dev           # Fast dev server
npm run build:test    # Quick build (~30 seconds)

# Switch back to all ezines
npm run setup:full
npm run build         # Full production build
```

Edit `.ezines-test` to customize which ezines are included in test mode.

## Architecture

### Next.js + Nextra Setup
- **Framework**: Next.js 14 with static export (`output: 'export'`)
- **Theme**: nextra-theme-docs for documentation-style layout
- **Deployment**: GitHub Pages with `/philez` base path (see next.config.mjs:13-14)
- **Config**: theme.config.tsx defines site appearance, nav, and metadata

### Content Structure
```
pages/
  ├── index.mdx              # Homepage (MDX)
  ├── _meta.json             # Top-level navigation
  ├── issues/                # NPC ezine issues (15 total)
  │   ├── _meta.json         # Issue navigation
  │   └── npc-001.mdx → npc-015.mdx  # (MDX - legacy)
  └── ezines/                # Other ezines
      ├── _meta.json         # Ezine list navigation
      ├── 40hex/             # Each ezine has its own folder
      │   ├── index.mdx      # Index page (MDX)
      │   ├── _meta.json     # Issue/file navigation
      │   ├── 40hex-01/      # Issue folders
      │   │   ├── file1.html # Content files (HTML, not MDX!)
      │   │   └── file2.html
      │   └── ...
      └── noway/
```

**File formats**:
- **MDX wrappers** (.mdx): Minimal wrappers that use `<TextFileViewer>` component
- **Text data** (.txt): Raw UTF-8 content in `public/data/{ezine}/{issue}/`
- **Navigation** (.mdx): Index pages and category pages
- **Metadata** (_meta.json): Nextra navigation structure

**Navigation pattern**: Each folder needs a `_meta.json` file mapping slugs to display names for Nextra's sidebar.

### Content Scripts

#### scripts/convert.mjs
Converts NPC issues from archives/ to pages/issues/. Only for the original NPC ezine.

```bash
node scripts/convert.mjs
```

#### scripts/fetch-ezine.mjs
Downloads, extracts, and converts a single ezine from autistici.org:

```bash
node scripts/fetch-ezine.mjs <ezine-name>
# Example: node scripts/fetch-ezine.mjs phrack
```

**Key features**:
- Downloads archives (.zip, .tar, .gz) from autistici.org/ezine/
- Extracts and finds all text files (.txt, .nfo, .diz, numeric extensions like .1, .2)
- **Binary detection**: Filters out Word docs, executables, PDFs using isBinaryContent()
- Converts CP437 to UTF-8
- Creates hierarchical folder structure (one folder per archive/issue)
- Generates _meta.json files with numerically-sorted navigation
- Handles numeric file extensions (e.g., "file.002" becomes "file-002.mdx")

#### scripts/fetch-all-ezines.mjs
Batch processes all ezines from autistici.org:

```bash
node scripts/fetch-all-ezines.mjs [--dry-run] [--skip-existing] [--resume] [--small-first]
```

**Options**:
- `--dry-run`: Show what would be processed without downloading
- `--skip-existing`: Skip ezines that already have pages/ folders
- `--resume`: Skip ezines marked as successful in fetch-log.json
- `--small-first`: Process smaller ezines first (counts files, then sorts)

**Features**:
- Maintains fetch-log.json with progress tracking (status, timestamps, errors)
- Dynamic timeouts based on ezine size (5min base + 30sec per file)
- Updates navigation after each successful ezine
- Can resume after crashes or errors

### CP437 Encoding Conversion

Both convert.mjs and fetch-ezine.mjs include CP437 to UTF-8 character mapping (cp437ToUnicode object) for preserving:
- Box drawing characters (─│┌┐└┘═║╔╗╚╝)
- Block elements (█▄▌▐▀░▒▓)
- Extended ASCII symbols (☺☻♥♦♣♠•◘○)
- Accented characters (é, ü, ñ, etc.)

This is critical for maintaining ASCII art integrity.

### MDX Content Format

All ezine pages use this structure:

```mdx
---
title: "Issue Title"
date: "Month Year"
ezine: "Ezine Name"
---

# Issue Title

**Published:** Date info

<div className="ascii-content">

\`\`\`
[Original CP437-converted text content]
\`\`\`

</div>
```

The triple-backtick code block preserves ASCII art formatting and spacing.

## Important Context

### File Extension Handling
The fetch script preserves numeric file extensions (like `.001`, `.2`, `.3`) by converting them to slugs (e.g., `file.002` → `file-002.mdx`). This is important for multi-part files in some ezines.

### Navigation Structure

**Categorized sidebar** to handle 400+ ezines:
- Curated categories in sidebar (Notorious, French, Spanish, Regional)
- Metadata-driven: `ezines-metadata.json` defines categories and descriptions
- Generate script: `scripts/generate-nav.mjs` creates category pages and navigation
- "Browse All Ezines" page lists complete archive alphabetically
- Run `npm run nav` to regenerate navigation after metadata changes

**Hierarchical folders** for each ezine:
- `ezines/40hex/40hex-01/` - Each archive becomes a subfolder
- `ezines/40hex/_meta.json` - Top-level lists all issue folders
- `ezines/40hex/40hex-01/_meta.json` - Lists files within that issue

Files are numerically sorted using extractSortKey() and compareSlugNumerically() functions.

### Production Build

**Build approach**: Local builds (not CI) due to memory requirements for ~400 ezines.

The site is configured for GitHub Pages deployment:
- `basePath` and `assetPrefix` set to `/philez/` in production
- Static export mode generates `/out` directory
- Images set to `unoptimized: true` for static hosting
- **Memory**: Requires significant RAM for large builds (64GB recommended for full archive)

## Working with This Codebase

**Adding ezines**:
1. Single ezine: `node scripts/fetch-ezine.mjs <name>`
2. All ezines: `node scripts/fetch-all-ezines.mjs --small-first --resume`
3. Script handles download, conversion, and navigation setup automatically

**Recommended workflow for importing all ezines**:
1. Start with `--small-first --resume` to handle easy ones first
2. Check fetch-log.json to see which failed
3. Re-run with `--resume` to retry only failed ezines
4. Monitor memory usage during build

**Modifying theme/appearance**:
- Edit `theme.config.tsx` for global settings
- Nextra theme config: https://nextra.site/docs/docs-theme/theme-configuration

**Debugging conversion issues**:
- Check archives/ folder for raw downloaded files
- CP437 mapping in convert.mjs and fetch-ezine.mjs may need adjustments
- Review findTextFiles() logic if text files are missed

**Testing locally**:
- Always use `npm run dev` to preview before building
- Check navigation works in sidebar
- Verify ASCII art renders correctly in code blocks

## SEO and Indexing

The site includes standard SEO files in the `public/` directory:

- **robots.txt**: Allows all crawlers (including AI/LLM bots), references sitemap
- **llms.txt**: Structured information for LLM indexing (purpose, scope, context)
- **sitemap.xml**: Auto-generated during build via `scripts/generate-sitemap.mjs`

The sitemap is automatically generated when running `npm run build`. It:
- Crawls the `out/` directory for all HTML files
- Assigns priorities (homepage=1.0, ezine indexes=0.8, articles=0.5-0.7)
- Includes lastmod dates from file timestamps
- Will warn if over 10,000 URLs (may need splitting)

## Search

**Pagefind** is used instead of Nextra's built-in FlexSearch due to scalability:
- Nextra's FlexSearch is client-side and bundles the entire index (won't scale to 10k+ pages)
- Pagefind generates a static search index at build time
- Lazy loads only what's needed for each search query
- Can handle 10,000+ pages efficiently
- Search index generated via `npx pagefind --site out` during build
- Custom component at `components/PagefindSearch.tsx` integrates with Nextra theme
