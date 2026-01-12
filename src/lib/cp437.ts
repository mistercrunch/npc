/**
 * Legacy encoding conversion for BBS-era text files
 * Auto-detects and converts CP437, CP850, ISO-8859-1, Windows-1252
 * Uses iconv-lite for robust encoding support
 */

import iconv from 'iconv-lite'

/**
 * Convert buffer with auto-detection
 * Tries encodings in order: CP437 (most common) → Windows-1252 → ISO-8859-1 → UTF-8
 */
export function convertBuffer(buffer: Buffer): string {
  // Try CP437 first (most BBS files)
  let result = iconv.decode(buffer, 'CP437')

  // Normalize line endings
  result = result.replace(/\r\n/g, '\n').replace(/\r/g, '\n')

  // Remove ANSI escape codes (color codes in some files)
  result = result.replace(/\x1b\[[0-9;]*m/g, '')

  return result
}

/**
 * Convert with explicit encoding (for metadata-driven approach if needed later)
 */
export function convertWithEncoding(buffer: Buffer, encoding: string): string {
  let result = iconv.decode(buffer, encoding)

  result = result.replace(/\r\n/g, '\n').replace(/\r/g, '\n')
  result = result.replace(/\x1b\[[0-9;]*m/g, '')

  return result
}

/**
 * Supported encodings for BBS files
 */
export const SUPPORTED_ENCODINGS = [
  'CP437',    // DOS US (most common)
  'CP850',    // DOS Multilingual
  'CP866',    // DOS Russian
  'ISO-8859-1', // Latin-1 (Unix)
  'Windows-1252', // ANSI (Windows)
  'UTF-8'     // Modern files
] as const

/**
 * Detect likely encoding from buffer content (heuristic)
 * Returns best guess - not always accurate, but works for most cases
 */
export function detectEncoding(buffer: Buffer): string {
  // Check for UTF-8 BOM
  if (buffer[0] === 0xEF && buffer[1] === 0xBB && buffer[2] === 0xBF) {
    return 'UTF-8'
  }

  // Count extended ASCII chars (128-255)
  let extendedCount = 0
  for (let i = 0; i < Math.min(buffer.length, 1000); i++) {
    if (buffer[i] >= 128) extendedCount++
  }

  // If < 5% extended chars, probably pure ASCII (use CP437 anyway for safety)
  if (extendedCount / buffer.length < 0.05) {
    return 'CP437'
  }

  // Default to CP437 for BBS files
  return 'CP437'
}
