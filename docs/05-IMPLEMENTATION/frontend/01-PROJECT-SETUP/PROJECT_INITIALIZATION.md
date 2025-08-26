# OHT-50 Frontend Project Initialization

**Phiên bản:** v1.0  
**Ngày tạo:** 2024-08-24  
**Trạng thái:** Implementation Phase  
**Phase:** 1 - Foundation & Setup

---

## 🚀 **PROJECT SETUP**

### **1. Initialize React + TypeScript Project**

```bash
# Create new Vite project
npm create vite@latest oht-50-frontend -- --template react-ts

# Navigate to project
cd oht-50-frontend

# Install dependencies
npm install
```

### **2. Install Core Dependencies**

```bash
# UI Framework & Styling
npm install tailwindcss @tailwindcss/forms @tailwindcss/typography
npm install flowbite flowbite-react
npm install lucide-react

# State Management
npm install zustand

# Charts & Visualization
npm install chart.js react-chartjs-2
npm install recharts

# Routing
npm install react-router-dom

# HTTP Client
npm install axios

# Utilities
npm install clsx tailwind-merge
npm install date-fns
npm install react-hook-form @hookform/resolvers zod

# Development Dependencies
npm install -D @types/node
npm install -D eslint @typescript-eslint/parser @typescript-eslint/eslint-plugin
npm install -D prettier eslint-config-prettier eslint-plugin-prettier
npm install -D @testing-library/react @testing-library/jest-dom @testing-library/user-event
npm install -D jest jest-environment-jsdom @types/jest
npm install -D vite-tsconfig-paths
```

### **3. Project Structure**

```
oht-50-frontend/
├── public/
│   ├── favicon.ico
│   └── assets/
├── src/
│   ├── components/
│   │   ├── ui/                 # Base UI components
│   │   ├── safety/            # Safety-first components
│   │   ├── control/           # Control components
│   │   ├── data/              # Data display components
│   │   └── layout/            # Layout components
│   ├── hooks/
│   │   ├── useAuth.ts
│   │   ├── useWebSocket.ts
│   │   └── useTelemetry.ts
│   ├── stores/
│   │   ├── authStore.ts
│   │   ├── systemStore.ts
│   │   ├── controlStore.ts
│   │   ├── telemetryStore.ts
│   │   ├── alertStore.ts
│   │   └── websocketStore.ts
│   ├── services/
│   │   ├── api/
│   │   │   ├── client.ts
│   │   │   ├── auth.ts
│   │   │   ├── system.ts
│   │   │   ├── control.ts
│   │   │   ├── telemetry.ts
│   │   │   └── missions.ts
│   │   └── websocket/
│   │       └── client.ts
│   ├── utils/
│   │   ├── constants.ts
│   │   ├── helpers.ts
│   │   ├── validation.ts
│   │   └── formatters.ts
│   ├── types/
│   │   ├── api.ts
│   │   ├── system.ts
│   │   ├── control.ts
│   │   └── telemetry.ts
│   ├── pages/
│   │   ├── Dashboard.tsx
│   │   ├── Control.tsx
│   │   ├── Missions.tsx
│   │   ├── Configuration.tsx
│   │   └── Login.tsx
│   ├── styles/
│   │   ├── globals.css
│   │   └── design-system.css
│   ├── App.tsx
│   ├── main.tsx
│   └── vite-env.d.ts
├── tests/
│   ├── components/
│   ├── stores/
│   └── utils/
├── .eslintrc.js
├── .prettierrc
├── tailwind.config.js
├── tsconfig.json
├── vite.config.ts
├── package.json
└── README.md
```

---

## ⚙️ **CONFIGURATION FILES**

### **1. Vite Configuration (vite.config.ts)**

```typescript
import { defineConfig } from 'vite'
import react from '@vitejs/plugin-react'
import tsconfigPaths from 'vite-tsconfig-paths'
import path from 'path'

export default defineConfig({
  plugins: [react(), tsconfigPaths()],
  resolve: {
    alias: {
      '@': path.resolve(__dirname, './src'),
      '@components': path.resolve(__dirname, './src/components'),
      '@stores': path.resolve(__dirname, './src/stores'),
      '@services': path.resolve(__dirname, './src/services'),
      '@utils': path.resolve(__dirname, './src/utils'),
      '@types': path.resolve(__dirname, './src/types'),
      '@pages': path.resolve(__dirname, './src/pages'),
      '@styles': path.resolve(__dirname, './src/styles'),
    },
  },
  server: {
    port: 3000,
    host: true,
  },
  build: {
    outDir: 'dist',
    sourcemap: true,
    rollupOptions: {
      output: {
        manualChunks: {
          vendor: ['react', 'react-dom'],
          charts: ['chart.js', 'react-chartjs-2', 'recharts'],
          ui: ['flowbite', 'flowbite-react'],
        },
      },
    },
  },
  test: {
    globals: true,
    environment: 'jsdom',
    setupFiles: ['./src/test/setup.ts'],
  },
})
```

### **2. TypeScript Configuration (tsconfig.json)**

```json
{
  "compilerOptions": {
    "target": "ES2020",
    "useDefineForClassFields": true,
    "lib": ["ES2020", "DOM", "DOM.Iterable"],
    "module": "ESNext",
    "skipLibCheck": true,
    "moduleResolution": "bundler",
    "allowImportingTsExtensions": true,
    "resolveJsonModule": true,
    "isolatedModules": true,
    "noEmit": true,
    "jsx": "react-jsx",
    "strict": true,
    "noUnusedLocals": true,
    "noUnusedParameters": true,
    "noFallthroughCasesInSwitch": true,
    "baseUrl": ".",
    "paths": {
      "@/*": ["src/*"],
      "@components/*": ["src/components/*"],
      "@stores/*": ["src/stores/*"],
      "@services/*": ["src/services/*"],
      "@utils/*": ["src/utils/*"],
      "@types/*": ["src/types/*"],
      "@pages/*": ["src/pages/*"],
      "@styles/*": ["src/styles/*"]
    }
  },
  "include": ["src", "tests"],
  "references": [{ "path": "./tsconfig.node.json" }]
}
```

### **3. Tailwind Configuration (tailwind.config.js)**

```javascript
/** @type {import('tailwindcss').Config} */
export default {
  content: [
    "./index.html",
    "./src/**/*.{js,ts,jsx,tsx}",
    "./node_modules/flowbite/**/*.js",
    "./node_modules/flowbite-react/**/*.js",
  ],
  theme: {
    extend: {
      colors: {
        // OHT-50 Design System Colors
        primary: {
          50: '#fff7ed',
          100: '#ffedd5',
          200: '#fed7aa',
          300: '#fdba74',
          400: '#fb923c',
          500: '#f97316', // Main orange
          600: '#ea580c',
          700: '#c2410c',
          800: '#9a3412',
          900: '#7c2d12',
        },
        industrial: {
          50: '#f8fafc',
          100: '#f1f5f9',
          200: '#e2e8f0',
          300: '#cbd5e1',
          400: '#94a3b8',
          500: '#64748b',
          600: '#475569',
          700: '#334155',
          800: '#1e293b',
          900: '#0f172a',
        },
        safety: {
          success: '#10b981',
          warning: '#f59e0b',
          error: '#ef4444',
          info: '#3b82f6',
        },
        status: {
          idle: '#6b7280',
          moving: '#3b82f6',
          docking: '#f59e0b',
          fault: '#ef4444',
          estop: '#dc2626',
        },
      },
      fontFamily: {
        sans: ['Inter', 'ui-sans-serif', 'system-ui'],
        mono: ['JetBrains Mono', 'ui-monospace', 'monospace'],
      },
      spacing: {
        '18': '4.5rem',
        '88': '22rem',
        '128': '32rem',
      },
      borderRadius: {
        '4xl': '2rem',
      },
      boxShadow: {
        'industrial': '0 4px 6px -1px rgba(0, 0, 0, 0.1), 0 2px 4px -1px rgba(0, 0, 0, 0.06)',
        'safety': '0 0 0 3px rgba(239, 68, 68, 0.5)',
      },
      animation: {
        'pulse-slow': 'pulse 3s cubic-bezier(0.4, 0, 0.6, 1) infinite',
        'bounce-slow': 'bounce 2s infinite',
        'shake': 'shake 0.5s ease-in-out',
      },
      keyframes: {
        shake: {
          '0%, 100%': { transform: 'translateX(0)' },
          '25%': { transform: 'translateX(-5px)' },
          '75%': { transform: 'translateX(5px)' },
        },
      },
    },
  },
  plugins: [
    require('@tailwindcss/forms'),
    require('@tailwindcss/typography'),
    require('flowbite/plugin'),
  ],
}
```

### **4. ESLint Configuration (.eslintrc.js)**

```javascript
module.exports = {
  root: true,
  env: {
    browser: true,
    es2021: true,
    node: true,
  },
  extends: [
    'eslint:recommended',
    '@typescript-eslint/recommended',
    'plugin:react/recommended',
    'plugin:react-hooks/recommended',
    'plugin:@typescript-eslint/recommended',
    'prettier',
  ],
  parser: '@typescript-eslint/parser',
  parserOptions: {
    ecmaFeatures: {
      jsx: true,
    },
    ecmaVersion: 12,
    sourceType: 'module',
  },
  plugins: ['react', '@typescript-eslint', 'prettier'],
  rules: {
    'prettier/prettier': 'error',
    'react/react-in-jsx-scope': 'off',
    'react/prop-types': 'off',
    '@typescript-eslint/explicit-module-boundary-types': 'off',
    '@typescript-eslint/no-unused-vars': ['error', { argsIgnorePattern: '^_' }],
    'no-console': ['warn', { allow: ['warn', 'error'] }],
  },
  settings: {
    react: {
      version: 'detect',
    },
  },
}
```

### **5. Prettier Configuration (.prettierrc)**

```json
{
  "semi": true,
  "trailingComma": "es5",
  "singleQuote": true,
  "printWidth": 80,
  "tabWidth": 2,
  "useTabs": false,
  "bracketSpacing": true,
  "arrowParens": "avoid",
  "endOfLine": "lf"
}
```

---

## 🎨 **DESIGN SYSTEM SETUP**

### **1. Global Styles (src/styles/globals.css)**

```css
@import url('https://fonts.googleapis.com/css2?family=Inter:wght@300;400;500;600;700&family=JetBrains+Mono:wght@400;500;600&display=swap');
@tailwind base;
@tailwind components;
@tailwind utilities;

@layer base {
  html {
    font-family: 'Inter', ui-sans-serif, system-ui, sans-serif;
  }
  
  body {
    @apply bg-industrial-50 text-industrial-900;
  }
  
  * {
    @apply border-industrial-200;
  }
}

@layer components {
  /* Safety-first components */
  .safety-button {
    @apply relative overflow-hidden transition-all duration-200;
  }
  
  .safety-button:active {
    @apply transform scale-95;
  }
  
  .emergency-stop {
    @apply bg-safety-error text-white shadow-safety;
  }
  
  .emergency-stop:hover {
    @apply bg-red-700 shadow-lg;
  }
  
  /* Control components */
  .control-pad {
    @apply bg-white rounded-lg shadow-industrial border border-industrial-200;
  }
  
  .control-button {
    @apply bg-industrial-100 hover:bg-industrial-200 text-industrial-700 
           font-medium px-4 py-2 rounded-md transition-colors duration-200;
  }
  
  /* Status indicators */
  .status-indicator {
    @apply inline-flex items-center px-2.5 py-0.5 rounded-full text-xs font-medium;
  }
  
  .status-idle { @apply bg-status-idle text-white; }
  .status-moving { @apply bg-status-moving text-white; }
  .status-docking { @apply bg-status-docking text-white; }
  .status-fault { @apply bg-status-fault text-white; }
  .status-estop { @apply bg-status-estop text-white; }
  
  /* Data display */
  .data-card {
    @apply bg-white rounded-lg shadow-industrial border border-industrial-200 p-6;
  }
  
  .metric-value {
    @apply text-2xl font-bold text-industrial-900;
  }
  
  .metric-label {
    @apply text-sm font-medium text-industrial-600 uppercase tracking-wide;
  }
}

@layer utilities {
  .text-balance {
    text-wrap: balance;
  }
  
  .scrollbar-hide {
    -ms-overflow-style: none;
    scrollbar-width: none;
  }
  
  .scrollbar-hide::-webkit-scrollbar {
    display: none;
  }
}
```

### **2. Design System CSS (src/styles/design-system.css)**

```css
/* Import from theme_1.css */
:root {
  --background: oklch(1.0000 0 0);
  --foreground: oklch(0.1448 0 0);
  --card: oklch(1.0000 0 0);
  --card-foreground: oklch(0.1448 0 0);
  --popover: oklch(1.0000 0 0);
  --popover-foreground: oklch(0.1448 0 0);
  --primary: oklch(0.5555 0 0);
  --primary-foreground: oklch(0.9851 0 0);
  --secondary: oklch(0.9702 0 0);
  --secondary-foreground: oklch(0.2046 0 0);
  --muted: oklch(0.9702 0 0);
  --muted-foreground: oklch(0.5486 0 0);
  --accent: oklch(0.9702 0 0);
  --accent-foreground: oklch(0.2046 0 0);
  --destructive: oklch(0.5830 0.2387 28.4765);
  --destructive-foreground: oklch(0.9702 0 0);
  --border: oklch(0.9219 0 0);
  --input: oklch(0.9219 0 0);
  --ring: oklch(0.7090 0 0);
  --chart-1: oklch(0.5555 0 0);
  --chart-2: oklch(0.5555 0 0);
  --chart-3: oklch(0.5555 0 0);
  --chart-4: oklch(0.5555 0 0);
  --chart-5: oklch(0.5555 0 0);
  --sidebar: oklch(0.9851 0 0);
  --sidebar-foreground: oklch(0.1448 0 0);
  --sidebar-primary: oklch(0.2046 0 0);
  --sidebar-primary-foreground: oklch(0.9851 0 0);
  --sidebar-accent: oklch(0.9702 0 0);
  --sidebar-accent-foreground: oklch(0.2046 0 0);
  --sidebar-border: oklch(0.9219 0 0);
  --sidebar-ring: oklch(0.7090 0 0);
  --font-sans: 'Inter', ui-sans-serif, system-ui, sans-serif;
  --font-serif: 'Inter', ui-serif, Georgia, Cambria, "Times New Roman", Times, serif;
  --font-mono: 'JetBrains Mono', ui-monospace, SFMono-Regular, Menlo, Monaco, Consolas, "Liberation Mono", "Courier New", monospace;
  --radius: 0rem;
  --shadow-2xs: 0px 1px 0px 0px hsl(0 0% 0% / 0.00);
  --shadow-xs: 0px 1px 0px 0px hsl(0 0% 0% / 0.00);
  --shadow-sm: 0px 1px 0px 0px hsl(0 0% 0% / 0.00), 0px 1px 2px -1px hsl(0 0% 0% / 0.00);
  --shadow: 0px 1px 0px 0px hsl(0 0% 0% / 0.00), 0px 1px 2px -1px hsl(0 0% 0% / 0.00);
  --shadow-md: 0px 1px 0px 0px hsl(0 0% 0% / 0.00), 0px 2px 4px -1px hsl(0 0% 0% / 0.00);
  --shadow-lg: 0px 1px 0px 0px hsl(0 0% 0% / 0.00), 0px 4px 6px -1px hsl(0 0% 0% / 0.00);
  --shadow-xl: 0px 1px 0px 0px hsl(0 0% 0% / 0.00), 0px 8px 10px -1px hsl(0 0% 0% / 0.00);
  --shadow-2xl: 0px 1px 0px 0px hsl(0 0% 0% / 0.00);
  --tracking-normal: 0em;
  --spacing: 0.25rem;
}

/* Animation keyframes */
@keyframes slideInLeft {
  from {
    transform: translateX(-100%);
    opacity: 0;
  }
  to {
    transform: translateX(0);
    opacity: 1;
  }
}

@keyframes slideInUp {
  from {
    transform: translateY(20px);
    opacity: 0;
  }
  to {
    transform: translateY(0);
    opacity: 1;
  }
}

@keyframes fadeIn {
  from {
    opacity: 0;
  }
  to {
    opacity: 1;
  }
}

@keyframes slideDown {
  from {
    transform: translateY(-100%);
    opacity: 0;
  }
  to {
    transform: translateY(0);
    opacity: 1;
  }
}

@keyframes pulseEmergency {
  0%, 100% {
    transform: scale(1);
    box-shadow: 0 0 0 0 rgba(239, 68, 68, 0.7);
  }
  50% {
    transform: scale(1.05);
    box-shadow: 0 0 0 10px rgba(239, 68, 68, 0);
  }
}

@keyframes shake {
  0%, 100% {
    transform: translateX(0);
  }
  25% {
    transform: translateX(-5px);
  }
  75% {
    transform: translateX(5px);
  }
}

@keyframes bounce {
  0%, 20%, 53%, 80%, 100% {
    transform: translate3d(0, 0, 0);
  }
  40%, 43% {
    transform: translate3d(0, -30px, 0);
  }
  70% {
    transform: translate3d(0, -15px, 0);
  }
  90% {
    transform: translate3d(0, -4px, 0);
  }
}

/* Animation classes */
.animate-slide-in-left {
  animation: slideInLeft 0.3s ease-out;
}

.animate-slide-in-up {
  animation: slideInUp 0.3s ease-out;
}

.animate-fade-in {
  animation: fadeIn 0.3s ease-out;
}

.animate-slide-down {
  animation: slideDown 0.3s ease-out;
}

.animate-pulse-emergency {
  animation: pulseEmergency 2s infinite;
}

.animate-shake {
  animation: shake 0.5s ease-in-out;
}

.animate-bounce {
  animation: bounce 1s ease-in-out;
}
```

---

## 📦 **PACKAGE.JSON SCRIPTS**

```json
{
  "scripts": {
    "dev": "vite",
    "build": "tsc && vite build",
    "preview": "vite preview",
    "test": "jest",
    "test:watch": "jest --watch",
    "test:coverage": "jest --coverage",
    "lint": "eslint src --ext ts,tsx --report-unused-disable-directives --max-warnings 0",
    "lint:fix": "eslint src --ext ts,tsx --fix",
    "format": "prettier --write \"src/**/*.{ts,tsx,js,jsx,json,css,md}\"",
    "type-check": "tsc --noEmit",
    "prepare": "husky install"
  }
}
```

---

## 🔧 **NEXT STEPS**

### **Immediate Actions:**
1. ✅ Run project initialization commands
2. ✅ Verify all dependencies installed
3. ✅ Test development server (`npm run dev`)
4. ✅ Verify TypeScript compilation
5. ✅ Test ESLint and Prettier

### **Phase 1 Week 1 Day 1-2 Checklist:**
- [ ] Project structure created
- [ ] All configuration files in place
- [ ] Design system CSS implemented
- [ ] Development environment working
- [ ] Git repository initialized
- [ ] Branch strategy defined

### **Ready for Next Phase:**
- Design System Implementation (Day 3-4)
- State Management Foundation (Day 5)

---

**Changelog:**
- v1.0: Initial project setup guide created

**Status:** Ready for implementation
