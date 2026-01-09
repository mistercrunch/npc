# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

PhileZ is a Nextra-based static site that archives underground ezines from the BBS era (1980s-2000s). It fetches archived textfiles from autistici.org/ezine/, converts them from CP437 DOS encoding to UTF-8, and generates MDX pages for a searchable, modern web experience.

**Live site:** https://mistercrunch.github.io/philez/

## Commands

```bash
npm run dev          # Start dev server
npm run build        # Build static site (outputs to ./out)

# Fetch a single ezine
node scripts/fetch-ezine.mjs phrack
node scripts/fetch-ezine.mjs noway --force-download

# Batch fetch all ezines
node scripts/fetch-all-ezines.mjs --small-first --resume
node scripts/fetch-all-ezines.mjs --skip-existing --dry-run
```

## Architecture

### Content Pipeline

1. **fetch-ezine.mjs** — Core script that:
   - Downloads archives (.zip/.tar/.gz) from `autistici.org/ezine/<name>/`
   - Extracts nested archives recursively
   - Converts CP437 to UTF-8 via byte-level mapping (preserves ASCII art box-drawing characters)
   - Detects and skips binary files (Word docs, executables, PDFs)
   - Generates MDX files with YAML frontmatter
   - Creates `_meta.json` files for Nextra sidebar navigation

2. **fetch-all-ezines.mjs** — Batch orchestrator that:
   - Processes all 373+ ezines from autistici.org
   - Supports `--small-first` to prioritize smaller ezines
   - Maintains `fetch-log.json` for resume capability
   - Updates navigation `_meta.json` after each successful ezine

### File Structure

- `pages/ezines/<ezine-name>/` — Each ezine gets its own directory
- `pages/ezines/<ezine-name>/index.mdx` — Ezine landing page
- `pages/ezines/<ezine-name>/_meta.json` — Nextra navigation config
- `pages/ezines/<ezine-name>/<issue>/` — Issue subdirectories with articles
- `archives/` — Downloaded archives (gitignored, kept locally for re-processing)

### Key Technical Details

- **Encoding**: CP437 → UTF-8 mapping is critical for ASCII art preservation
- **Binary detection**: Uses magic byte signatures + null byte concentration
- **Sorting**: `compareSlugNumerically()` ensures proper issue ordering (1, 2, 10 not 1, 10, 2)
- **Idempotent**: Scripts check for existing extracted folders before re-downloading
- **Static export**: Configured for GitHub Pages with basePath `/philez`

### Nextra/Next.js Config

- `theme.config.tsx` — Site branding, sidebar config, SEO meta
- `next.config.mjs` — Static export, GitHub Pages basePath
- `next-sitemap.config.js` — Sitemap generation

## Deployment

Pushes to `main` trigger GitHub Actions (`.github/workflows/deploy.yml`) which builds and deploys to GitHub Pages.
