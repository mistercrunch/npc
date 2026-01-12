# PhileZ Astro Architecture

## Philosophy

Instead of generating 10,000+ static pages, we build a **dynamic site** that reads text files on-demand from the `archives/` folder. The entire site is driven by a single metadata file (`ezines.yml`) that can be hand-curated for quality.

## Core Concept

- **Source of truth**: `archives/` folder (text files from autistici.org)
- **Metadata**: `astro/ezines.yml` - Hand-curated YAML with labels, descriptions, categories
- **Pages**: ~10 total pages (home, categories, ezine viewer, file viewer)
- **Rendering**: Backend SSR - no client-side loading, fully indexable by search engines

## URL Structure

```
/                           → Homepage with categories
/category/notorious         → Category page (Phrack, 40hex, LOD, etc.)
/category/french            → French Underground
/category/spanish           → Spanish/Latin American
/category/regional          → Regional Scenes
/browse                     → Alphabetical list of all ezines
/ezine/[slug]               → Ezine index (list of issues/archives)
/ezine/[slug]/[issue]       → Issue index (list of files)
/view/[slug]/[issue]/[file] → File viewer (renders text with CP437)
```

## File Organization

```
astro/
├── DESIGN.md              # This file
├── ezines.yml             # Master metadata file
├── src/
│   ├── pages/
│   │   ├── index.astro              # Homepage
│   │   ├── browse.astro             # All ezines alphabetically
│   │   ├── category/[slug].astro    # Dynamic category pages
│   │   ├── ezine/[slug]/index.astro # Ezine index
│   │   ├── ezine/[slug]/[issue].astro # Issue file list
│   │   └── view/[...path].astro     # File viewer (catch-all)
│   ├── components/
│   │   ├── Layout.astro       # Base layout with sidebar
│   │   ├── Sidebar.astro      # Navigation sidebar
│   │   ├── FileViewer.astro   # Text file renderer
│   │   └── Breadcrumbs.astro  # Navigation breadcrumbs
│   ├── lib/
│   │   ├── ezines.ts          # Load and parse ezines.yml
│   │   ├── files.ts           # Scan archives/ folder
│   │   └── cp437.ts           # CP437 to UTF-8 conversion
│   └── styles/
│       └── global.css         # Dark theme, ASCII styling
├── public/
│   ├── robots.txt
│   └── llms.txt
└── astro.config.mjs           # SSR config for Node adapter
```

## ezines.yml Structure

```yaml
categories:
  notorious:
    name: "Notorious Classics"
    description: "The legendary ezines that defined the scene"
    ezines:
      - phrack
      - 40hex
      - lod
      - 2600

  french:
    name: "French Underground"
    description: "French-language hacker and phreaker zines"
    ezines:
      - FrHack
      - Northern_Phun_Co  # Bilingual (fr/en)
      - hackito

  spanish:
    name: "Spanish/Latin American"
    description: "Spanish-language underground publications"
    ezines:
      - cyberpatrol
      - nuke

  regional:
    name: "Regional Scenes"
    description: "BBS scenes from around the world"
    ezines:
      - hack-tic  # Netherlands
      - Northern_Phun_Co  # Quebec

ezines:
  phrack:
    name: "Phrack"
    tagline: "Hacking, Phreaking, and Anarchy"
    description: "One of the longest-running hacker magazines, published since 1985"
    years: "1985-present"
    language: en
    topics: [hacking, phreaking, security]
    featured: true

  40hex:
    name: "40Hex"
    tagline: "Virus Magazine"
    description: "Computer virus research and code from the early 90s"
    years: "1990-1994"
    language: en
    topics: [virus, malware, assembly]

  Northern_Phun_Co:
    name: "NPC (418)"
    tagline: "Northern Phun Co."
    description: "Bilingual hacker/phreaker zine from Quebec's 418 area code"
    years: "1992-1994"
    language: "fr/en"
    topics: [hacking, phreaking, bbs]

  # ... all other ezines with minimal metadata
  # Many can just have: name + language

  cud:
    name: "Computer underground Digest"
    language: en

  # Uncategorized ezines still listed, just not in sidebar
  obscure-ezine:
    name: "Obscure Ezine"
    language: en
```

## How It Works

### 1. Homepage (`/`)

- Reads `ezines.yml`
- Displays curated categories as cards
- Each category shows 4-6 featured ezines with descriptions
- Link to "Browse All" for complete alphabetical list

### 2. Category Pages (`/category/notorious`)

- Reads category from `ezines.yml`
- Lists all ezines in that category
- Shows name, tagline, years, topics

### 3. Browse All (`/browse`)

- Alphabetical list of ALL ezines from `ezines.yml`
- Even uncategorized ones appear here

### 4. Ezine Index (`/ezine/phrack`)

- Reads `archives/phrack/` folder structure
- Lists all archives/issues found on disk
- Backend scans filesystem at request time
- Shows issue count, dates (if parseable from filenames)

### 5. Issue View (`/ezine/phrack/phrack63-tar`)

- Scans `archives/phrack/phrack63-tar/` folder
- Lists all .txt files found
- Shows file sizes, names

### 6. File Viewer (`/view/phrack/phrack63-tar/article-01`)

- Reads `archives/phrack/phrack63-tar/article-01.txt`
- Converts CP437 → UTF-8
- Renders in `<pre>` with monospace font
- Dark terminal aesthetic
- Breadcrumbs for navigation
- **Fully SSR** - content baked into HTML for SEO

## Benefits

✅ **No build memory issues** - No pre-generation of 10k pages
✅ **SEO-friendly** - All content server-rendered in HTML
✅ **Easy curation** - Edit `ezines.yml` to feature/categorize content
✅ **Fast iterations** - No rebuild needed when adding ezines
✅ **Scalable** - Can handle 1,000+ ezines without memory issues
✅ **Simple** - ~10 page templates instead of 10,000 generated files

## Performance Considerations

### Caching Strategy

- **Filesystem scans**: Cache directory listings (expires after 1 hour)
- **File reads**: Cache converted CP437 content (LRU cache, max 1000 files)
- **YAML loading**: Load once at startup, watch for changes in dev

### CDN

For production on GitHub Pages:
- Use Astro's static adapter with `output: 'server'` for local dev
- Switch to `output: 'static'` for deployment with getStaticPaths
- Or deploy to Vercel/Netlify with SSR support

### Build Options

**Option A: Pure SSR** (Vercel/Netlify/custom server)
- Every request rendered on-demand
- Best for active curation/changes
- Requires hosting with Node.js

**Option B: Hybrid Static** (GitHub Pages compatible)
- Pre-render known paths at build time
- Use `getStaticPaths()` to generate pages for ezines in `ezines.yml`
- Still dynamic file scanning, but pages are pre-built
- Best of both worlds if we can keep it under memory limits

**Option C: Smart Static** (Recommended for GitHub Pages)
- Only generate pages for **featured/categorized** ezines (~50 ezines)
- Uncategorized ezines show as "Browse Archives" with external links
- Keeps build under control, focuses on curated content

## Migration Strategy

1. **Generate initial ezines.yml** from current `archives/` structure
2. **Build minimal Astro site** (homepage + file viewer)
3. **Hand-curate** top 20-30 ezines in YAML
4. **Test SSR locally**
5. **Deploy** to Vercel/Netlify OR use hybrid static for GitHub Pages

## Next Steps

1. Run script to generate `ezines.yml` from `archives/` folder
2. Initialize Astro project: `npm create astro@latest`
3. Build layout and sidebar components
4. Implement file viewer with CP437 conversion
5. Test with dev server
6. Choose deployment strategy (SSR vs static)
