# ADR-FE-02: Data Fetching & Error Handling Strategy

**Status:** Accepted  
**Date:** 2025-01-27  
**Team:** Frontend  
**Type:** Architecture Decision Record

## Context

OHT-50 dashboard cần xử lý nhiều loại data:
- Real-time telemetry từ WebSocket
- Configuration data từ REST API
- Historical data cho charts
- User authentication data
- Error states và loading states

Cần đảm bảo reliability, performance và user experience tốt.

## Decision

**Sử dụng custom `apiFetchJson` utility với retry/backoff strategy:**

```javascript
// Core API utility
const apiFetchJson = async (url, options = {}) => {
  const config = {
    baseURL: process.env.REACT_APP_API_BASE_URL,
    timeout: 10000,
    retries: 3,
    backoff: 'exponential',
    ...options
  };
  
  return fetchWithRetry(url, config);
};

// WebSocket connection manager
class WebSocketManager {
  constructor(url) {
    this.url = url;
    this.reconnectAttempts = 0;
    this.maxReconnectAttempts = 5;
    this.reconnectDelay = 1000;
  }
  
  connect() {
    // Implementation with exponential backoff
  }
  
  onMessage(handler) {
    // Message handling with error recovery
  }
}
```

**Error handling strategy:**
- Global error boundary cho React errors
- Toast notifications cho user-facing errors
- Retry mechanism cho network failures
- Graceful degradation cho partial failures

## Consequences

### Positive:
- ✅ **Reliability:** Automatic retry với exponential backoff
- ✅ **User Experience:** Clear error messages và loading states
- ✅ **Performance:** Caching và optimistic updates
- ✅ **Maintainability:** Centralized error handling

### Negative:
- ❌ **Complexity:** Cần quản lý multiple error states
- ❌ **Bundle Size:** Additional utility code
- ❌ **Testing:** Cần test error scenarios

## Alternatives Considered

1. **Axios với interceptors**
   - Pros: Feature-rich, mature library
   - Cons: Additional dependency, bundle size

2. **React Query/SWR**
   - Pros: Built-in caching, background updates
   - Cons: Learning curve, overkill cho simple cases

3. **Native fetch với manual handling**
   - Pros: No dependencies, full control
   - Cons: Boilerplate code, manual retry logic

## Implementation Notes

- Implement exponential backoff: 1s, 2s, 4s, 8s, 16s
- Cache successful responses trong localStorage
- Show loading skeletons cho better UX
- Log errors cho debugging

## Error Categories

1. **Network Errors:** Retry automatically
2. **Authentication Errors:** Redirect to login
3. **Validation Errors:** Show inline validation
4. **Server Errors:** Show user-friendly message
5. **Unknown Errors:** Log và show generic message

## Related Documents

- [Backend API Documentation](../backend/TELEMETRY_API.md)
- [WebSocket Protocol Specification](../specs/websocket_protocol.md)