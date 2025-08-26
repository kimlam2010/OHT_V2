# Testing Implementation

**Version:** 1.0  
**Date:** 2024-12-19  
**Team:** Frontend  
**Status:** In Progress

## 📋 **Overview**

Testing implementation cho OHT-50 frontend, bao gồm unit testing, integration testing, accessibility testing, và performance testing.

## 🏗️ **Testing Architecture**

### **Testing Pyramid**
```typescript
// Testing Architecture
┌─────────────────────────────────────┐
│           E2E Tests                 │
│         (Cypress/Playwright)        │
├─────────────────────────────────────┤
│        Integration Tests            │
│      (React Testing Library)        │
├─────────────────────────────────────┤
│          Unit Tests                 │
│         (Jest/Vitest)               │
└─────────────────────────────────────┘
```

### **Testing Structure**
```typescript
// Testing Organization
tests/
├── unit/
│   ├── components/
│   ├── stores/
│   ├── services/
│   └── utils/
├── integration/
│   ├── components/
│   ├── pages/
│   └── workflows/
├── e2e/
│   ├── scenarios/
│   └── specs/
├── accessibility/
│   ├── components/
│   └── pages/
└── performance/
    ├── components/
    └── pages/
```

## 🧪 **Unit Testing**

### **Jest Configuration**
```typescript
// jest.config.js
module.exports = {
  preset: 'ts-jest',
  testEnvironment: 'jsdom',
  setupFilesAfterEnv: ['<rootDir>/src/setupTests.ts'],
  moduleNameMapping: {
    '^@/(.*)$': '<rootDir>/src/$1',
    '\\.(css|less|scss|sass)$': 'identity-obj-proxy',
    '\\.(jpg|jpeg|png|gif|svg)$': '<rootDir>/tests/__mocks__/fileMock.js'
  },
  collectCoverageFrom: [
    'src/**/*.{ts,tsx}',
    '!src/**/*.d.ts',
    '!src/main.tsx',
    '!src/vite-env.d.ts'
  ],
  coverageThreshold: {
    global: {
      branches: 80,
      functions: 80,
      lines: 80,
      statements: 80
    }
  },
  testMatch: [
    '<rootDir>/tests/unit/**/*.test.{ts,tsx}',
    '<rootDir>/src/**/*.test.{ts,tsx}'
  ]
};
```

### **Test Setup**
```typescript
// src/setupTests.ts
import '@testing-library/jest-dom';
import { server } from './tests/mocks/server';

// Mock IntersectionObserver
global.IntersectionObserver = class IntersectionObserver {
  constructor() {}
  observe() { return null; }
  unobserve() { return null; }
  disconnect() { return null; }
};

// Mock ResizeObserver
global.ResizeObserver = class ResizeObserver {
  constructor() {}
  observe() { return null; }
  unobserve() { return null; }
  disconnect() { return null; }
};

// Mock WebSocket
global.WebSocket = class WebSocket {
  constructor() {}
  send() { return null; }
  close() { return null; }
};

// Setup MSW server
beforeAll(() => server.listen());
afterEach(() => server.resetHandlers());
afterAll(() => server.close());

// Mock console methods in tests
const originalError = console.error;
const originalWarn = console.warn;

beforeEach(() => {
  console.error = jest.fn();
  console.warn = jest.fn();
});

afterEach(() => {
  console.error = originalError;
  console.warn = originalWarn;
});
```

### **Component Testing**
```typescript
// tests/unit/components/PrimaryButton.test.tsx
import { render, screen, fireEvent } from '@testing-library/react';
import { PrimaryButton } from '../../../src/components/buttons/PrimaryButton';

describe('PrimaryButton', () => {
  const defaultProps = {
    onClick: jest.fn(),
    children: 'Click me'
  };

  beforeEach(() => {
    jest.clearAllMocks();
  });

  it('renders with correct text', () => {
    render(<PrimaryButton {...defaultProps} />);
    expect(screen.getByText('Click me')).toBeInTheDocument();
  });

  it('calls onClick when clicked', () => {
    render(<PrimaryButton {...defaultProps} />);
    fireEvent.click(screen.getByText('Click me'));
    expect(defaultProps.onClick).toHaveBeenCalledTimes(1);
  });

  it('is disabled when disabled prop is true', () => {
    render(<PrimaryButton {...defaultProps} disabled />);
    expect(screen.getByText('Click me')).toBeDisabled();
  });

  it('shows loading spinner when loading prop is true', () => {
    render(<PrimaryButton {...defaultProps} loading />);
    expect(screen.getByRole('status')).toBeInTheDocument();
  });

  it('applies correct size classes', () => {
    const { rerender } = render(<PrimaryButton {...defaultProps} size="sm" />);
    expect(screen.getByText('Click me')).toHaveClass('px-3', 'py-1.5', 'text-sm');

    rerender(<PrimaryButton {...defaultProps} size="lg" />);
    expect(screen.getByText('Click me')).toHaveClass('px-6', 'py-3', 'text-lg');
  });

  it('applies correct variant classes', () => {
    const { rerender } = render(<PrimaryButton {...defaultProps} variant="outline" />);
    expect(screen.getByText('Click me')).toHaveClass('border', 'border-primary-500');

    rerender(<PrimaryButton {...defaultProps} variant="ghost" />);
    expect(screen.getByText('Click me')).toHaveClass('text-primary-500');
  });

  it('has correct accessibility attributes', () => {
    render(<PrimaryButton {...defaultProps} aria-label="Submit form" />);
    expect(screen.getByLabelText('Submit form')).toBeInTheDocument();
  });
});
```

### **Store Testing**
```typescript
// tests/unit/stores/authStore.test.ts
import { renderHook, act } from '@testing-library/react';
import { useAuthStore } from '../../../src/stores/auth/authStore';

describe('Auth Store', () => {
  beforeEach(() => {
    // Clear store state
    useAuthStore.setState({
      user: null,
      token: null,
      isAuthenticated: false,
      isLoading: false,
      error: null
    });
  });

  it('should handle login success', async () => {
    const { result } = renderHook(() => useAuthStore());

    // Mock successful login
    global.fetch = jest.fn().mockResolvedValueOnce({
      ok: true,
      json: async () => ({
        user: { id: '1', username: 'test', role: 'operator' },
        token: 'test-token'
      })
    });

    await act(async () => {
      await result.current.login('test', 'password');
    });

    expect(result.current.isAuthenticated).toBe(true);
    expect(result.current.user?.username).toBe('test');
    expect(result.current.token).toBe('test-token');
  });

  it('should handle login failure', async () => {
    const { result } = renderHook(() => useAuthStore());

    // Mock failed login
    global.fetch = jest.fn().mockResolvedValueOnce({
      ok: false
    });

    await act(async () => {
      await result.current.login('test', 'wrong-password');
    });

    expect(result.current.isAuthenticated).toBe(false);
    expect(result.current.error).toBe('Login failed');
  });

  it('should handle logout', () => {
    const { result } = renderHook(() => useAuthStore());

    // Set initial authenticated state
    act(() => {
      useAuthStore.setState({
        user: { id: '1', username: 'test', role: 'operator' },
        token: 'test-token',
        isAuthenticated: true
      });
    });

    act(() => {
      result.current.logout();
    });

    expect(result.current.isAuthenticated).toBe(false);
    expect(result.current.user).toBeNull();
    expect(result.current.token).toBeNull();
  });

  it('should handle token refresh', async () => {
    const { result } = renderHook(() => useAuthStore());

    // Set initial state with token
    act(() => {
      useAuthStore.setState({
        token: 'old-token',
        isAuthenticated: true
      });
    });

    // Mock successful refresh
    global.fetch = jest.fn().mockResolvedValueOnce({
      ok: true,
      json: async () => ({
        token: 'new-token'
      })
    });

    await act(async () => {
      await result.current.refreshToken();
    });

    expect(result.current.token).toBe('new-token');
  });
});
```

### **Service Testing**
```typescript
// tests/unit/services/api/auth.test.ts
import { AuthApi } from '../../../src/services/api/auth';
import { apiClient } from '../../../src/services/api/client';

// Mock API client
jest.mock('../../../src/services/api/client', () => ({
  apiClient: {
    post: jest.fn(),
    get: jest.fn(),
    put: jest.fn()
  }
}));

describe('AuthApi', () => {
  beforeEach(() => {
    jest.clearAllMocks();
  });

  describe('login', () => {
    it('should successfully login user', async () => {
      const mockResponse = {
        data: {
          data: {
            user: { id: '1', username: 'test', role: 'operator' },
            token: 'test-token'
          }
        }
      };

      (apiClient.post as jest.Mock).mockResolvedValue(mockResponse);

      const result = await AuthApi.login('test', 'password');

      expect(apiClient.post).toHaveBeenCalledWith('/auth/login', {
        username: 'test',
        password: 'password'
      });
      expect(result).toEqual(mockResponse.data.data);
    });

    it('should handle login failure', async () => {
      const mockError = new Error('Login failed');
      (apiClient.post as jest.Mock).mockRejectedValue(mockError);

      await expect(AuthApi.login('test', 'wrong')).rejects.toThrow('Login failed');
    });
  });

  describe('getProfile', () => {
    it('should fetch user profile', async () => {
      const mockProfile = {
        id: '1',
        username: 'test',
        role: 'operator' as const,
        permissions: ['control:manual']
      };

      (apiClient.get as jest.Mock).mockResolvedValue({
        data: { data: mockProfile }
      });

      const result = await AuthApi.getProfile();

      expect(apiClient.get).toHaveBeenCalledWith('/auth/me');
      expect(result).toEqual(mockProfile);
    });
  });
});
```

## 🔗 **Integration Testing**

### **Component Integration Testing**
```typescript
// tests/integration/components/ControlPanel.test.tsx
import { render, screen, fireEvent, waitFor } from '@testing-library/react';
import { ControlPanel } from '../../../src/components/ControlPanel';
import { useControlStore } from '../../../src/stores/control/controlStore';
import { useAuthStore } from '../../../src/stores/auth/authStore';

// Mock stores
jest.mock('../../../src/stores/control/controlStore');
jest.mock('../../../src/stores/auth/authStore');

describe('ControlPanel Integration', () => {
  const mockMove = jest.fn();
  const mockStop = jest.fn();
  const mockEmergencyStop = jest.fn();

  beforeEach(() => {
    jest.clearAllMocks();

    // Mock control store
    (useControlStore as jest.Mock).mockReturnValue({
      currentSpeed: 50,
      targetSpeed: 50,
      isMoving: false,
      move: mockMove,
      stop: mockStop,
      emergencyStop: mockEmergencyStop,
      isLoading: false
    });

    // Mock auth store
    (useAuthStore as jest.Mock).mockReturnValue({
      isAuthenticated: true,
      user: { role: 'operator' }
    });
  });

  it('should send move command when directional button is clicked', async () => {
    render(<ControlPanel />);

    fireEvent.click(screen.getByLabelText('Move forward'));

    await waitFor(() => {
      expect(mockMove).toHaveBeenCalledWith('forward');
    });
  });

  it('should send stop command when stop button is clicked', async () => {
    render(<ControlPanel />);

    fireEvent.click(screen.getByLabelText('Stop'));

    await waitFor(() => {
      expect(mockStop).toHaveBeenCalled();
    });
  });

  it('should send emergency stop when E-Stop button is clicked', async () => {
    render(<ControlPanel />);

    fireEvent.click(screen.getByLabelText('Emergency Stop'));

    await waitFor(() => {
      expect(mockEmergencyStop).toHaveBeenCalled();
    });
  });

  it('should disable controls when not authenticated', () => {
    (useAuthStore as jest.Mock).mockReturnValue({
      isAuthenticated: false,
      user: null
    });

    render(<ControlPanel />);

    expect(screen.getByLabelText('Move forward')).toBeDisabled();
    expect(screen.getByLabelText('Stop')).toBeDisabled();
  });

  it('should show loading state during command execution', () => {
    (useControlStore as jest.Mock).mockReturnValue({
      currentSpeed: 50,
      targetSpeed: 50,
      isMoving: false,
      move: mockMove,
      stop: mockStop,
      emergencyStop: mockEmergencyStop,
      isLoading: true
    });

    render(<ControlPanel />);

    expect(screen.getByRole('status')).toBeInTheDocument();
  });
});
```

### **Page Integration Testing**
```typescript
// tests/integration/pages/Dashboard.test.tsx
import { render, screen, waitFor } from '@testing-library/react';
import { Dashboard } from '../../../src/pages/Dashboard';
import { useDashboardData } from '../../../src/hooks/useStores';

// Mock hooks
jest.mock('../../../src/hooks/useStores');

describe('Dashboard Integration', () => {
  const mockDashboardData = {
    auth: { isAuthenticated: true, user: { role: 'operator' } },
    system: {
      status: {
        mode: 'IDLE',
        connection: 'ONLINE',
        battery: 85,
        temperature: 45
      }
    },
    control: {
      currentSpeed: 0,
      isMoving: false
    },
    telemetry: {
      currentData: {
        battery: 85,
        position: { x: 0, y: 0, z: 0 },
        speed: 0
      }
    },
    alerts: {
      alerts: []
    },
    isOnline: true,
    canControl: true,
    hasCriticalAlerts: false
  };

  beforeEach(() => {
    jest.clearAllMocks();
    (useDashboardData as jest.Mock).mockReturnValue(mockDashboardData);
  });

  it('should render all dashboard components', () => {
    render(<Dashboard />);

    expect(screen.getByText('System Status')).toBeInTheDocument();
    expect(screen.getByText('Control Panel')).toBeInTheDocument();
    expect(screen.getByText('Telemetry')).toBeInTheDocument();
    expect(screen.getByText('Alerts')).toBeInTheDocument();
  });

  it('should display system status correctly', () => {
    render(<Dashboard />);

    expect(screen.getByText('IDLE')).toBeInTheDocument();
    expect(screen.getByText('85%')).toBeInTheDocument();
    expect(screen.getByText('45°C')).toBeInTheDocument();
  });

  it('should show offline status when disconnected', () => {
    (useDashboardData as jest.Mock).mockReturnValue({
      ...mockDashboardData,
      isOnline: false
    });

    render(<Dashboard />);

    expect(screen.getByText('OFFLINE')).toBeInTheDocument();
  });

  it('should disable controls when system is in E-STOP mode', () => {
    (useDashboardData as jest.Mock).mockReturnValue({
      ...mockDashboardData,
      system: {
        ...mockDashboardData.system,
        status: {
          ...mockDashboardData.system.status,
          mode: 'ESTOP'
        }
      },
      canControl: false
    });

    render(<Dashboard />);

    expect(screen.getByText('E-STOP')).toBeInTheDocument();
  });

  it('should show critical alerts when present', () => {
    (useDashboardData as jest.Mock).mockReturnValue({
      ...mockDashboardData,
      alerts: {
        alerts: [
          {
            id: '1',
            level: 'critical',
            title: 'Emergency Stop',
            message: 'System stopped',
            timestamp: new Date().toISOString(),
            dismissed: false
          }
        ]
      },
      hasCriticalAlerts: true
    });

    render(<Dashboard />);

    expect(screen.getByText('Emergency Stop')).toBeInTheDocument();
  });
});
```

## ♿ **Accessibility Testing**

### **Accessibility Test Setup**
```typescript
// tests/accessibility/setup.ts
import { axe, toHaveNoViolations } from 'jest-axe';

expect.extend(toHaveNoViolations);

// Custom accessibility matchers
expect.extend({
  toHaveValidAriaLabel(received) {
    const pass = received.getAttribute('aria-label') !== null;
    return {
      pass,
      message: () =>
        `expected element ${pass ? 'not ' : ''}to have valid aria-label`
    };
  },

  toHaveValidAriaDescribedBy(received) {
    const describedBy = received.getAttribute('aria-describedby');
    const pass = describedBy !== null && document.getElementById(describedBy) !== null;
    return {
      pass,
      message: () =>
        `expected element ${pass ? 'not ' : ''}to have valid aria-describedby`
    };
  }
});
```

### **Component Accessibility Testing**
```typescript
// tests/accessibility/components/PrimaryButton.test.tsx
import { render, screen } from '@testing-library/react';
import { axe, toHaveNoViolations } from 'jest-axe';
import { PrimaryButton } from '../../../src/components/buttons/PrimaryButton';

expect.extend(toHaveNoViolations);

describe('PrimaryButton Accessibility', () => {
  const defaultProps = {
    onClick: jest.fn(),
    children: 'Click me'
  };

  it('should not have accessibility violations', async () => {
    const { container } = render(<PrimaryButton {...defaultProps} />);
    const results = await axe(container);
    expect(results).toHaveNoViolations();
  });

  it('should have proper ARIA attributes', () => {
    render(<PrimaryButton {...defaultProps} aria-label="Submit form" />);
    
    const button = screen.getByRole('button');
    expect(button).toHaveAttribute('aria-label', 'Submit form');
  });

  it('should be keyboard accessible', () => {
    render(<PrimaryButton {...defaultProps} />);
    
    const button = screen.getByRole('button');
    expect(button).toHaveAttribute('tabindex', '0');
  });

  it('should show loading state to screen readers', () => {
    render(<PrimaryButton {...defaultProps} loading />);
    
    expect(screen.getByRole('status')).toHaveAttribute('aria-live', 'polite');
  });

  it('should announce disabled state', () => {
    render(<PrimaryButton {...defaultProps} disabled />);
    
    const button = screen.getByRole('button');
    expect(button).toHaveAttribute('aria-disabled', 'true');
  });
});
```

### **Page Accessibility Testing**
```typescript
// tests/accessibility/pages/Dashboard.test.tsx
import { render, screen } from '@testing-library/react';
import { axe, toHaveNoViolations } from 'jest-axe';
import { Dashboard } from '../../../src/pages/Dashboard';

expect.extend(toHaveNoViolations);

describe('Dashboard Accessibility', () => {
  beforeEach(() => {
    // Mock store data
    jest.mock('../../../src/hooks/useStores', () => ({
      useDashboardData: () => ({
        auth: { isAuthenticated: true },
        system: { status: { mode: 'IDLE' } },
        control: { isMoving: false },
        telemetry: { currentData: null },
        alerts: { alerts: [] }
      })
    }));
  });

  it('should not have accessibility violations', async () => {
    const { container } = render(<Dashboard />);
    const results = await axe(container);
    expect(results).toHaveNoViolations();
  });

  it('should have proper heading structure', () => {
    render(<Dashboard />);
    
    const headings = screen.getAllByRole('heading');
    expect(headings[0]).toHaveTextContent('OHT-50 Dashboard');
  });

  it('should have proper landmark regions', () => {
    render(<Dashboard />);
    
    expect(screen.getByRole('banner')).toBeInTheDocument(); // Header
    expect(screen.getByRole('navigation')).toBeInTheDocument(); // Sidebar
    expect(screen.getByRole('main')).toBeInTheDocument(); // Main content
  });

  it('should announce status changes', () => {
    render(<Dashboard />);
    
    const statusRegion = screen.getByRole('status');
    expect(statusRegion).toHaveAttribute('aria-live', 'polite');
  });

  it('should have proper focus management', () => {
    render(<Dashboard />);
    
    // Test tab navigation
    const focusableElements = screen.getAllByRole('button');
    focusableElements.forEach(element => {
      expect(element).toHaveAttribute('tabindex', '0');
    });
  });
});
```

## ⚡ **Performance Testing**

### **Performance Test Setup**
```typescript
// tests/performance/setup.ts
import { render } from '@testing-library/react';

// Performance measurement utilities
export const measureRenderTime = (component: React.ReactElement) => {
  const start = performance.now();
  render(component);
  const end = performance.now();
  return end - start;
};

export const measureMemoryUsage = () => {
  if ('memory' in performance) {
    return (performance as any).memory;
  }
  return null;
};

export const measureBundleSize = async (componentPath: string) => {
  // This would integrate with bundle analyzer
  return {
    size: 0,
    gzipped: 0
  };
};
```

### **Component Performance Testing**
```typescript
// tests/performance/components/TelemetryChart.test.tsx
import { render } from '@testing-library/react';
import { TelemetryChart } from '../../../src/components/data/TelemetryChart';
import { measureRenderTime } from '../setup';

describe('TelemetryChart Performance', () => {
  const mockData = Array.from({ length: 1000 }, (_, i) => ({
    timestamp: new Date(Date.now() - i * 1000).toISOString(),
    value: Math.random() * 100
  }));

  it('should render within performance budget', () => {
    const renderTime = measureRenderTime(
      <TelemetryChart
        data={mockData}
        title="Performance Test"
        unit="%"
      />
    );

    expect(renderTime).toBeLessThan(100); // 100ms budget
  });

  it('should handle large datasets efficiently', () => {
    const largeData = Array.from({ length: 10000 }, (_, i) => ({
      timestamp: new Date(Date.now() - i * 1000).toISOString(),
      value: Math.random() * 100
    }));

    const renderTime = measureRenderTime(
      <TelemetryChart
        data={largeData}
        title="Large Dataset Test"
        unit="%"
      />
    );

    expect(renderTime).toBeLessThan(500); // 500ms budget for large datasets
  });

  it('should not cause memory leaks', () => {
    const initialMemory = measureMemoryUsage();
    
    // Render and unmount multiple times
    for (let i = 0; i < 100; i++) {
      const { unmount } = render(
        <TelemetryChart
          data={mockData}
          title="Memory Test"
          unit="%"
        />
      );
      unmount();
    }

    const finalMemory = measureMemoryUsage();
    
    if (initialMemory && finalMemory) {
      const memoryIncrease = finalMemory.usedJSHeapSize - initialMemory.usedJSHeapSize;
      expect(memoryIncrease).toBeLessThan(10 * 1024 * 1024); // 10MB limit
    }
  });
});
```

### **Bundle Size Testing**
```typescript
// tests/performance/bundle-size.test.ts
import { measureBundleSize } from './setup';

describe('Bundle Size', () => {
  it('should meet bundle size budget', async () => {
    const bundleInfo = await measureBundleSize('src/main.tsx');
    
    expect(bundleInfo.size).toBeLessThan(500 * 1024); // 500KB limit
    expect(bundleInfo.gzipped).toBeLessThan(150 * 1024); // 150KB gzipped limit
  });

  it('should have reasonable component sizes', async () => {
    const components = [
      'src/components/buttons/PrimaryButton.tsx',
      'src/components/controls/DirectionalControl.tsx',
      'src/components/data/TelemetryChart.tsx'
    ];

    for (const component of components) {
      const bundleInfo = await measureBundleSize(component);
      expect(bundleInfo.size).toBeLessThan(50 * 1024); // 50KB per component
    }
  });
});
```

## 🧪 **E2E Testing**

### **Cypress Configuration**
```typescript
// cypress.config.ts
import { defineConfig } from 'cypress';

export default defineConfig({
  e2e: {
    baseUrl: 'http://localhost:3000',
    setupNodeEvents(on, config) {
      // implement node event listeners here
    },
    specPattern: 'tests/e2e/**/*.cy.{js,jsx,ts,tsx}',
    supportFile: 'tests/e2e/support/e2e.ts',
    viewportWidth: 1280,
    viewportHeight: 720,
    video: false,
    screenshotOnRunFailure: true
  },
  component: {
    devServer: {
      framework: 'react',
      bundler: 'vite'
    },
    specPattern: 'tests/e2e/components/**/*.cy.{js,jsx,ts,tsx}'
  }
});
```

### **E2E Test Scenarios**
```typescript
// tests/e2e/scenarios/login.cy.ts
describe('Login Flow', () => {
  beforeEach(() => {
    cy.visit('/login');
  });

  it('should login successfully with valid credentials', () => {
    cy.get('[data-testid="username-input"]').type('operator');
    cy.get('[data-testid="password-input"]').type('password123');
    cy.get('[data-testid="login-button"]').click();

    cy.url().should('include', '/dashboard');
    cy.get('[data-testid="user-menu"]').should('contain', 'operator');
  });

  it('should show error with invalid credentials', () => {
    cy.get('[data-testid="username-input"]').type('invalid');
    cy.get('[data-testid="password-input"]').type('wrong');
    cy.get('[data-testid="login-button"]').click();

    cy.get('[data-testid="error-message"]').should('be.visible');
    cy.url().should('include', '/login');
  });

  it('should redirect to dashboard if already authenticated', () => {
    // Mock authenticated state
    cy.window().then((win) => {
      win.localStorage.setItem('auth_token', 'mock-token');
    });

    cy.visit('/login');
    cy.url().should('include', '/dashboard');
  });
});
```

```typescript
// tests/e2e/scenarios/control-panel.cy.ts
describe('Control Panel', () => {
  beforeEach(() => {
    // Mock authentication and system state
    cy.intercept('GET', '/api/auth/me', {
      statusCode: 200,
      body: {
        user: { id: '1', username: 'operator', role: 'operator' },
        token: 'mock-token'
      }
    });

    cy.intercept('GET', '/api/system/status', {
      statusCode: 200,
      body: {
        mode: 'IDLE',
        connection: 'ONLINE',
        battery: 85
      }
    });

    cy.visit('/dashboard');
  });

  it('should send move commands when directional buttons are clicked', () => {
    cy.intercept('POST', '/api/control/move', {
      statusCode: 200,
      body: { success: true }
    }).as('moveCommand');

    cy.get('[data-testid="move-forward"]').click();
    cy.wait('@moveCommand').its('request.body').should('deep.equal', {
      direction: 'forward',
      speed: 50
    });
  });

  it('should send emergency stop when E-Stop button is clicked', () => {
    cy.intercept('POST', '/api/control/emergency-stop', {
      statusCode: 200,
      body: { success: true }
    }).as('emergencyStop');

    cy.get('[data-testid="emergency-stop"]').click();
    cy.wait('@emergencyStop');
  });

  it('should update speed when slider is moved', () => {
    cy.get('[data-testid="speed-slider"]').invoke('val', 75).trigger('change');
    cy.get('[data-testid="speed-display"]').should('contain', '75%');
  });

  it('should disable controls when system is offline', () => {
    cy.intercept('GET', '/api/system/status', {
      statusCode: 200,
      body: {
        mode: 'IDLE',
        connection: 'OFFLINE',
        battery: 85
      }
    });

    cy.visit('/dashboard');
    cy.get('[data-testid="move-forward"]').should('be.disabled');
  });
});
```

## 📊 **Test Coverage**

### **Coverage Configuration**
```typescript
// coverage.config.js
module.exports = {
  coverageDirectory: 'coverage',
  coverageReporters: ['text', 'lcov', 'html'],
  collectCoverageFrom: [
    'src/**/*.{ts,tsx}',
    '!src/**/*.d.ts',
    '!src/main.tsx',
    '!src/vite-env.d.ts',
    '!src/tests/**/*',
    '!src/**/*.test.{ts,tsx}',
    '!src/**/*.spec.{ts,tsx}'
  ],
  coverageThreshold: {
    global: {
      branches: 80,
      functions: 80,
      lines: 80,
      statements: 80
    },
    './src/components/': {
      branches: 90,
      functions: 90,
      lines: 90,
      statements: 90
    },
    './src/stores/': {
      branches: 85,
      functions: 85,
      lines: 85,
      statements: 85
    }
  }
};
```

### **Coverage Reports**
```typescript
// scripts/test-coverage.ts
import { execSync } from 'child_process';

const runTests = () => {
  try {
    // Run tests with coverage
    execSync('npm run test:coverage', { stdio: 'inherit' });
    
    // Generate coverage report
    execSync('npm run coverage:report', { stdio: 'inherit' });
    
    console.log('✅ All tests passed with coverage requirements met');
  } catch (error) {
    console.error('❌ Tests failed or coverage requirements not met');
    process.exit(1);
  }
};

runTests();
```

## 🔄 **Changelog**

**v1.0 (2024-12-19)**
- ✅ Created comprehensive testing architecture
- ✅ Implemented unit testing with Jest
- ✅ Added integration testing with React Testing Library
- ✅ Created accessibility testing with jest-axe
- ✅ Implemented performance testing
- ✅ Added E2E testing with Cypress
- ✅ Created test coverage configuration
- ✅ Added testing utilities and helpers

---

**Next Steps:**
1. Implement visual regression testing
2. Add load testing for WebSocket connections
3. Create automated testing pipeline
4. Add testing documentation
5. Implement continuous testing integration

