# PhileZ

**The BBS-era underground ezine archive**

A project to preserve and make accessible the underground e-zines of the BBS era — hacking, phreaking, virii, anarchy, and cracking textfiles from the digital underground, converted for the modern web.

**Live site:** https://mistercrunch.github.io/philez/

## Goals

- Make old fanzines easier to read and consume on the modern web
- Get content indexed by search engines for discoverability
- Convert CP437 DOS encoding to proper UTF-8 while preserving ASCII art
- Make content searchable using Nextra's built-in search
- Preserve digital history that might otherwise be lost

## Development

```bash
npm install
npm run dev
```

## Fetching Ezines

The project includes scripts to fetch and convert ezines from the autistici.org archive.

```bash
# Fetch a single ezine
node scripts/fetch-ezine.mjs phrack

# Fetch all ezines (with options)
node scripts/fetch-all-ezines.mjs --small-first --resume
```

Options for `fetch-all-ezines.mjs`:
- `--small-first` — Process smaller ezines first
- `--resume` — Skip ezines that have already been processed
- `--skip-existing` — Skip ezines that already have pages generated

Options for `fetch-ezine.mjs`:
- `--force-download` — Re-download archives even if already extracted

## Source

Original archives from [autistici.org/ezine](https://www.autistici.org/ezine/).
