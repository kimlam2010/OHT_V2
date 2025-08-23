# ADR-FE-03: Charting Library & Sampling Strategy

**Status:** Accepted  
**Date:** 2025-01-27  
**Team:** Frontend  
**Type:** Architecture Decision Record

## Context

OHT-50 cần hiển thị real-time charts cho:
- Velocity (v) - vận tốc
- Acceleration (a) - gia tốc  
- Position (x) - vị trí
- Historical data analysis
- Performance monitoring

Cần đảm bảo performance tốt với real-time data streaming và responsive design.

## Decision

**Sử dụng Chart.js với custom real-time adapter:**

```javascript
// Chart configuration
const chartConfig = {
  type: 'line',
  data: {
    datasets: [
      {
        label: 'Velocity (m/s)',
        data: [],
        borderColor: 'rgb(75, 192, 192)',
        tension: 0.1
      },
      {
        label: 'Acceleration (m/s²)',
        data: [],
        borderColor: 'rgb(255, 99, 132)',
        tension: 0.1
      },
      {
        label: 'Position (m)',
        data: [],
        borderColor: 'rgb(54, 162, 235)',
        tension: 0.1
      }
    ]
  },
  options: {
    responsive: true,
    animation: false, // Disable cho real-time performance
    scales: {
      x: {
        type: 'time',
        time: {
          unit: 'second'
        }
      }
    }
  }
};

// Sampling strategy
const samplingConfig = {
  live: {
    maxPoints: 100,
    updateInterval: 100, // 10 FPS
    decimation: 'lttb' // Largest-Triangle-Three-Buckets
  },
  historical: {
    maxPoints: 1000,
    updateInterval: 1000,
    decimation: 'min-max'
  }
};
```

**Sampling strategy:**
- **Live mode:** 10 FPS với 100 data points
- **Historical mode:** 1 FPS với 1000 data points
- **Decimation:** LTTB algorithm cho smooth rendering
- **Memory management:** Circular buffer implementation

## Consequences

### Positive:
- ✅ **Performance:** Optimized cho real-time rendering
- ✅ **Memory:** Efficient data management
- ✅ **User Experience:** Smooth animations và interactions
- ✅ **Flexibility:** Support multiple chart types

### Negative:
- ❌ **Bundle Size:** Chart.js library (~200KB)
- ❌ **Complexity:** Custom sampling logic
- ❌ **Browser Support:** Canvas-based rendering

## Alternatives Considered

1. **D3.js**
   - Pros: Maximum flexibility, powerful
   - Cons: Steep learning curve, large bundle size

2. **Recharts**
   - Pros: React-native, declarative
   - Cons: Limited real-time optimization

3. **Custom Canvas implementation**
   - Pros: Full control, minimal bundle
   - Cons: Development time, maintenance overhead

## Implementation Notes

- Use Web Workers cho data processing
- Implement virtual scrolling cho large datasets
- Add zoom/pan controls cho detailed analysis
- Export functionality cho data analysis

## Performance Targets

- **Render time:** < 16ms (60 FPS)
- **Memory usage:** < 50MB cho 1 hour of data
- **Data points:** Support up to 10,000 points
- **Update frequency:** 10 FPS cho live mode

## Related Documents

- [VAX Charts Implementation](../specs/vax_charts_spec.md)
- [Real-time Data Protocol](../specs/realtime_protocol.md)