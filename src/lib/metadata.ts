/**
 * Library for loading and working with ezine metadata
 */

import { readFileSync, existsSync } from 'fs'
import { load } from 'js-yaml'
import { join } from 'path'

export interface EzineMetadata {
  // Basic information
  slug: string
  name: string
  description: string

  // Historical context
  years_active?: string
  publication_frequency?: string
  authors?: string[]
  groups?: string[]
  origin?: {
    country?: string
    city?: string
    region?: string
  }

  // Content classification
  languages?: string[]
  topics?: string[]
  technical_level?: 'beginner' | 'intermediate' | 'advanced' | 'mixed'

  // Significance & influence
  significance?: string
  influenced_by?: string[]
  related_ezines?: string[]

  // Content summary
  typical_content?: string[]
  highlights?: Array<{
    issue: string
    title: string
    description: string
  }>

  // Quality & preservation
  preservation_notes?: string
  content_warnings?: string[]

  // Metadata about metadata
  metadata_created?: string
  metadata_updated?: string
  metadata_version?: string
  metadata_source?: 'llm_generated' | 'manual' | 'hybrid' | 'community_contributed'
  metadata_reviewed?: boolean

  // Translations
  translations?: {
    [langCode: string]: {
      name?: string
      description?: string
    }
  }
}

/**
 * Load metadata for a specific ezine
 * Returns null if meta.yaml doesn't exist
 */
export function loadEzineMetadata(ezineSlug: string): EzineMetadata | null {
  const metaPath = join(process.cwd(), 'archives', ezineSlug, 'meta.yaml')

  if (!existsSync(metaPath)) {
    return null
  }

  try {
    const content = readFileSync(metaPath, 'utf-8')
    const metadata = load(content) as EzineMetadata

    // Ensure slug matches
    if (!metadata.slug) {
      metadata.slug = ezineSlug
    }

    return metadata
  } catch (err) {
    console.error(`Error loading metadata for ${ezineSlug}:`, err)
    return null
  }
}

/**
 * Get all ezines that have metadata
 */
export function getAllEzinesWithMetadata(): string[] {
  const archivesDir = join(process.cwd(), 'archives')
  const { readdirSync, statSync } = require('fs')

  try {
    const entries = readdirSync(archivesDir)
    return entries.filter((entry: string) => {
      const entryPath = join(archivesDir, entry)
      const metaPath = join(entryPath, 'meta.yaml')
      return statSync(entryPath).isDirectory() && existsSync(metaPath)
    })
  } catch (err) {
    console.error('Error scanning for metadata:', err)
    return []
  }
}

/**
 * Format a list for display (joins with commas and "and")
 */
export function formatList(items: string[] | undefined): string {
  if (!items || items.length === 0) return ''
  if (items.length === 1) return items[0]
  if (items.length === 2) return `${items[0]} and ${items[1]}`
  return `${items.slice(0, -1).join(', ')}, and ${items[items.length - 1]}`
}

/**
 * Get a human-readable topic name
 */
export function formatTopicName(topic: string): string {
  const topicNames: { [key: string]: string } = {
    'hacking': 'Hacking',
    'phreaking': 'Phreaking',
    'cracking': 'Cracking',
    'viruses': 'Viruses',
    'anarchy': 'Anarchy',
    'carding': 'Carding',
    'social_engineering': 'Social Engineering',
    'security': 'Security',
    'programming': 'Programming',
    'networking': 'Networking',
    'hardware': 'Hardware',
    'culture': 'Culture',
    'politics': 'Politics',
    'tutorials': 'Tutorials',
    'news': 'News',
    'humor': 'Humor'
  }
  return topicNames[topic] || topic
}

/**
 * Get ISO language name from code
 */
export function getLanguageName(code: string): string {
  const languages: { [key: string]: string } = {
    'en': 'English',
    'fr': 'French',
    'es': 'Spanish',
    'de': 'German',
    'nl': 'Dutch',
    'it': 'Italian',
    'pt': 'Portuguese',
    'pl': 'Polish',
    'ru': 'Russian',
    'sv': 'Swedish',
    'no': 'Norwegian',
    'da': 'Danish',
    'fi': 'Finnish'
  }
  return languages[code] || code.toUpperCase()
}
