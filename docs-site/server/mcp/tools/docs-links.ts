import { z } from 'zod'

export default defineMcpTool({
  description: 'Return key documentation URLs for MCP and LLM integrations.',
  inputSchema: {
    locale: z.enum(['en', 'fr', 'ru', 'ja']).default('en').describe('Preferred locale for integration links.'),
  },
  handler: async ({ locale }) => {
    const links = locale === 'fr'
      ? [
          '/fr/getting-started/introduction',
          '/fr/integrations/integration-mcp',
          '/fr/integrations/integration-llm',
          '/llms.txt',
          '/llms-full.txt',
        ]
      : locale === 'ru'
      ? [
          '/ru/getting-started/introduction',
          '/ru/integrations/mcp-integration',
          '/ru/integrations/llm-integration',
          '/llms.txt',
          '/llms-full.txt',
        ]
      : locale === 'ja'
      ? [
          '/ja/getting-started/introduction',
          '/ja/integrations/mcp-integration',
          '/ja/integrations/llm-integration',
          '/llms.txt',
          '/llms-full.txt',
        ]
      : [
          '/en/getting-started/introduction',
          '/en/integrations/mcp-integration',
          '/en/integrations/llm-integration',
          '/llms.txt',
          '/llms-full.txt',
        ]

    return {
      content: [
        {
          type: 'text',
          text: links.join('\n'),
        },
      ],
    }
  },
})
