#!/usr/bin/env node
/**
 * Batch Ezine Fetcher
 *
 * Downloads ALL ezines from autistici.org/ezine/
 *
 * Usage: node scripts/fetch-all-ezines.mjs [--dry-run] [--skip-existing]
 */

import { execSync } from 'child_process'
import { existsSync, mkdirSync, writeFileSync, readFileSync } from 'fs'
import { join, dirname } from 'path'
import { fileURLToPath } from 'url'

const __dirname = dirname(fileURLToPath(import.meta.url))
const ROOT_DIR = join(__dirname, '..')
const PAGES_DIR = join(ROOT_DIR, 'pages', 'ezines')
const LOG_FILE = join(ROOT_DIR, 'fetch-log.json')

const BASE_URL = 'https://www.autistici.org/ezine/'

// Parse args
const args = process.argv.slice(2)
const DRY_RUN = args.includes('--dry-run')
const SKIP_EXISTING = args.includes('--skip-existing')
const RESUME = args.includes('--resume') // Skip already-successful from log
const SMALL_FIRST = args.includes('--small-first') // Process smaller ezines first
const SPECIFIC = args.find(a => !a.startsWith('--'))

async function fetchEzineList() {
  console.log('Fetching ezine directory listing...')
  try {
    const html = execSync(`curl -sL "${BASE_URL}"`, { encoding: 'utf-8', maxBuffer: 10 * 1024 * 1024 })

    // Parse directory links (folders end with /)
    const links = [...html.matchAll(/href="([^"]+)\/"/gi)]
      .map(m => m[1])
      .filter(l => !l.startsWith('?') && !l.startsWith('/') && l !== '..')

    return links
  } catch (err) {
    console.error('Failed to fetch ezine list:', err.message)
    return []
  }
}

async function countEzineFiles(name) {
  try {
    const html = execSync(`curl -sL "${BASE_URL}${name}/"`, { encoding: 'utf-8', maxBuffer: 10 * 1024 * 1024, timeout: 30000 })
    const files = [...html.matchAll(/href="([^"]+)"/gi)].filter(m => !m[1].startsWith('?') && !m[1].startsWith('/'))
    return files.length
  } catch {
    return 999 // Unknown size, process last
  }
}

function loadLog() {
  if (existsSync(LOG_FILE)) {
    return JSON.parse(readFileSync(LOG_FILE, 'utf-8'))
  }
  return { processed: {}, errors: [], stats: { total: 0, success: 0, failed: 0, skipped: 0 } }
}

function saveLog(log) {
  writeFileSync(LOG_FILE, JSON.stringify(log, null, 2))
}

async function processEzine(name, log, fileCount = 0) {
  const ezinePageDir = join(PAGES_DIR, name)

  // Skip if already successful in log (--resume)
  if (RESUME && log.processed[name]?.status === 'success') {
    console.log(`⏭ Skipping ${name} (already successful)`)
    log.stats.skipped++
    return true
  }

  // Skip if already processed and flag set
  if (SKIP_EXISTING && existsSync(ezinePageDir)) {
    console.log(`⏭ Skipping ${name} (already exists)`)
    log.stats.skipped++
    return true
  }

  if (DRY_RUN) {
    console.log(`[DRY RUN] Would process: ${name} (~${fileCount} files)`)
    return true
  }

  try {
    console.log(`\n${'─'.repeat(60)}`)
    console.log(`Processing ${name} (~${fileCount} files)...`)
    console.log('─'.repeat(60))

    // Longer timeout for bigger ezines (base 5min + 30sec per file)
    const timeout = Math.max(300000, 300000 + fileCount * 30000)

    const output = execSync(
      `node "${join(__dirname, 'fetch-ezine.mjs')}" "${name}"`,
      { encoding: 'utf-8', maxBuffer: 50 * 1024 * 1024, timeout }
    )
    console.log(output)

    log.processed[name] = { status: 'success', files: fileCount, timestamp: new Date().toISOString() }
    log.stats.success++
    return true
  } catch (err) {
    console.error(`✗ Failed to process ${name}:`, err.message)
    log.processed[name] = { status: 'error', error: err.message, timestamp: new Date().toISOString() }
    if (!log.errors.includes(name)) log.errors.push(name)
    log.stats.failed++
    return false
  }
}

function updateMeta(ezines, log) {
  // Update the main ezines _meta.json
  const metaPath = join(PAGES_DIR, '_meta.json')
  const meta = { index: 'All Ezines' }

  const successful = ezines.filter(e => log.processed[e]?.status === 'success' || existsSync(join(PAGES_DIR, e)))
  successful.sort().forEach(name => {
    meta[name] = name
  })

  writeFileSync(metaPath, JSON.stringify(meta, null, 2))

  // Update the index page
  const indexPath = join(PAGES_DIR, 'index.mdx')
  const indexContent = `# Ezine Archive

Historical underground ezines from the BBS era (1980s-2000s), preserved and converted for the modern web.

**Source:** [autistici.org/ezine](https://www.autistici.org/ezine/)

## Stats

- **Total ezines:** ${successful.length}
- **Last updated:** ${new Date().toISOString().split('T')[0]}

## All Ezines

${successful.sort().map(name => `- [${name}](/ezines/${name})`).join('\n')}

---

## Adding More

\`\`\`bash
node scripts/fetch-ezine.mjs <ezine-name>
node scripts/fetch-all-ezines.mjs --skip-existing
\`\`\`
`
  writeFileSync(indexPath, indexContent)
}

async function main() {
  console.log('═'.repeat(60))
  console.log('  BATCH EZINE FETCHER')
  console.log('═'.repeat(60))
  console.log(`Mode: ${DRY_RUN ? 'DRY RUN' : 'LIVE'}`)
  console.log(`Skip existing: ${SKIP_EXISTING}`)
  console.log(`Resume from log: ${RESUME}`)
  console.log(`Small first: ${SMALL_FIRST}`)
  console.log()

  // Get list of all ezines
  let ezines = await fetchEzineList()

  if (SPECIFIC) {
    ezines = ezines.filter(e => e.toLowerCase().includes(SPECIFIC.toLowerCase()))
    console.log(`Filtered to ${ezines.length} ezines matching "${SPECIFIC}"`)
  }

  console.log(`Found ${ezines.length} ezines to process`)

  // Load progress log
  const log = loadLog()

  // Get file counts and sort if --small-first
  let ezineData = ezines.map(name => ({ name, files: 0 }))

  if (SMALL_FIRST) {
    console.log('Counting files in each ezine (this takes a moment)...')
    for (let i = 0; i < ezineData.length; i++) {
      // Skip counting if already successful
      if (RESUME && log.processed[ezineData[i].name]?.status === 'success') {
        ezineData[i].files = -1 // Will be skipped anyway
        continue
      }
      if (SKIP_EXISTING && existsSync(join(PAGES_DIR, ezineData[i].name))) {
        ezineData[i].files = -1 // Will be skipped anyway
        continue
      }
      ezineData[i].files = await countEzineFiles(ezineData[i].name)
      process.stdout.write(`\r  Counted ${i + 1}/${ezineData.length}...`)
    }
    console.log('\n')

    // Sort: skipped first (files=-1), then by file count ascending
    ezineData.sort((a, b) => a.files - b.files)
  }

  log.stats.total = ezines.length

  if (DRY_RUN) {
    console.log('Ezines that would be processed:')
    ezineData.forEach((e, i) => console.log(`  ${i + 1}. ${e.name} (~${e.files} files)`))
    return
  }

  // Process each ezine
  let processed = 0
  for (const { name, files } of ezineData) {
    processed++
    console.log(`\n[${processed}/${ezineData.length}] ${name}`)

    await processEzine(name, log, files)

    // Save progress after each ezine
    saveLog(log)

    // Update navigation after each successful ezine
    updateMeta(ezineData.map(e => e.name), log)
  }

  // Update navigation
  console.log('\nUpdating navigation...')
  updateMeta(ezines, log)

  // Final summary
  console.log('\n' + '═'.repeat(60))
  console.log('  COMPLETE')
  console.log('═'.repeat(60))
  console.log(`Total:    ${log.stats.total}`)
  console.log(`Success:  ${log.stats.success}`)
  console.log(`Failed:   ${log.stats.failed}`)
  console.log(`Skipped:  ${log.stats.skipped}`)

  if (log.errors.length > 0) {
    console.log(`\nFailed ezines:`)
    log.errors.forEach(e => console.log(`  - ${e}`))
  }

  console.log(`\nLog saved to: ${LOG_FILE}`)
}

main().catch(err => {
  console.error('Fatal error:', err)
  process.exit(1)
})
