# ADR-FE-06: Progressive Web App (PWA) Strategy

**Status:** Accepted  
**Date:** 2025-01-27  
**Team:** Frontend  
**Type:** Architecture Decision Record

## Context

OHT-50 dashboard cần hỗ trợ:
- Offline functionality cho critical operations
- Mobile app-like experience
- Installation trên mobile devices
- Background sync cho telemetry data
- Push notifications cho alerts
- Performance optimization

Cần đảm bảo reliability và user experience tốt.

## Decision

**Implement PWA với Workbox cho service worker management:**

```javascript
// PWA configuration
const pwaConfig = {
  name: 'OHT-50 Dashboard',
  shortName: 'OHT-50',
  description: 'OHT-50 Master Module Dashboard',
  themeColor: '#1f2937',
  backgroundColor: '#ffffff',
  display: 'standalone',
  orientation: 'portrait',
  scope: '/',
  startUrl: '/',
  
  // Icons
  icons: [
    {
      src: '/icons/icon-192x192.png',
      sizes: '192x192',
      type: 'image/png'
    },
    {
      src: '/icons/icon-512x512.png',
      sizes: '512x512',
      type: 'image/png'
    }
  ],
  
  // Service worker
  workbox: {
    runtimeCaching: [
      {
        urlPattern: /^https:\/\/api\.oht50\.com\/.*/,
        handler: 'NetworkFirst',
        options: {
          cacheName: 'api-cache',
          expiration: {
            maxEntries: 100,
            maxAgeSeconds: 60 * 60 * 24 // 24 hours
          }
        }
      },
      {
        urlPattern: /\.(js|css|png|jpg|jpeg|svg|gif)$/,
        handler: 'CacheFirst',
        options: {
          cacheName: 'static-assets',
          expiration: {
            maxEntries: 200,
            maxAgeSeconds: 60 * 60 * 24 * 30 // 30 days
          }
        }
      }
    ]
  }
};

// Service worker registration
if ('serviceWorker' in navigator) {
  window.addEventListener('load', () => {
    navigator.serviceWorker.register('/sw.js')
      .then(registration => {
        console.log('SW registered:', registration);
      })
      .catch(error => {
        console.log('SW registration failed:', error);
      });
  });
}
```

**PWA features:**
- **Offline support:** Critical UI components cached
- **Background sync:** Telemetry data sync khi online
- **Push notifications:** System alerts và status updates
- **App installation:** Add to home screen
- **Performance:** Resource caching và optimization

## Consequences

### Positive:
- ✅ **Offline Support:** Critical functions work offline
- ✅ **Mobile Experience:** App-like interface
- ✅ **Performance:** Faster loading với caching
- ✅ **User Engagement:** Push notifications
- ✅ **Installation:** Add to home screen

### Negative:
- ❌ **Complexity:** Service worker management
- ❌ **Testing:** Need to test offline scenarios
- ❌ **Browser Support:** Limited PWA support trên iOS
- ❌ **Bundle Size:** Additional PWA assets

## Alternatives Considered

1. **Native mobile app**
   - Pros: Full platform features, better performance
   - Cons: Development overhead, app store distribution

2. **Hybrid app (Cordova/PhoneGap)**
   - Pros: Cross-platform, web technologies
   - Cons: Performance issues, limited native features

3. **Responsive web app only**
   - Pros: Simple, no PWA complexity
   - Cons: No offline support, limited mobile features

## Implementation Notes

- Use Workbox cho service worker generation
- Implement offline-first architecture
- Add background sync cho critical data
- Test PWA features trên multiple devices

## PWA Features

### Core Features:
1. **Offline Dashboard:** Basic UI và controls
2. **Data Caching:** Recent telemetry data
3. **Background Sync:** Sync khi connection restored
4. **Push Notifications:** System alerts

### Advanced Features:
1. **App Installation:** Add to home screen
2. **Splash Screen:** Custom loading screen
3. **Theme Support:** Dark/light mode
4. **Performance:** Resource optimization

## Performance Targets

- **First Load:** < 3 seconds
- **Offline Load:** < 1 second
- **Cache Hit Rate:** > 90%
- **Background Sync:** < 5 minutes delay

## Browser Support

- **Chrome:** Full PWA support
- **Firefox:** Full PWA support
- **Safari:** Limited PWA support
- **Edge:** Full PWA support

## Related Documents

- [PWA Implementation Guide](../design/PWA_IMPLEMENTATION.md)
- [Service Worker Strategy](../design/SERVICE_WORKER.md)
- [Offline Architecture](../design/OFFLINE_ARCHITECTURE.md)