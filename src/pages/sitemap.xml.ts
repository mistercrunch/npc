import type { APIRoute } from 'astro'
import { loadEzines } from '../lib/ezines'
import { getEzineIssues, getIssueFiles } from '../lib/files'

const SITE_URL = 'https://philez.org'

export const GET: APIRoute = async () => {
  const data = loadEzines()

  const urls: string[] = []

  // Homepage
  urls.push(`${SITE_URL}/`)

  // Browse page
  urls.push(`${SITE_URL}/browse`)

  // Category pages
  for (const [categorySlug, category] of Object.entries(data.categories)) {
    urls.push(`${SITE_URL}/category/${categorySlug}`)
  }

  // Ezine pages
  for (const [ezineSlug, ezineData] of Object.entries(data.ezines)) {
    // Skip if slug is invalid
    if (!ezineSlug || ezineSlug === 'undefined') continue

    // Ezine index
    urls.push(`${SITE_URL}/ezine/${ezineSlug}`)

    // Get all issues and files
    try {
      const issues = getEzineIssues(ezineSlug)

      for (const issue of issues) {
        // Issue pages
        urls.push(`${SITE_URL}/ezine/${ezineSlug}/${issue.slug}`)

        // Individual file pages
        const files = getIssueFiles(ezineSlug, issue.slug)
        for (const file of files) {
          urls.push(`${SITE_URL}/view/${ezineSlug}/${issue.slug}/${file.slug}`)
        }
      }
    } catch (err) {
      console.error(`Error generating sitemap for ${ezineSlug}:`, err)
    }
  }

  // Generate XML
  const xml = `<?xml version="1.0" encoding="UTF-8"?>
<urlset xmlns="http://www.sitemaps.org/schemas/sitemap/0.9">
${urls.map(url => `  <url>
    <loc>${url}</loc>
    <changefreq>monthly</changefreq>
    <priority>${getPriority(url)}</priority>
  </url>`).join('\n')}
</urlset>`

  return new Response(xml, {
    headers: {
      'Content-Type': 'application/xml; charset=utf-8',
    },
  })
}

function getPriority(url: string): string {
  if (url.endsWith('/')) return '1.0' // Homepage
  if (url.includes('/browse')) return '0.9'
  if (url.includes('/category/')) return '0.8'
  if (url.match(/\/ezine\/[^/]+$/)) return '0.7' // Ezine index
  if (url.match(/\/ezine\/[^/]+\/[^/]+$/)) return '0.6' // Issue page
  if (url.includes('/view/')) return '0.5' // Individual files
  return '0.5'
}
