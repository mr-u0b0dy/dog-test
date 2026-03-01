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
    }, {
      code: 'ru',
      name: 'Русский',
    }, {
      code: 'ja',
      name: '日本語',
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
        description: 'Installation, project structure, and first-test tutorial.',
        links: [
          {
            title: 'Introduction',
            description: 'What dog-test is and why it exists.',
            href: '/en/getting-started/introduction',
          },
          {
            title: 'Installation',
            description: 'Prerequisites, cloning, and first build.',
            href: '/en/getting-started/installation',
          },
          {
            title: 'Project Structure',
            description: 'Directory layout and component overview.',
            href: '/en/getting-started/project-structure',
          },
          {
            title: 'Writing Your First Test',
            description: 'Step-by-step guide to your first dog test.',
            href: '/en/getting-started/first-test',
          },
        ],
      },
      {
        title: 'Reference',
        description: 'Architecture, examples, CLI workflow, and C API.',
        links: [
          {
            title: 'Architecture',
            description: 'Component overview and execution flow.',
            href: '/en/reference/architecture',
          },
          {
            title: 'Examples',
            description: 'UART, I2C, and SPI example walkthroughs.',
            href: '/en/reference/examples',
          },
          {
            title: 'Dog Test Workflow',
            description: 'Full CLI reference for dt_runner.py.',
            href: '/en/reference/dt-workflow',
          },
          {
            title: 'C API Reference',
            description: 'All assertion macros, test registration, and types.',
            href: '/en/reference/c-api',
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
