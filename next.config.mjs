import nextra from 'nextra'

const withNextra = nextra({
  theme: 'nextra-theme-docs',
  themeConfig: './theme.config.tsx',
})

export default withNextra({
  output: 'export',
  images: {
    unoptimized: true
  },
  basePath: process.env.NODE_ENV === 'production' ? '/npc' : '',
  assetPrefix: process.env.NODE_ENV === 'production' ? '/npc/' : '',
})
