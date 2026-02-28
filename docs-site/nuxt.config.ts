export default defineNuxtConfig({
  modules: ['@nuxtjs/i18n', '@nuxtjs/mcp-toolkit', 'nuxt-llms'],
  i18n: {
    defaultLocale: 'en',
    locales: [{
      code: 'en',
      name: 'English',
    }, {
      code: 'fr',
      name: 'Français',
    }],
  },
  mcp: {
    name: 'Dog Test Docs MCP',
    version: '1.0.0',
  },
  llms: {
    domain: process.env.NUXT_PUBLIC_SITE_URL || 'http://localhost:3000',
    title: 'Dog Test Documentation',
    description: 'Hardware-in-the-loop testing docs with MCP and LLM integration references.',
    sections: [
      {
        title: 'Getting Started',
        description: 'Quick start guides in English and French.',
        links: [
          {
            title: 'English Introduction',
            description: 'Start with the English introduction.',
            href: '/en/getting-started/introduction',
          },
          {
            title: 'Introduction en Français',
            description: 'Commencez avec l’introduction en français.',
            href: '/fr/getting-started/introduction',
          },
        ],
      },
      {
        title: 'Integrations',
        description: 'MCP and LLM integration guides.',
        links: [
          {
            title: 'MCP Integration',
            description: 'Expose docs helpers via MCP tools.',
            href: '/en/integrations/mcp-integration',
          },
          {
            title: 'LLM Integration',
            description: 'Use llms.txt and llms-full.txt routes for agents.',
            href: '/en/integrations/llm-integration',
          },
        ],
      },
    ],
    full: {
      title: 'Dog Test Full Documentation',
      description: 'Extended LLM-oriented documentation with integration details.',
    },
  },
  image: {
    provider: 'none',
  },
})
