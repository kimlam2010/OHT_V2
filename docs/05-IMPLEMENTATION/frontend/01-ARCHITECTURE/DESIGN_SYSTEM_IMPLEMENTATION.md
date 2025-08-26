# 🎨 Design System Implementation - OHT-50 Dashboard

**Phiên bản:** v1.0  
**Ngày tạo:** 2025-01-28  
**Trạng thái:** ✅ Hoàn thành  
**Đối tượng đọc:** Frontend Developers, UI/UX Designers, Design System Managers

---

## 🎯 **Mục tiêu Design System Implementation:**

### **Mục đích chính:**
- Implement comprehensive design system cho OHT-50 dashboard
- Tuân thủ UI/UX Design Requirements đã được hoàn thành
- Đảm bảo consistency và maintainability
- Tối ưu hóa cho safety-first design principles

### **Nguyên tắc implementation:**
1. **Consistency:** Unified design language across all components
2. **Safety-First:** Components prioritize safety và accessibility
3. **Scalability:** Modular system for easy maintenance
4. **Accessibility:** WCAG 2.1 AA compliance built-in
5. **Performance:** Optimized cho real-time updates

---

## 🎨 **DESIGN TOKENS IMPLEMENTATION:**

### **Color System:**
```css
/* src/styles/design-tokens.css */
:root {
  /* Primary Colors - Industrial Orange Theme */
  --primary-orange: #FF6B35;
  --primary-orange-light: #FF8A5C;
  --primary-orange-dark: #E55A2B;
  --primary-orange-50: #FFF7F5;
  --primary-orange-100: #FFEDE8;
  --primary-orange-200: #FFD4C7;
  --primary-orange-300: #FFB8A3;
  --primary-orange-400: #FF9A7A;
  --primary-orange-500: #FF6B35;
  --primary-orange-600: #E55A2B;
  --primary-orange-700: #CC4A22;
  --primary-orange-800: #B33A1A;
  --primary-orange-900: #992A12;
  
  /* Industrial Grays */
  --industrial-gray-50: #F9FAFB;
  --industrial-gray-100: #F3F4F6;
  --industrial-gray-200: #E5E7EB;
  --industrial-gray-300: #D1D5DB;
  --industrial-gray-400: #9CA3AF;
  --industrial-gray-500: #6B7280;
  --industrial-gray-600: #4B5563;
  --industrial-gray-700: #374151;
  --industrial-gray-800: #1F2937;
  --industrial-gray-900: #111827;
  
  /* Background Colors */
  --bg-primary: #FFFFFF;
  --bg-secondary: #F9FAFB;
  --bg-tertiary: #F3F4F6;
  --bg-overlay: rgba(0, 0, 0, 0.5);
  
  /* Semantic Colors */
  --success-green: #10B981;
  --success-green-light: #34D399;
  --success-green-dark: #059669;
  
  --warning-yellow: #F59E0B;
  --warning-yellow-light: #FBBF24;
  --warning-yellow-dark: #D97706;
  
  --error-red: #EF4444;
  --error-red-light: #F87171;
  --error-red-dark: #DC2626;
  
  --info-blue: #3B82F6;
  --info-blue-light: #60A5FA;
  --info-blue-dark: #2563EB;
  
  /* Safety Colors */
  --safety-red: #DC2626;
  --safety-orange: #EA580C;
  --safety-yellow: #CA8A04;
  --safety-green: #16A34A;
  
  /* Status Colors */
  --status-online: #10B981;
  --status-offline: #6B7280;
  --status-warning: #F59E0B;
  --status-error: #EF4444;
  --status-maintenance: #8B5CF6;
  
  /* Border Colors */
  --border-light: #E5E7EB;
  --border-medium: #D1D5DB;
  --border-dark: #9CA3AF;
  --border-focus: #FF6B35;
}
```

### **Typography System:**
```css
/* Typography System */
:root {
  /* Font Families */
  --font-sans: 'Inter', -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, 'Helvetica Neue', Arial, sans-serif;
  --font-mono: 'JetBrains Mono', 'Fira Code', 'Source Code Pro', 'SF Mono', Monaco, 'Cascadia Code', 'Roboto Mono', Consolas, 'Courier New', monospace;
  --font-display: 'Space Grotesk', 'Inter', sans-serif;
  
  /* Font Sizes */
  --text-xs: 0.75rem;    /* 12px */
  --text-sm: 0.875rem;   /* 14px */
  --text-base: 1rem;     /* 16px */
  --text-lg: 1.125rem;   /* 18px */
  --text-xl: 1.25rem;    /* 20px */
  --text-2xl: 1.5rem;    /* 24px */
  --text-3xl: 1.875rem;  /* 30px */
  --text-4xl: 2.25rem;   /* 36px */
  --text-5xl: 3rem;      /* 48px */
  --text-6xl: 3.75rem;   /* 60px */
  
  /* Font Weights */
  --font-light: 300;
  --font-normal: 400;
  --font-medium: 500;
  --font-semibold: 600;
  --font-bold: 700;
  --font-extrabold: 800;
  --font-black: 900;
  
  /* Line Heights */
  --leading-none: 1;
  --leading-tight: 1.25;
  --leading-snug: 1.375;
  --leading-normal: 1.5;
  --leading-relaxed: 1.625;
  --leading-loose: 2;
  
  /* Letter Spacing */
  --tracking-tighter: -0.05em;
  --tracking-tight: -0.025em;
  --tracking-normal: 0em;
  --tracking-wide: 0.025em;
  --tracking-wider: 0.05em;
  --tracking-widest: 0.1em;
}
```

### **Spacing System:**
```css
/* Spacing System */
:root {
  /* Base Spacing Unit */
  --space-unit: 0.25rem; /* 4px */
  
  /* Spacing Scale */
  --space-0: 0;
  --space-px: 1px;
  --space-0.5: 0.125rem;  /* 2px */
  --space-1: 0.25rem;     /* 4px */
  --space-1.5: 0.375rem;  /* 6px */
  --space-2: 0.5rem;      /* 8px */
  --space-2.5: 0.625rem;  /* 10px */
  --space-3: 0.75rem;     /* 12px */
  --space-3.5: 0.875rem;  /* 14px */
  --space-4: 1rem;        /* 16px */
  --space-5: 1.25rem;     /* 20px */
  --space-6: 1.5rem;      /* 24px */
  --space-7: 1.75rem;     /* 28px */
  --space-8: 2rem;        /* 32px */
  --space-9: 2.25rem;     /* 36px */
  --space-10: 2.5rem;     /* 40px */
  --space-11: 2.75rem;    /* 44px */
  --space-12: 3rem;       /* 48px */
  --space-14: 3.5rem;     /* 56px */
  --space-16: 4rem;       /* 64px */
  --space-20: 5rem;       /* 80px */
  --space-24: 6rem;       /* 96px */
  --space-28: 7rem;       /* 112px */
  --space-32: 8rem;       /* 128px */
  --space-36: 9rem;       /* 144px */
  --space-40: 10rem;      /* 160px */
  --space-44: 11rem;      /* 176px */
  --space-48: 12rem;      /* 192px */
  --space-52: 13rem;      /* 208px */
  --space-56: 14rem;      /* 224px */
  --space-60: 15rem;      /* 240px */
  --space-64: 16rem;      /* 256px */
  --space-72: 18rem;      /* 288px */
  --space-80: 20rem;      /* 320px */
  --space-96: 24rem;      /* 384px */
  
  /* Component Spacing */
  --component-padding-xs: var(--space-2);
  --component-padding-sm: var(--space-3);
  --component-padding-md: var(--space-4);
  --component-padding-lg: var(--space-6);
  --component-padding-xl: var(--space-8);
  
  /* Layout Spacing */
  --layout-gap-xs: var(--space-2);
  --layout-gap-sm: var(--space-4);
  --layout-gap-md: var(--space-6);
  --layout-gap-lg: var(--space-8);
  --layout-gap-xl: var(--space-12);
}
```

### **Border Radius System:**
```css
/* Border Radius System */
:root {
  --radius-none: 0;
  --radius-sm: 0.125rem;   /* 2px */
  --radius-base: 0.25rem;  /* 4px */
  --radius-md: 0.375rem;   /* 6px */
  --radius-lg: 0.5rem;     /* 8px */
  --radius-xl: 0.75rem;    /* 12px */
  --radius-2xl: 1rem;      /* 16px */
  --radius-3xl: 1.5rem;    /* 24px */
  --radius-full: 9999px;
}
```

### **Shadow System:**
```css
/* Shadow System */
:root {
  --shadow-xs: 0 1px 2px 0 rgba(0, 0, 0, 0.05);
  --shadow-sm: 0 1px 3px 0 rgba(0, 0, 0, 0.1), 0 1px 2px 0 rgba(0, 0, 0, 0.06);
  --shadow-base: 0 1px 3px 0 rgba(0, 0, 0, 0.1), 0 1px 2px 0 rgba(0, 0, 0, 0.06);
  --shadow-md: 0 4px 6px -1px rgba(0, 0, 0, 0.1), 0 2px 4px -1px rgba(0, 0, 0, 0.06);
  --shadow-lg: 0 10px 15px -3px rgba(0, 0, 0, 0.1), 0 4px 6px -2px rgba(0, 0, 0, 0.05);
  --shadow-xl: 0 20px 25px -5px rgba(0, 0, 0, 0.1), 0 10px 10px -5px rgba(0, 0, 0, 0.04);
  --shadow-2xl: 0 25px 50px -12px rgba(0, 0, 0, 0.25);
  --shadow-inner: inset 0 2px 4px 0 rgba(0, 0, 0, 0.06);
  
  /* Focus Shadows */
  --shadow-focus: 0 0 0 3px rgba(255, 107, 53, 0.1);
  --shadow-focus-error: 0 0 0 3px rgba(239, 68, 68, 0.1);
  --shadow-focus-success: 0 0 0 3px rgba(16, 185, 129, 0.1);
  --shadow-focus-warning: 0 0 0 3px rgba(245, 158, 11, 0.1);
  
  /* Emergency Shadows */
  --shadow-emergency: 0 0 0 4px rgba(220, 38, 38, 0.3);
  --shadow-emergency-pulse: 0 0 0 8px rgba(220, 38, 38, 0.2);
}
```

---

## 🧩 **COMPONENT LIBRARY IMPLEMENTATION:**

### **Button Components:**
```typescript
// src/components/ui/Button/Button.tsx
import React from 'react';
import { cn } from '../../../utils/cn';

export interface ButtonProps extends React.ButtonHTMLAttributes<HTMLButtonElement> {
  variant?: 'primary' | 'secondary' | 'danger' | 'warning' | 'success' | 'ghost' | 'emergency';
  size?: 'xs' | 'sm' | 'md' | 'lg' | 'xl';
  loading?: boolean;
  disabled?: boolean;
  children: React.ReactNode;
}

const Button: React.FC<ButtonProps> = ({
  variant = 'primary',
  size = 'md',
  loading = false,
  disabled = false,
  className,
  children,
  ...props
}) => {
  const baseClasses = 'inline-flex items-center justify-center font-medium rounded-md transition-all duration-200 focus:outline-none focus:ring-2 focus:ring-offset-2 disabled:opacity-50 disabled:cursor-not-allowed';
  
  const variantClasses = {
    primary: 'bg-primary-orange text-white hover:bg-primary-orange-dark focus:ring-primary-orange',
    secondary: 'bg-industrial-gray-100 text-industrial-gray-700 hover:bg-industrial-gray-200 focus:ring-industrial-gray-500',
    danger: 'bg-error-red text-white hover:bg-error-red-dark focus:ring-error-red',
    warning: 'bg-warning-yellow text-industrial-gray-800 hover:bg-warning-yellow-dark focus:ring-warning-yellow',
    success: 'bg-success-green text-white hover:bg-success-green-dark focus:ring-success-green',
    ghost: 'bg-transparent text-industrial-gray-700 hover:bg-industrial-gray-100 focus:ring-industrial-gray-500',
    emergency: 'bg-safety-red text-white hover:bg-safety-red-dark focus:ring-safety-red shadow-emergency animate-pulse'
  };
  
  const sizeClasses = {
    xs: 'px-2 py-1 text-xs',
    sm: 'px-3 py-1.5 text-sm',
    md: 'px-4 py-2 text-sm',
    lg: 'px-6 py-3 text-base',
    xl: 'px-8 py-4 text-lg'
  };
  
  return (
    <button
      className={cn(
        baseClasses,
        variantClasses[variant],
        sizeClasses[size],
        loading && 'opacity-75 cursor-wait',
        className
      )}
      disabled={disabled || loading}
      {...props}
    >
      {loading && (
        <svg className="animate-spin -ml-1 mr-2 h-4 w-4" fill="none" viewBox="0 0 24 24">
          <circle className="opacity-25" cx="12" cy="12" r="10" stroke="currentColor" strokeWidth="4" />
          <path className="opacity-75" fill="currentColor" d="M4 12a8 8 0 018-8V0C5.373 0 0 5.373 0 12h4zm2 5.291A7.962 7.962 0 014 12H0c0 3.042 1.135 5.824 3 7.938l3-2.647z" />
        </svg>
      )}
      {children}
    </button>
  );
};

export default Button;
```

### **Input Components:**
```typescript
// src/components/ui/Input/Input.tsx
import React from 'react';
import { cn } from '../../../utils/cn';

export interface InputProps extends React.InputHTMLAttributes<HTMLInputElement> {
  label?: string;
  error?: string;
  helperText?: string;
  leftIcon?: React.ReactNode;
  rightIcon?: React.ReactNode;
}

const Input: React.FC<InputProps> = ({
  label,
  error,
  helperText,
  leftIcon,
  rightIcon,
  className,
  ...props
}) => {
  return (
    <div className="w-full">
      {label && (
        <label className="block text-sm font-medium text-industrial-gray-700 mb-1">
          {label}
        </label>
      )}
      <div className="relative">
        {leftIcon && (
          <div className="absolute inset-y-0 left-0 pl-3 flex items-center pointer-events-none">
            <div className="h-5 w-5 text-industrial-gray-400">
              {leftIcon}
            </div>
          </div>
        )}
        <input
          className={cn(
            'block w-full rounded-md border border-industrial-gray-300 px-3 py-2 text-industrial-gray-900 placeholder-industrial-gray-400 focus:border-primary-orange focus:outline-none focus:ring-1 focus:ring-primary-orange sm:text-sm',
            leftIcon && 'pl-10',
            rightIcon && 'pr-10',
            error && 'border-error-red focus:border-error-red focus:ring-error-red',
            className
          )}
          {...props}
        />
        {rightIcon && (
          <div className="absolute inset-y-0 right-0 pr-3 flex items-center pointer-events-none">
            <div className="h-5 w-5 text-industrial-gray-400">
              {rightIcon}
            </div>
          </div>
        )}
      </div>
      {error && (
        <p className="mt-1 text-sm text-error-red">{error}</p>
      )}
      {helperText && !error && (
        <p className="mt-1 text-sm text-industrial-gray-500">{helperText}</p>
      )}
    </div>
  );
};

export default Input;
```

### **Card Components:**
```typescript
// src/components/ui/Card/Card.tsx
import React from 'react';
import { cn } from '../../../utils/cn';

export interface CardProps {
  children: React.ReactNode;
  className?: string;
  padding?: 'none' | 'sm' | 'md' | 'lg' | 'xl';
  shadow?: 'none' | 'sm' | 'md' | 'lg' | 'xl';
  border?: boolean;
}

const Card: React.FC<CardProps> = ({
  children,
  className,
  padding = 'md',
  shadow = 'md',
  border = true
}) => {
  const paddingClasses = {
    none: '',
    sm: 'p-3',
    md: 'p-6',
    lg: 'p-8',
    xl: 'p-12'
  };
  
  const shadowClasses = {
    none: '',
    sm: 'shadow-sm',
    md: 'shadow-md',
    lg: 'shadow-lg',
    xl: 'shadow-xl'
  };
  
  return (
    <div
      className={cn(
        'bg-white rounded-lg',
        border && 'border border-industrial-gray-200',
        shadowClasses[shadow],
        paddingClasses[padding],
        className
      )}
    >
      {children}
    </div>
  );
};

export default Card;
```

---

## 🚨 **SAFETY-FIRST COMPONENTS:**

### **Emergency Stop Button:**
```typescript
// src/components/safety/EmergencyStop/EmergencyStop.tsx
import React, { useCallback } from 'react';
import { useRobotStore } from '../../../stores/robotStore';
import { useAlertStore } from '../../../stores/alertStore';
import Button from '../../ui/Button/Button';

const EmergencyStop: React.FC = () => {
  const { emergencyStop } = useRobotStore();
  const { addAlert } = useAlertStore();
  
  const handleEmergencyStop = useCallback(() => {
    // Immediate action - no confirmation needed
    emergencyStop();
    
    // Add critical alert
    addAlert({
      id: `emergency-${Date.now()}`,
      type: 'critical',
      title: 'Emergency Stop Activated',
      message: 'Robot has been stopped due to emergency stop activation.',
      timestamp: new Date(),
      acknowledged: false
    });
    
    // Play emergency sound
    playEmergencySound();
  }, [emergencyStop, addAlert]);
  
  const playEmergencySound = () => {
    try {
      const audio = new Audio('/sounds/emergency-stop.mp3');
      audio.volume = 0.8;
      audio.play();
    } catch (error) {
      console.warn('Could not play emergency sound:', error);
    }
  };
  
  return (
    <Button
      variant="emergency"
      size="xl"
      onClick={handleEmergencyStop}
      className="w-full h-16 text-xl font-bold uppercase tracking-wider"
      aria-label="Emergency Stop - Immediate robot halt"
      data-testid="emergency-stop"
    >
      <svg className="w-8 h-8 mr-3" fill="currentColor" viewBox="0 0 24 24">
        <path d="M6 6h12v12H6z"/>
      </svg>
      Emergency Stop
    </Button>
  );
};

export default EmergencyStop;
```

### **Alert System Component:**
```typescript
// src/components/safety/AlertSystem/AlertSystem.tsx
import React from 'react';
import { useAlertStore } from '../../../stores/alertStore';
import { cn } from '../../../utils/cn';

const AlertSystem: React.FC = () => {
  const { alerts, removeAlert, acknowledgeAlert } = useAlertStore();
  
  const getAlertClasses = (type: string) => {
    const baseClasses = 'flex items-center p-4 rounded-lg border-l-4';
    
    switch (type) {
      case 'critical':
        return cn(baseClasses, 'bg-error-red-50 border-error-red text-error-red-800');
      case 'warning':
        return cn(baseClasses, 'bg-warning-yellow-50 border-warning-yellow text-warning-yellow-800');
      case 'info':
        return cn(baseClasses, 'bg-info-blue-50 border-info-blue text-info-blue-800');
      case 'success':
        return cn(baseClasses, 'bg-success-green-50 border-success-green text-success-green-800');
      default:
        return cn(baseClasses, 'bg-industrial-gray-50 border-industrial-gray-300 text-industrial-gray-800');
    }
  };
  
  const getAlertIcon = (type: string) => {
    switch (type) {
      case 'critical':
        return (
          <svg className="w-5 h-5" fill="currentColor" viewBox="0 0 20 20">
            <path fillRule="evenodd" d="M8.257 3.099c.765-1.36 2.722-1.36 3.486 0l5.58 9.92c.75 1.334-.213 2.98-1.742 2.98H4.42c-1.53 0-2.493-1.646-1.743-2.98l5.58-9.92zM11 13a1 1 0 11-2 0 1 1 0 012 0zm-1-8a1 1 0 00-1 1v3a1 1 0 002 0V6a1 1 0 00-1-1z" clipRule="evenodd" />
          </svg>
        );
      case 'warning':
        return (
          <svg className="w-5 h-5" fill="currentColor" viewBox="0 0 20 20">
            <path fillRule="evenodd" d="M8.257 3.099c.765-1.36 2.722-1.36 3.486 0l5.58 9.92c.75 1.334-.213 2.98-1.742 2.98H4.42c-1.53 0-2.493-1.646-1.743-2.98l5.58-9.92zM11 13a1 1 0 11-2 0 1 1 0 012 0zm-1-8a1 1 0 00-1 1v3a1 1 0 002 0V6a1 1 0 00-1-1z" clipRule="evenodd" />
          </svg>
        );
      case 'info':
        return (
          <svg className="w-5 h-5" fill="currentColor" viewBox="0 0 20 20">
            <path fillRule="evenodd" d="M18 10a8 8 0 11-16 0 8 8 0 0116 0zm-7-4a1 1 0 11-2 0 1 1 0 012 0zM9 9a1 1 0 000 2v3a1 1 0 001 1h1a1 1 0 100-2v-3a1 1 0 00-1-1H9z" clipRule="evenodd" />
          </svg>
        );
      case 'success':
        return (
          <svg className="w-5 h-5" fill="currentColor" viewBox="0 0 20 20">
            <path fillRule="evenodd" d="M10 18a8 8 0 100-16 8 8 0 000 16zm3.707-9.293a1 1 0 00-1.414-1.414L9 10.586 7.707 9.293a1 1 0 00-1.414 1.414l2 2a1 1 0 001.414 0l4-4z" clipRule="evenodd" />
          </svg>
        );
      default:
        return null;
    }
  };
  
  if (alerts.length === 0) return null;
  
  return (
    <div className="fixed top-4 right-4 z-50 space-y-2 max-w-md">
      {alerts.map((alert) => (
        <div
          key={alert.id}
          className={cn(
            getAlertClasses(alert.type),
            'animate-slide-in-right'
          )}
          role="alert"
          aria-live="assertive"
        >
          <div className="flex-shrink-0 mr-3">
            {getAlertIcon(alert.type)}
          </div>
          <div className="flex-1">
            <h3 className="text-sm font-medium">{alert.title}</h3>
            <p className="text-sm mt-1">{alert.message}</p>
          </div>
          <div className="flex-shrink-0 ml-3">
            <button
              onClick={() => removeAlert(alert.id)}
              className="text-current hover:opacity-75 focus:outline-none focus:ring-2 focus:ring-offset-2 focus:ring-current rounded"
              aria-label="Dismiss alert"
            >
              <svg className="w-4 h-4" fill="currentColor" viewBox="0 0 20 20">
                <path fillRule="evenodd" d="M4.293 4.293a1 1 0 011.414 0L10 8.586l4.293-4.293a1 1 0 111.414 1.414L11.414 10l4.293 4.293a1 1 0 01-1.414 1.414L10 11.414l-4.293 4.293a1 1 0 01-1.414-1.414L8.586 10 4.293 5.707a1 1 0 010-1.414z" clipRule="evenodd" />
              </svg>
            </button>
          </div>
        </div>
      ))}
    </div>
  );
};

export default AlertSystem;
```

---

## 📱 **RESPONSIVE DESIGN IMPLEMENTATION:**

### **Responsive Utilities:**
```typescript
// src/hooks/useResponsive.ts
import { useState, useEffect } from 'react';

export type Breakpoint = 'mobile' | 'tablet' | 'desktop' | 'large';

export const useResponsive = () => {
  const [breakpoint, setBreakpoint] = useState<Breakpoint>('mobile');
  const [isMobile, setIsMobile] = useState(false);
  const [isTablet, setIsTablet] = useState(false);
  const [isDesktop, setIsDesktop] = useState(false);
  const [isLarge, setIsLarge] = useState(false);
  
  useEffect(() => {
    const handleResize = () => {
      const width = window.innerWidth;
      
      if (width < 768) {
        setBreakpoint('mobile');
        setIsMobile(true);
        setIsTablet(false);
        setIsDesktop(false);
        setIsLarge(false);
      } else if (width < 1024) {
        setBreakpoint('tablet');
        setIsMobile(false);
        setIsTablet(true);
        setIsDesktop(false);
        setIsLarge(false);
      } else if (width < 1440) {
        setBreakpoint('desktop');
        setIsMobile(false);
        setIsTablet(false);
        setIsDesktop(true);
        setIsLarge(false);
      } else {
        setBreakpoint('large');
        setIsMobile(false);
        setIsTablet(false);
        setIsDesktop(false);
        setIsLarge(true);
      }
    };
    
    handleResize();
    window.addEventListener('resize', handleResize);
    
    return () => window.removeEventListener('resize', handleResize);
  }, []);
  
  return {
    breakpoint,
    isMobile,
    isTablet,
    isDesktop,
    isLarge
  };
};
```

### **Responsive Components:**
```typescript
// src/components/layout/Sidebar/Sidebar.tsx
import React, { useState } from 'react';
import { useResponsive } from '../../../hooks/useResponsive';
import { cn } from '../../../utils/cn';

interface SidebarProps {
  children: React.ReactNode;
  className?: string;
}

const Sidebar: React.FC<SidebarProps> = ({ children, className }) => {
  const { isMobile, isTablet } = useResponsive();
  const [isOpen, setIsOpen] = useState(!isMobile);
  
  const toggleSidebar = () => setIsOpen(!isOpen);
  
  return (
    <>
      {/* Mobile overlay */}
      {isMobile && isOpen && (
        <div
          className="fixed inset-0 bg-black bg-opacity-50 z-40 lg:hidden"
          onClick={toggleSidebar}
        />
      )}
      
      {/* Sidebar */}
      <aside
        className={cn(
          'fixed top-0 left-0 h-full bg-white border-r border-industrial-gray-200 z-50 transition-transform duration-300 ease-in-out',
          isMobile ? 'w-64 transform' : 'w-64',
          isMobile && !isOpen && '-translate-x-full',
          isTablet && 'w-72',
          !isMobile && !isTablet && 'w-80',
          className
        )}
      >
        <div className="flex flex-col h-full">
          {/* Header */}
          <div className="flex items-center justify-between p-4 border-b border-industrial-gray-200">
            <h2 className="text-lg font-semibold text-industrial-gray-900">
              OHT-50 Control
            </h2>
            {isMobile && (
              <button
                onClick={toggleSidebar}
                className="p-2 rounded-md text-industrial-gray-400 hover:text-industrial-gray-600 hover:bg-industrial-gray-100"
              >
                <svg className="w-5 h-5" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                  <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M6 18L18 6M6 6l12 12" />
                </svg>
              </button>
            )}
          </div>
          
          {/* Content */}
          <div className="flex-1 overflow-y-auto p-4">
            {children}
          </div>
        </div>
      </aside>
      
      {/* Mobile toggle button */}
      {isMobile && !isOpen && (
        <button
          onClick={toggleSidebar}
          className="fixed top-4 left-4 z-50 p-2 bg-primary-orange text-white rounded-md shadow-lg"
        >
          <svg className="w-5 h-5" fill="none" stroke="currentColor" viewBox="0 0 24 24">
            <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M4 6h16M4 12h16M4 18h16" />
          </svg>
        </button>
      )}
    </>
  );
};

export default Sidebar;
```

---

## ♿ **ACCESSIBILITY IMPLEMENTATION:**

### **Accessibility Utilities:**
```typescript
// src/utils/accessibility.ts
export const focusManager = {
  trap: (element: HTMLElement) => {
    const focusableElements = element.querySelectorAll(
      'button, [href], input, select, textarea, [tabindex]:not([tabindex="-1"])'
    );
    
    const firstElement = focusableElements[0] as HTMLElement;
    const lastElement = focusableElements[focusableElements.length - 1] as HTMLElement;
    
    const handleKeyDown = (e: KeyboardEvent) => {
      if (e.key === 'Tab') {
        if (e.shiftKey) {
          if (document.activeElement === firstElement) {
            e.preventDefault();
            lastElement.focus();
          }
        } else {
          if (document.activeElement === lastElement) {
            e.preventDefault();
            firstElement.focus();
          }
        }
      }
    };
    
    element.addEventListener('keydown', handleKeyDown);
    firstElement.focus();
    
    return () => {
      element.removeEventListener('keydown', handleKeyDown);
    };
  },
  
  announce: (message: string, priority: 'polite' | 'assertive' = 'polite') => {
    const announcement = document.createElement('div');
    announcement.setAttribute('aria-live', priority);
    announcement.setAttribute('aria-atomic', 'true');
    announcement.className = 'sr-only';
    announcement.textContent = message;
    
    document.body.appendChild(announcement);
    
    setTimeout(() => {
      document.body.removeChild(announcement);
    }, 1000);
  }
};

export const keyboardShortcuts = {
  emergencyStop: (callback: () => void) => {
    const handleKeyDown = (e: KeyboardEvent) => {
      if ((e.ctrlKey || e.metaKey) && e.key === 'm') {
        e.preventDefault();
        callback();
      }
    };
    
    document.addEventListener('keydown', handleKeyDown);
    
    return () => {
      document.removeEventListener('keydown', handleKeyDown);
    };
  }
};
```

### **Accessible Components:**
```typescript
// src/components/ui/Modal/Modal.tsx
import React, { useEffect, useRef } from 'react';
import { createPortal } from 'react-dom';
import { focusManager } from '../../../utils/accessibility';
import { cn } from '../../../utils/cn';

interface ModalProps {
  isOpen: boolean;
  onClose: () => void;
  title: string;
  children: React.ReactNode;
  className?: string;
}

const Modal: React.FC<ModalProps> = ({
  isOpen,
  onClose,
  title,
  children,
  className
}) => {
  const modalRef = useRef<HTMLDivElement>(null);
  
  useEffect(() => {
    if (isOpen && modalRef.current) {
      const cleanup = focusManager.trap(modalRef.current);
      return cleanup;
    }
  }, [isOpen]);
  
  useEffect(() => {
    const handleEscape = (e: KeyboardEvent) => {
      if (e.key === 'Escape') {
        onClose();
      }
    };
    
    if (isOpen) {
      document.addEventListener('keydown', handleEscape);
      document.body.style.overflow = 'hidden';
    }
    
    return () => {
      document.removeEventListener('keydown', handleEscape);
      document.body.style.overflow = 'unset';
    };
  }, [isOpen, onClose]);
  
  if (!isOpen) return null;
  
  return createPortal(
    <div
      className="fixed inset-0 z-50 flex items-center justify-center"
      role="dialog"
      aria-modal="true"
      aria-labelledby="modal-title"
    >
      {/* Backdrop */}
      <div
        className="absolute inset-0 bg-black bg-opacity-50"
        onClick={onClose}
      />
      
      {/* Modal */}
      <div
        ref={modalRef}
        className={cn(
          'relative bg-white rounded-lg shadow-xl max-w-md w-full mx-4',
          className
        )}
      >
        {/* Header */}
        <div className="flex items-center justify-between p-6 border-b border-industrial-gray-200">
          <h2 id="modal-title" className="text-lg font-semibold text-industrial-gray-900">
            {title}
          </h2>
          <button
            onClick={onClose}
            className="p-2 rounded-md text-industrial-gray-400 hover:text-industrial-gray-600 hover:bg-industrial-gray-100 focus:outline-none focus:ring-2 focus:ring-primary-orange"
            aria-label="Close modal"
          >
            <svg className="w-5 h-5" fill="none" stroke="currentColor" viewBox="0 0 24 24">
              <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M6 18L18 6M6 6l12 12" />
            </svg>
          </button>
        </div>
        
        {/* Content */}
        <div className="p-6">
          {children}
        </div>
      </div>
    </div>,
    document.body
  );
};

export default Modal;
```

---

## 📚 **REFERENCES:**

### **UI/UX Requirements:**
- [Design System](../../../../02-REQUIREMENTS/07-UI-UX-DESIGN-REQUIREMENTS/REQ_UI_UX_Design_System.md)
- [Visual Design](../../../../02-REQUIREMENTS/07-UI-UX-DESIGN-REQUIREMENTS/REQ_UI_UX_Visual_Design.md)
- [Accessibility](../../../../02-REQUIREMENTS/07-UI-UX-DESIGN-REQUIREMENTS/REQ_UI_UX_Accessibility.md)

### **Design Assets:**
- [SuperDesign Iterations](../../../../../.superdesign/design_iterations/)
- [Design Plan](../../../../../.superdesign/OHT-50_DESIGN_PLAN.md)

---

**Changelog v1.0:**
- ✅ Created comprehensive design system implementation
- ✅ Implemented design tokens (colors, typography, spacing, shadows)
- ✅ Created component library với safety-first components
- ✅ Added responsive design implementation
- ✅ Implemented accessibility features
- ✅ Created emergency controls với immediate response
- ✅ Added alert system với real-time notifications
- ✅ Implemented modal system với focus management

**🚨 Lưu ý:** Design system implementation tuân thủ strict safety-first principles với industrial orange theme cho robot OHT-50.
