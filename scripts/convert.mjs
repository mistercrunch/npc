import { readFileSync, writeFileSync, readdirSync, existsSync, mkdirSync } from 'fs'
import { join, dirname } from 'path'
import { fileURLToPath } from 'url'

const __dirname = dirname(fileURLToPath(import.meta.url))
const archivesDir = join(__dirname, '..', 'archives')
const pagesDir = join(__dirname, '..', 'pages', 'issues')

// CP437 to Unicode mapping for box drawing and special chars
const cp437ToUnicode = {
  // Box drawing characters
  0xC4: '─', 0xCD: '═', 0xB3: '│', 0xBA: '║',
  0xDA: '┌', 0xBF: '┐', 0xC0: '└', 0xD9: '┘',
  0xC2: '┬', 0xC1: '┴', 0xC3: '├', 0xB4: '┤', 0xC5: '┼',
  0xC9: '╔', 0xBB: '╗', 0xC8: '╚', 0xBC: '╝',
  0xCB: '╦', 0xCA: '╩', 0xCC: '╠', 0xB9: '╣', 0xCE: '╬',
  0xD5: '╒', 0xB8: '╕', 0xD4: '╘', 0xBE: '╛',
  0xD6: '╓', 0xB7: '╖', 0xD3: '╙', 0xBD: '╜',
  0xC6: '╞', 0xB5: '╡', 0xD1: '╤', 0xCF: '╧',
  0xC7: '╟', 0xB6: '╢', 0xD2: '╥', 0xD0: '╨',
  0xD8: '╪', 0xD7: '╫',
  // Block elements
  0xDB: '█', 0xDC: '▄', 0xDD: '▌', 0xDE: '▐', 0xDF: '▀',
  0xB0: '░', 0xB1: '▒', 0xB2: '▓',
  // Other special chars
  0x01: '☺', 0x02: '☻', 0x03: '♥', 0x04: '♦', 0x05: '♣', 0x06: '♠',
  0x07: '•', 0x08: '◘', 0x09: '○', 0x0A: '◙', 0x0B: '♂', 0x0C: '♀',
  0x0D: '♪', 0x0E: '♫', 0x0F: '☼',
  0x10: '►', 0x11: '◄', 0x12: '↕', 0x13: '‼', 0x14: '¶', 0x15: '§',
  0x16: '▬', 0x17: '↨', 0x18: '↑', 0x19: '↓', 0x1A: '→', 0x1B: '←',
  0x1C: '∟', 0x1D: '↔', 0x1E: '▲', 0x1F: '▼',
  // Extended Latin
  0x80: 'Ç', 0x81: 'ü', 0x82: 'é', 0x83: 'â', 0x84: 'ä', 0x85: 'à',
  0x86: 'å', 0x87: 'ç', 0x88: 'ê', 0x89: 'ë', 0x8A: 'è', 0x8B: 'ï',
  0x8C: 'î', 0x8D: 'ì', 0x8E: 'Ä', 0x8F: 'Å',
  0x90: 'É', 0x91: 'æ', 0x92: 'Æ', 0x93: 'ô', 0x94: 'ö', 0x95: 'ò',
  0x96: 'û', 0x97: 'ù', 0x98: 'ÿ', 0x99: 'Ö', 0x9A: 'Ü', 0x9B: '¢',
  0x9C: '£', 0x9D: '¥', 0x9E: '₧', 0x9F: 'ƒ',
  0xA0: 'á', 0xA1: 'í', 0xA2: 'ó', 0xA3: 'ú', 0xA4: 'ñ', 0xA5: 'Ñ',
  0xA6: 'ª', 0xA7: 'º', 0xA8: '¿', 0xA9: '⌐', 0xAA: '¬', 0xAB: '½',
  0xAC: '¼', 0xAD: '¡', 0xAE: '«', 0xAF: '»',
  // More symbols
  0xE0: 'α', 0xE1: 'ß', 0xE2: 'Γ', 0xE3: 'π', 0xE4: 'Σ', 0xE5: 'σ',
  0xE6: 'µ', 0xE7: 'τ', 0xE8: 'Φ', 0xE9: 'Θ', 0xEA: 'Ω', 0xEB: 'δ',
  0xEC: '∞', 0xED: 'φ', 0xEE: 'ε', 0xEF: '∩',
  0xF0: '≡', 0xF1: '±', 0xF2: '≥', 0xF3: '≤', 0xF4: '⌠', 0xF5: '⌡',
  0xF6: '÷', 0xF7: '≈', 0xF8: '°', 0xF9: '∙', 0xFA: '·', 0xFB: '√',
  0xFC: 'ⁿ', 0xFD: '²', 0xFE: '■', 0xFF: ' ',
}

function convertCP437ToUTF8(buffer) {
  let result = ''
  for (let i = 0; i < buffer.length; i++) {
    const byte = buffer[i]
    if (byte < 0x20 && byte !== 0x09 && byte !== 0x0A && byte !== 0x0D) {
      // Control chars that have special CP437 glyphs
      result += cp437ToUnicode[byte] || ' '
    } else if (byte < 0x80) {
      // Standard ASCII
      result += String.fromCharCode(byte)
    } else {
      // Extended CP437
      result += cp437ToUnicode[byte] || '?'
    }
  }
  return result
}

function extractMetadata(content, issueNum) {
  // Try to extract date and other info from the content
  const datePatterns = [
    /(\d{2})-(\d{2})-(\d{4})/,
    /(\w+)\s+(\d{4})/,
  ]

  const months = {
    '01': 'January', '02': 'February', '03': 'March', '04': 'April',
    '05': 'May', '06': 'June', '07': 'July', '08': 'August',
    '09': 'September', '10': 'October', '11': 'November', '12': 'December'
  }

  // Issue dates based on file timestamps we saw
  const issueDates = {
    '001': 'November 1992', '002': 'December 1992', '003': 'January 1993',
    '004': 'February 1993', '005': 'March 1993', '006': 'April 1993',
    '007': 'May 1993', '008': 'July 1993', '009': 'August 1993',
    '010': 'October 1993', '011': 'November 1993', '012': 'December 1993',
    '013': 'February 1994', '014': 'March 1994', '015': 'April 1994'
  }

  return {
    date: issueDates[issueNum] || 'Unknown',
    issue: parseInt(issueNum, 10)
  }
}

function convertToMarkdown(content, issueNum) {
  const meta = extractMetadata(content, issueNum)

  // Clean up some common issues
  let cleaned = content
    // Remove ANSI escape codes if any leaked through
    .replace(/\x1b\[[0-9;]*m/g, '')
    // Normalize line endings
    .replace(/\r\n/g, '\n')
    .replace(/\r/g, '\n')

  // Wrap the content in a code block to preserve ASCII art formatting
  const markdown = `---
title: "NPC Issue #${meta.issue}"
date: "${meta.date}"
---

# NPC Issue #${meta.issue}

**Published:** ${meta.date}

<div className="ascii-content">

\`\`\`
${cleaned}
\`\`\`

</div>
`

  return markdown
}

// Main conversion
console.log('Converting NPC issues from CP437 to Markdown...\n')

if (!existsSync(pagesDir)) {
  mkdirSync(pagesDir, { recursive: true })
}

const issues = []

for (let i = 1; i <= 15; i++) {
  const issueNum = String(i).padStart(3, '0')
  const issueDir = join(archivesDir, `npc-${issueNum}`)

  // Find the main txt file
  const possibleNames = [`NPC-${issueNum}.TXT`, `npc-${issueNum}`, `NPC-${issueNum}.txt`]
  let txtFile = null

  for (const name of possibleNames) {
    const fullPath = join(issueDir, name)
    if (existsSync(fullPath)) {
      txtFile = fullPath
      break
    }
  }

  if (!txtFile) {
    console.log(`⚠ Issue ${issueNum}: No text file found`)
    continue
  }

  try {
    const buffer = readFileSync(txtFile)
    const utf8Content = convertCP437ToUTF8(buffer)
    const markdown = convertToMarkdown(utf8Content, issueNum)

    const outputPath = join(pagesDir, `npc-${issueNum}.mdx`)
    writeFileSync(outputPath, markdown)

    console.log(`✓ Issue ${issueNum}: Converted successfully`)
    issues.push({ num: issueNum, issue: i })
  } catch (err) {
    console.log(`✗ Issue ${issueNum}: Error - ${err.message}`)
  }
}

// Create index page for issues
const issuesIndex = `# All Issues

Browse all 15 issues of Northern Phun Co. (1992-1994)

${issues.map(({ num, issue }) => `- [Issue #${issue}](/issues/npc-${num})`).join('\n')}
`

writeFileSync(join(pagesDir, 'index.mdx'), issuesIndex)
console.log('\n✓ Created issues index page')
console.log(`\nDone! Converted ${issues.length} issues.`)
