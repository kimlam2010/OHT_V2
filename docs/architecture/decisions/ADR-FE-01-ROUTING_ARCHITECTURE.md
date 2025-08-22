# ADR-FE-01: Routing & Code-splitting Architecture

**Status:** Updated  
**Date:** 2025-01-27  
**Team:** Frontend  
**Type:** Architecture Decision Record

## Context

OHT-50 dashboard cần một hệ thống routing hiệu quả để quản lý nhiều trang chức năng, được nâng cấp cho industrial deployment:
- Dashboard tổng quan
- Đồ thị v-a-x real-time
- Nhật ký hệ thống
- Cấu hình an toàn
- Job Monitor
- Chẩn đoán
- User settings
- **NEW:** Hardware Monitoring
- **NEW:** Advanced Safety Configuration
- **NEW:** Performance Analytics
- **NEW:** Maintenance Dashboard

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
      { path: 'settings', element: <UserSettings /> },
      // NEW: Industrial-grade pages
      { path: 'hardware', element: <HardwareMonitoring /> },
      { path: 'safety-advanced', element: <AdvancedSafety /> },
      { path: 'analytics', element: <PerformanceAnalytics /> },
      { path: 'maintenance', element: <MaintenanceDashboard /> }
    ]
  }
];

// Lazy loading implementation
const VAXCharts = lazy(() => import('./pages/VAXCharts'));
const Logs = lazy(() => import('./pages/Logs'));
const HardwareMonitoring = lazy(() => import('./pages/HardwareMonitoring')); // NEW
const AdvancedSafety = lazy(() => import('./pages/AdvancedSafety')); // NEW
const PerformanceAnalytics = lazy(() => import('./pages/PerformanceAnalytics')); // NEW
const MaintenanceDashboard = lazy(() => import('./pages/MaintenanceDashboard')); // NEW
```

**Code splitting strategy:**
- Route-based splitting cho mỗi trang chính
- Component-based splitting cho heavy components
- Vendor splitting cho third-party libraries
- **NEW:** Industrial component splitting cho hardware-intensive pages

## Consequences

### Positive:
- ✅ **Performance:** Giảm bundle size, tăng load time
- ✅ **Maintainability:** Code được tổ chức theo feature
- ✅ **Scalability:** Dễ thêm trang mới
- ✅ **User Experience:** Smooth navigation với loading states
- ✅ **Industrial Features:** Complete industrial monitoring capabilities

### Negative:
- ❌ **Complexity:** Cần quản lý loading states
- ❌ **SEO:** Cần SSR cho public pages (nếu cần)
- ❌ **Bundle:** Multiple HTTP requests cho initial load
- ❌ **Memory Usage:** Industrial components có thể heavy

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

4. **Module Federation (Webpack 5)**
   - Pros: Advanced code splitting, shared dependencies
   - Cons: Complex setup, learning curve

## Implementation Notes

- Sử dụng `React.Suspense` cho loading states
- Implement error boundaries cho route errors
- Preload critical routes khi user hover
- Monitor bundle sizes với webpack-bundle-analyzer
- **NEW:** Implement progressive loading cho industrial components
- **NEW:** Add performance monitoring cho heavy pages

## NEW: Industrial Page Structure

### **Hardware Monitoring Page**
```javascript
// pages/HardwareMonitoring/index.tsx
const HardwareMonitoring = () => {
  return (
    <div className="hardware-monitoring">
      <MotorStatusPanel />
      <SensorHealthPanel />
      <PowerSystemMonitor />
      <MechanicalHealthPanel />
    </div>
  );
};
```

### **Advanced Safety Page**
```javascript
// pages/AdvancedSafety/index.tsx
const AdvancedSafety = () => {
  return (
    <div className="advanced-safety">
      <SafetyConfigPanel />
      <SafetyZoneEditor />
      <SafetyStatusDashboard />
      <ComplianceMonitor />
    </div>
  );
};
```

### **Performance Analytics Page**
```javascript
// pages/PerformanceAnalytics/index.tsx
const PerformanceAnalytics = () => {
  return (
    <div className="performance-analytics">
      <EfficiencyMetricsPanel />
      <PerformanceTrendChart />
      <QualityAssurancePanel />
      <OptimizationRecommendations />
    </div>
  );
};
```

### **Maintenance Dashboard Page**
```javascript
// pages/MaintenanceDashboard/index.tsx
const MaintenanceDashboard = () => {
  return (
    <div className="maintenance-dashboard">
      <PredictiveMaintenancePanel />
      <SparePartsManagement />
      <ServiceHistory />
      <MaintenanceSchedule />
    </div>
  );
};
```

## NEW: Navigation Structure

```javascript
// Updated navigation structure
const navigationItems = [
  { path: '/overview', label: 'Tổng quan', icon: 'home' },
  { path: '/vax-charts', label: 'Đồ thị v‑a‑x', icon: 'chart' },
  { path: '/logs', label: 'Nhật ký', icon: 'file-text' },
  { path: '/safety-config', label: 'Cấu hình an toàn', icon: 'shield' },
  { path: '/jobs', label: 'Job Monitor', icon: 'play' },
  { path: '/diagnostics', label: 'Chẩn đoán', icon: 'wrench' },
  { path: '/settings', label: 'Cài đặt', icon: 'settings' },
  // NEW: Industrial pages
  { path: '/hardware', label: 'Hardware Monitoring', icon: 'cpu' },
  { path: '/safety-advanced', label: 'Safety Advanced', icon: 'shield-check' },
  { path: '/analytics', label: 'Performance Analytics', icon: 'trending-up' },
  { path: '/maintenance', label: 'Maintenance', icon: 'tool' }
];
```

## NEW: Performance Considerations

### **Bundle Size Optimization**
```javascript
// Optimize industrial components
const HardwareMonitoring = lazy(() => 
  import('./pages/HardwareMonitoring').then(module => ({
    default: module.default
  }))
);

// Preload critical industrial pages
const preloadIndustrialPages = () => {
  import('./pages/HardwareMonitoring');
  import('./pages/AdvancedSafety');
};
```

### **Memory Management**
```javascript
// Implement memory cleanup for heavy components
useEffect(() => {
  return () => {
    // Cleanup chart instances
    // Clear WebSocket connections
    // Reset component state
  };
}, []);
```

## Related Documents

- [Frontend Team Quick Guide](../tasks/FRONTEND_TEAM_QUICK_GUIDE.md)
- [Design System Documentation](./ADR-FE-04-DESIGN_SYSTEM.md)
- [Component Library Documentation](../design/COMPONENT_LIBRARY.md)
- [Industrial Standards Documentation](../design/INDUSTRIAL_STANDARDS.md)

## Changelog

**v2.0 (2025-01-27):**
- ✅ Added Hardware Monitoring page routing
- ✅ Added Advanced Safety page routing
- ✅ Added Performance Analytics page routing
- ✅ Added Maintenance Dashboard page routing
- ✅ Updated navigation structure
- ✅ Added performance considerations
- ✅ Enhanced code splitting strategy