/**
 * Central configuration for file type handling
 */

/**
 * File extensions to completely skip (not show in listings at all)
 * These are typically binary files, executables, or archives
 */
export const SKIP_EXTENSIONS = [
  // Archives
  'zip', 'tar', 'gz', 'bz2', 'xz', 'rar', '7z', 'arj', 'lzh', 'arc',

  // Executables
  'exe', 'com', 'dll', 'bin', 'bat', 'cmd', 'vbs', 'scr',

  // Images (too many, would clutter listings)
  'jpg', 'jpeg', 'png', 'gif', 'bmp', 'ico', 'tif', 'tiff', 'pcx',

  // Documents (binary formats)
  'pdf', 'doc', 'docx', 'xls', 'xlsx', 'ppt', 'pptx',

  // Other binary
  'obj', 'o', 'a', 'lib', 'so', 'dylib',
]

/**
 * Media file extensions (show in listings, but handle specially)
 * These should be downloadable or playable
 */
export const MEDIA_EXTENSIONS = [
  // Audio
  'mp3', 'wav', 'mid', 'midi', 'mod', 'xm', 's3m', 'it', 'ogg', 'flac', 'au', 'aif', 'aiff',

  // Video (rare in old ezines, but possible)
  'avi', 'mpg', 'mpeg', 'mp4', 'mov', 'wmv', 'flv',
]

/**
 * Check if a file should be completely skipped
 */
export function shouldSkipFile(filename: string): boolean {
  const ext = getExtension(filename)
  return SKIP_EXTENSIONS.includes(ext)
}

/**
 * Check if a file is a media file
 */
export function isMediaFile(filename: string): boolean {
  const ext = getExtension(filename)
  return MEDIA_EXTENSIONS.includes(ext)
}

/**
 * Check if a file is viewable as text
 */
export function isTextFile(filename: string): boolean {
  return !shouldSkipFile(filename) && !isMediaFile(filename)
}

/**
 * Get file extension in lowercase, without the dot
 */
function getExtension(filename: string): string {
  const match = filename.match(/\.([^.]+)$/i)
  return match ? match[1].toLowerCase() : ''
}

/**
 * Get a display icon for a file based on its type
 */
export function getFileIcon(filename: string): string {
  if (isMediaFile(filename)) {
    const ext = getExtension(filename)
    if (['mp3', 'wav', 'mid', 'midi', 'mod', 'xm', 's3m', 'it', 'ogg', 'flac', 'au', 'aif', 'aiff'].includes(ext)) {
      return '🎵'
    }
    return '🎬'
  }
  return '📄'
}

/**
 * Get file type description for display
 */
export function getFileTypeDescription(filename: string): string | null {
  if (isMediaFile(filename)) {
    const ext = getExtension(filename).toUpperCase()
    return `${ext} media file`
  }
  return null
}
