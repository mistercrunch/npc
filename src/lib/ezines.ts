/**
 * Load and parse ezines.yml metadata
 */

import { readFileSync } from 'fs'
import { join } from 'path'
import yaml from 'js-yaml'

export interface EzineMetadata {
  name: string
  tagline?: string
  description?: string
  years?: string
  language: string
  topics?: string[]
  issues: number
  files: number
}

export interface Category {
  name: string
  description: string
  ezines: string[]
}

export interface EzinesData {
  categories: Record<string, Category>
  ezines: Record<string, EzineMetadata>
}

let cachedData: EzinesData | null = null

export function loadEzines(): EzinesData {
  if (cachedData) return cachedData

  const yamlPath = join(process.cwd(), 'ezines.yml')
  const content = readFileSync(yamlPath, 'utf-8')
  cachedData = yaml.load(content) as EzinesData

  return cachedData
}

export function getEzine(slug: string): EzineMetadata | undefined {
  const data = loadEzines()
  return data.ezines[slug]
}

export function getCategory(slug: string): Category | undefined {
  const data = loadEzines()
  return data.categories[slug]
}

export function getAllEzines(): Array<{ slug: string; metadata: EzineMetadata }> {
  const data = loadEzines()
  return Object.entries(data.ezines)
    .map(([slug, metadata]) => ({ slug, metadata }))
    .sort((a, b) => a.metadata.name.localeCompare(b.metadata.name))
}

export function getEzineCategories(ezineSlug: string): string[] {
  const data = loadEzines()
  const categories: string[] = []

  for (const [catSlug, category] of Object.entries(data.categories)) {
    if (category.ezines.includes(ezineSlug)) {
      categories.push(catSlug)
    }
  }

  return categories
}
