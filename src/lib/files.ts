/**
 * Scan archives/ folder for ezine issues and files
 */

import { readdirSync, statSync, readFileSync } from 'fs'
import { join } from 'path'
import { convertBuffer } from './cp437'
import { shouldSkipFile, isMediaFile } from './fileTypes'

const ARCHIVES_DIR = join(process.cwd(), 'archives')

export interface IssueInfo {
  slug: string
  name: string
  fileCount: number
}

export interface FileInfo {
  slug: string
  name: string
  size: number
  isMedia: boolean
}

/**
 * Get all issues/archives for an ezine
 */
export function getEzineIssues(ezineSlug: string): IssueInfo[] {
  const ezinePath = join(ARCHIVES_DIR, ezineSlug)
  const issues: IssueInfo[] = []

  try {
    const entries = readdirSync(ezinePath)

    for (const entry of entries) {
      const fullPath = join(ezinePath, entry)
      const stat = statSync(fullPath)

      // Only include actual directories
      if (!stat.isDirectory()) continue
      if (entry.startsWith('.')) continue

      // Count files in this issue
      const files = getIssueFiles(ezineSlug, entry)

      issues.push({
        slug: entry,
        name: entry.replace(/-/g, ' ').replace(/_/g, ' '),
        fileCount: files.length
      })
    }

  } catch (err) {
    console.error(`Error reading issues for ${ezineSlug}:`, err)
  }

  // Sort numerically if possible
  issues.sort((a, b) => {
    const aNum = extractNumber(a.slug)
    const bNum = extractNumber(b.slug)
    if (aNum !== null && bNum !== null) {
      return aNum - bNum
    }
    return a.slug.localeCompare(b.slug)
  })

  return issues
}

/**
 * Get all text files in an issue
 */
export function getIssueFiles(ezineSlug: string, issueSlug: string): FileInfo[] {
  let issuePath = join(ARCHIVES_DIR, ezineSlug, issueSlug)
  const files: FileInfo[] = []

  try {
    // Handle double-nested directories (e.g., 29a-4/29a-4/)
    // If there's only one entry and it's a directory with a similar name, unwrap it
    let entries = readdirSync(issuePath)
    if (entries.length === 1) {
      const singleEntry = entries[0]
      const singlePath = join(issuePath, singleEntry)
      const singleStat = statSync(singlePath)
      if (singleStat.isDirectory()) {
        // Unwrap if the directory name matches or is similar
        issuePath = singlePath
        entries = readdirSync(issuePath)
      }
    }

    for (const entry of entries) {
      const fullPath = join(issuePath, entry)
      const stat = statSync(fullPath)

      if (!stat.isFile()) continue
      if (entry.startsWith('.')) continue

      // Skip files using central config
      if (shouldSkipFile(entry)) continue

      // Create URL-safe slug by removing extension and sanitizing special chars
      const baseSlug = entry.replace(/\.[^.]+$/, '') // Remove extension
      // Replace problematic characters with safe equivalents
      const slug = baseSlug
        .replace(/[!#]/g, '_')
        .replace(/['"`]/g, '')
        .replace(/\s+/g, '-')
        .replace(/[()]/g, '')

      files.push({
        slug,
        name: entry,
        size: stat.size,
        isMedia: isMediaFile(entry)
      })
    }

  } catch (err) {
    console.error(`Error reading files for ${ezineSlug}/${issueSlug}:`, err)
  }

  // Sort numerically
  files.sort((a, b) => {
    const aNum = extractNumber(a.slug)
    const bNum = extractNumber(b.slug)
    if (aNum !== null && bNum !== null) {
      return aNum - bNum
    }
    return a.slug.localeCompare(b.slug)
  })

  return files
}

/**
 * Read a text file with CP437 conversion
 */
export function readTextFile(ezineSlug: string, issueSlug: string, fileSlug: string): string {
  let issuePath = join(ARCHIVES_DIR, ezineSlug, issueSlug)

  // Apply same unwrapping logic as getIssueFiles
  try {
    let entries = readdirSync(issuePath)
    if (entries.length === 1) {
      const singleEntry = entries[0]
      const singlePath = join(issuePath, singleEntry)
      const singleStat = statSync(singlePath)
      if (singleStat.isDirectory()) {
        issuePath = singlePath
      }
    }
  } catch (err) {
    // Ignore errors, will fail below with better message
  }

  // Get all files in the issue directory
  const files = getIssueFiles(ezineSlug, issueSlug)

  // Find the file that matches this slug
  const fileInfo = files.find(f => f.slug === fileSlug)
  if (!fileInfo) {
    throw new Error(`File not found: ${ezineSlug}/${issueSlug}/${fileSlug}`)
  }

  // Read the actual file using its real name
  const filePath = join(issuePath, fileInfo.name)
  const buffer = readFileSync(filePath)
  return convertBuffer(buffer)
}

/**
 * Extract numeric part from a slug for sorting
 */
function extractNumber(slug: string): number | null {
  const match = slug.match(/(\d+)/)
  return match ? parseInt(match[1], 10) : null
}

/**
 * Format file size in human-readable format
 */
export function formatFileSize(bytes: number): string {
  if (bytes < 1024) return `${bytes} B`
  if (bytes < 1024 * 1024) return `${(bytes / 1024).toFixed(1)} KB`
  return `${(bytes / (1024 * 1024)).toFixed(1)} MB`
}
