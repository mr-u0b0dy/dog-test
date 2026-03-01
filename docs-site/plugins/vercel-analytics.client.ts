import { computeRoute, inject, pageview } from '@vercel/analytics'

export default defineNuxtPlugin((nuxtApp) => {
  const route = useRoute()

  inject({
    framework: 'nuxt',
    disableAutoTrack: true,
  })

  const sendPageView = () => {
    pageview({
      route: computeRoute(route.path, route.params as Record<string, string | string[]>),
      path: route.path,
    })
  }

  sendPageView()
  nuxtApp.hook('page:finish', sendPageView)
})
