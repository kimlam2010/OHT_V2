# ADR-FE-01: Routing & Code-splitting Architecture

**Status:** Accepted  
**Date:** 2025-01-27  
**Team:** Frontend  
**Type:** Architecture Decision Record

## Context

OHT-50 dashboard cần một hệ thống routing hiệu quả để quản lý nhiều trang chức năng:
- Dashboard tổng quan
- Đồ thị v-a-x real-time
- Nhật ký hệ thống
- Cấu hình an toàn
- Job Monitor
- Chẩn đoán
- User settings

Cần đảm bảo performance tốt với lazy loading và code splitting.

## Decision

**Sử dụng React Router v6 với lazy loading và code splitting:**

```javascript
// App routing structure
const routes = [
  {
    path: '/',
    element: <Dashboard />,
    children: [
      { path: 'overview', element: <Overview /> },
      { path: 'vax-charts', element: <VAXCharts /> },
      { path: 'logs', element: <Logs /> },
      { path: 'safety-config', element: <SafetyConfig /> },
      { path: 'jobs', element: <JobMonitor /> },
      { path: 'diagnostics', element: <Diagnostics /> },
      { path: 'settings', element: <UserSettings /> }
    ]
  }
];

// Lazy loading implementation
const VAXCharts = lazy(() => import('./pages/VAXCharts'));
const Logs = lazy(() => import('./pages/Logs'));
```

**Code splitting strategy:**
- Route-based splitting cho mỗi trang chính
- Component-based splitting cho heavy components
- Vendor splitting cho third-party libraries

## Consequences

### Positive:
- ✅ **Performance:** Giảm bundle size, tăng load time
- ✅ **Maintainability:** Code được tổ chức theo feature
- ✅ **Scalability:** Dễ thêm trang mới
- ✅ **User Experience:** Smooth navigation với loading states

### Negative:
- ❌ **Complexity:** Cần quản lý loading states
- ❌ **SEO:** Cần SSR cho public pages (nếu cần)
- ❌ **Bundle:** Multiple HTTP requests cho initial load

## Alternatives Considered

1. **Single Page Application (SPA) without code splitting**
   - Pros: Đơn giản, ít HTTP requests
   - Cons: Bundle size lớn, load time chậm

2. **Multi-page Application (MPA)**
   - Pros: SEO tốt, simple architecture
   - Cons: Không có smooth transitions, duplicate code

3. **Micro-frontends**
   - Pros: Team independence, technology diversity
   - Cons: Over-engineering cho project size hiện tại

## Implementation Notes

- Sử dụng `React.Suspense` cho loading states
- Implement error boundaries cho route errors
- Preload critical routes khi user hover
- Monitor bundle sizes với webpack-bundle-analyzer

## Related Documents

- [Frontend Team Quick Guide](../tasks/FRONTEND_TEAM_QUICK_GUIDE.md)
- [Design System Documentation](./ADR-FE-04-DESIGN_SYSTEM.md)