# Ezine Metadata System

This directory contains templates and instructions for creating rich, structured metadata for PhileZ ezines.

## Overview

The metadata system adds historical context, descriptions, and categorization to make the archive more discoverable and preserve the historical significance of these underground publications.

## Structure

```
metadata/
  ├── README.md          # This file
  ├── TEMPLATE.yaml      # Template for metadata files
  └── INSTRUCTIONS.md    # Detailed instructions for LLM agents

src/pages/ezine/{ezine_slug}/
  └── meta.yaml          # Metadata for each ezine (created from TEMPLATE.yaml)
```

## Metadata Location

Each ezine's metadata lives in its page directory:

```
src/pages/ezine/
  ├── phrack/
  │   ├── meta.yaml         # Metadata for Phrack
  │   ├── index.astro       # Landing page (can load meta.yaml)
  │   └── [issueSlug]/      # Issue pages
  ├── 40hex/
  │   ├── meta.yaml         # Metadata for 40Hex
  │   └── ...
  └── Northern_Phun_Co/
      ├── meta.yaml         # Metadata for NPC (example)
      └── ...
```

## Using Metadata in Pages

Import and use the metadata library in your Astro pages:

```astro
---
// src/pages/ezine/[slug]/index.astro
import { loadEzineMetadata } from '../../../lib/metadata'

const ezineSlug = Astro.params.slug as string
const metadata = loadEzineMetadata(ezineSlug)
---

{metadata && (
  <div>
    <h1>{metadata.name}</h1>
    <p>{metadata.description}</p>
    {metadata.years_active && <p>Active: {metadata.years_active}</p>}
    {metadata.significance && <div>{metadata.significance}</div>}
  </div>
)}
```

## Creating Metadata

### For Humans

1. Navigate to the ezine's page folder: `cd src/pages/ezine/{ezine_slug}/`
2. Copy the template: `cp ../../../metadata/TEMPLATE.yaml meta.yaml`
3. Read 3-5 representative files from `../archives/{ezine_slug}/`
4. Fill in the metadata following `INSTRUCTIONS.md`
5. Validate YAML syntax

### For LLM Agents

Follow the detailed workflow in `INSTRUCTIONS.md`. The process takes 15-45 minutes per ezine depending on complexity.

**Key principles:**
- Read actual content, don't guess from filenames
- Be factual and neutral
- Focus on historical significance
- Use "unknown" rather than guessing
- Include content warnings where appropriate

## Metadata Fields

See `TEMPLATE.yaml` for the complete structure. Key sections:

- **Basic Info**: slug, name, description
- **Historical Context**: years active, authors, groups, origin
- **Content**: languages, topics, technical level
- **Significance**: historical importance, influence, related ezines
- **Quality**: preservation notes, content warnings

## Validation

Before committing metadata, check:

- [ ] Slug matches folder name exactly
- [ ] YAML syntax is valid (no tabs, proper indentation)
- [ ] Years active match file dates
- [ ] Description is concise (1-2 sentences)
- [ ] Topics list is comprehensive
- [ ] Content warnings are appropriate

## Progress Tracking

Track which ezines have metadata:

```bash
# List all ezines
ls ../archives/

# List ezines with metadata
find src/pages/ezine -name "meta.yaml" | wc -l

# Find ezines missing metadata
comm -23 <(ls ../archives/ | sort) <(find src/pages/ezine -name meta.yaml -exec dirname {} \; | xargs -I {} basename {} | sort)
```

## Examples

- `src/pages/ezine/Northern_Phun_Co/meta.yaml` - Complete example for a bilingual Quebec ezine

## Future Enhancements

- [ ] Per-file summaries for specific ezines
- [ ] Translations of descriptions to multiple languages
- [ ] Automated topic extraction from content
- [ ] Cross-referencing between related ezines
- [ ] Timeline visualization of ezine publications

## Questions?

See `INSTRUCTIONS.md` for detailed guidance on creating metadata.
