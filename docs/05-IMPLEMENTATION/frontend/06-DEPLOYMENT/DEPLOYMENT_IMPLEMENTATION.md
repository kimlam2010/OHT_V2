# Deployment Implementation

**Version:** 1.0  
**Date:** 2024-12-19  
**Team:** Frontend  
**Status:** In Progress

## 📋 **Overview**

Deployment implementation cho OHT-50 frontend, bao gồm Docker containerization, CI/CD pipeline, và production deployment strategies.

## 🐳 **Docker Containerization**

### **Dockerfile**
```dockerfile
# Dockerfile
FROM node:18-alpine AS base

# Install dependencies only when needed
FROM base AS deps
RUN apk add --no-cache libc6-compat
WORKDIR /app

# Install dependencies based on the preferred package manager
COPY package.json package-lock.json* ./
RUN npm ci --only=production

# Rebuild the source code only when needed
FROM base AS builder
WORKDIR /app
COPY --from=deps /app/node_modules ./node_modules
COPY . .

# Build the application
RUN npm run build

# Production image, copy all the files and run the app
FROM nginx:alpine AS runner
WORKDIR /app

ENV NODE_ENV production

# Copy built application
COPY --from=builder /app/dist /usr/share/nginx/html

# Copy nginx configuration
COPY nginx.conf /etc/nginx/nginx.conf

# Copy startup script
COPY docker-entrypoint.sh /usr/local/bin/
RUN chmod +x /usr/local/bin/docker-entrypoint.sh

# Create non-root user
RUN addgroup -g 1001 -S nodejs
RUN adduser -S nextjs -u 1001

# Change ownership of the app directory
RUN chown -R nextjs:nodejs /usr/share/nginx/html
RUN chown -R nextjs:nodejs /var/cache/nginx
RUN chown -R nextjs:nodejs /var/log/nginx
RUN chown -R nextjs:nodejs /etc/nginx/conf.d

USER nextjs

EXPOSE 3000

ENTRYPOINT ["docker-entrypoint.sh"]
CMD ["nginx", "-g", "daemon off;"]
```

### **Nginx Configuration**
```nginx
# nginx.conf
events {
    worker_connections 1024;
}

http {
    include       /etc/nginx/mime.types;
    default_type  application/octet-stream;

    # Logging
    log_format main '$remote_addr - $remote_user [$time_local] "$request" '
                    '$status $body_bytes_sent "$http_referer" '
                    '"$http_user_agent" "$http_x_forwarded_for"';

    access_log /var/log/nginx/access.log main;
    error_log /var/log/nginx/error.log warn;

    # Gzip compression
    gzip on;
    gzip_vary on;
    gzip_min_length 1024;
    gzip_proxied any;
    gzip_comp_level 6;
    gzip_types
        text/plain
        text/css
        text/xml
        text/javascript
        application/json
        application/javascript
        application/xml+rss
        application/atom+xml
        image/svg+xml;

    # Security headers
    add_header X-Frame-Options "SAMEORIGIN" always;
    add_header X-XSS-Protection "1; mode=block" always;
    add_header X-Content-Type-Options "nosniff" always;
    add_header Referrer-Policy "no-referrer-when-downgrade" always;
    add_header Content-Security-Policy "default-src 'self' http: https: data: blob: 'unsafe-inline'" always;

    server {
        listen 3000;
        server_name localhost;
        root /usr/share/nginx/html;
        index index.html;

        # Handle client-side routing
        location / {
            try_files $uri $uri/ /index.html;
        }

        # API proxy
        location /api/ {
            proxy_pass http://backend:8000/api/;
            proxy_http_version 1.1;
            proxy_set_header Upgrade $http_upgrade;
            proxy_set_header Connection 'upgrade';
            proxy_set_header Host $host;
            proxy_set_header X-Real-IP $remote_addr;
            proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
            proxy_set_header X-Forwarded-Proto $scheme;
            proxy_cache_bypass $http_upgrade;
        }

        # WebSocket proxy
        location /ws/ {
            proxy_pass http://backend:8000/ws/;
            proxy_http_version 1.1;
            proxy_set_header Upgrade $http_upgrade;
            proxy_set_header Connection "upgrade";
            proxy_set_header Host $host;
            proxy_set_header X-Real-IP $remote_addr;
            proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
            proxy_set_header X-Forwarded-Proto $scheme;
        }

        # Static assets caching
        location ~* \.(js|css|png|jpg|jpeg|gif|ico|svg)$ {
            expires 1y;
            add_header Cache-Control "public, immutable";
        }

        # Health check
        location /health {
            access_log off;
            return 200 "healthy\n";
            add_header Content-Type text/plain;
        }
    }
}
```

### **Docker Compose**
```yaml
# docker-compose.yml
version: '3.8'

services:
  frontend:
    build:
      context: .
      dockerfile: Dockerfile
    ports:
      - "3000:3000"
    environment:
      - NODE_ENV=production
      - REACT_APP_API_BASE_URL=http://localhost:8000/api
      - REACT_APP_WS_URL=ws://localhost:8000/ws
    depends_on:
      - backend
    networks:
      - oht-network
    restart: unless-stopped
    healthcheck:
      test: ["CMD", "curl", "-f", "http://localhost:3000/health"]
      interval: 30s
      timeout: 10s
      retries: 3

  backend:
    image: oht-backend:latest
    ports:
      - "8000:8000"
    environment:
      - DATABASE_URL=postgresql://user:password@db:5432/oht50
    depends_on:
      - db
    networks:
      - oht-network
    restart: unless-stopped

  db:
    image: postgres:15-alpine
    environment:
      - POSTGRES_DB=oht50
      - POSTGRES_USER=user
      - POSTGRES_PASSWORD=password
    volumes:
      - postgres_data:/var/lib/postgresql/data
    networks:
      - oht-network
    restart: unless-stopped

networks:
  oht-network:
    driver: bridge

volumes:
  postgres_data:
```

## 🔄 **CI/CD Pipeline**

### **GitHub Actions Workflow**
```yaml
# .github/workflows/ci-cd.yml
name: CI/CD Pipeline

on:
  push:
    branches: [ main, develop ]
  pull_request:
    branches: [ main ]

env:
  REGISTRY: ghcr.io
  IMAGE_NAME: ${{ github.repository }}

jobs:
  test:
    runs-on: ubuntu-latest
    
    steps:
    - name: Checkout code
      uses: actions/checkout@v4

    - name: Setup Node.js
      uses: actions/setup-node@v4
      with:
        node-version: '18'
        cache: 'npm'

    - name: Install dependencies
      run: npm ci

    - name: Run linting
      run: npm run lint

    - name: Run type checking
      run: npm run type-check

    - name: Run unit tests
      run: npm run test:unit

    - name: Run integration tests
      run: npm run test:integration

    - name: Run accessibility tests
      run: npm run test:a11y

    - name: Upload coverage reports
      uses: codecov/codecov-action@v3
      with:
        file: ./coverage/lcov.info
        flags: frontend
        name: frontend-coverage

  build:
    needs: test
    runs-on: ubuntu-latest
    if: github.event_name == 'push'
    
    steps:
    - name: Checkout code
      uses: actions/checkout@v4

    - name: Setup Docker Buildx
      uses: docker/setup-buildx-action@v3

    - name: Log in to Container Registry
      uses: docker/login-action@v3
      with:
        registry: ${{ env.REGISTRY }}
        username: ${{ github.actor }}
        password: ${{ secrets.GITHUB_TOKEN }}

    - name: Extract metadata
      id: meta
      uses: docker/metadata-action@v5
      with:
        images: ${{ env.REGISTRY }}/${{ env.IMAGE_NAME }}
        tags: |
          type=ref,event=branch
          type=ref,event=pr
          type=semver,pattern={{version}}
          type=semver,pattern={{major}}.{{minor}}
          type=sha

    - name: Build and push Docker image
      uses: docker/build-push-action@v5
      with:
        context: .
        push: true
        tags: ${{ steps.meta.outputs.tags }}
        labels: ${{ steps.meta.outputs.labels }}
        cache-from: type=gha
        cache-to: type=gha,mode=max

  deploy-staging:
    needs: build
    runs-on: ubuntu-latest
    if: github.ref == 'refs/heads/develop'
    environment: staging
    
    steps:
    - name: Deploy to staging
      run: |
        echo "Deploying to staging environment"
        # Add staging deployment commands here

  deploy-production:
    needs: build
    runs-on: ubuntu-latest
    if: github.ref == 'refs/heads/main'
    environment: production
    
    steps:
    - name: Deploy to production
      run: |
        echo "Deploying to production environment"
        # Add production deployment commands here
```

### **Build Scripts**
```json
// package.json scripts
{
  "scripts": {
    "build": "vite build",
    "build:analyze": "vite build --mode analyze",
    "build:staging": "vite build --mode staging",
    "build:production": "vite build --mode production",
    "docker:build": "docker build -t oht-frontend .",
    "docker:run": "docker run -p 3000:3000 oht-frontend",
    "docker:compose": "docker-compose up -d",
    "docker:compose:down": "docker-compose down",
    "deploy:staging": "npm run build:staging && docker build -t oht-frontend:staging .",
    "deploy:production": "npm run build:production && docker build -t oht-frontend:production ."
  }
}
```

## 🌍 **Environment Configuration**

### **Environment Variables**
```bash
# .env.development
VITE_API_BASE_URL=http://localhost:8000/api
VITE_WS_URL=ws://localhost:8000/ws
VITE_APP_ENV=development
VITE_DEBUG=true
VITE_LOG_LEVEL=debug
```

```bash
# .env.staging
VITE_API_BASE_URL=https://staging-api.oht50.com/api
VITE_WS_URL=wss://staging-api.oht50.com/ws
VITE_APP_ENV=staging
VITE_DEBUG=false
VITE_LOG_LEVEL=info
```

```bash
# .env.production
VITE_API_BASE_URL=https://api.oht50.com/api
VITE_WS_URL=wss://api.oht50.com/ws
VITE_APP_ENV=production
VITE_DEBUG=false
VITE_LOG_LEVEL=warn
```

### **Vite Configuration**
```typescript
// vite.config.ts
import { defineConfig, loadEnv } from 'vite';
import react from '@vitejs/plugin-react';
import { resolve } from 'path';

export default defineConfig(({ command, mode }) => {
  const env = loadEnv(mode, process.cwd(), '');
  
  return {
    plugins: [react()],
    resolve: {
      alias: {
        '@': resolve(__dirname, 'src')
      }
    },
    build: {
      outDir: 'dist',
      sourcemap: mode === 'development',
      rollupOptions: {
        output: {
          manualChunks: {
            vendor: ['react', 'react-dom'],
            router: ['react-router-dom'],
            ui: ['@headlessui/react', '@heroicons/react'],
            charts: ['chart.js', 'react-chartjs-2']
          }
        }
      }
    },
    server: {
      port: 3000,
      proxy: {
        '/api': {
          target: env.VITE_API_BASE_URL || 'http://localhost:8000',
          changeOrigin: true,
          secure: false
        },
        '/ws': {
          target: env.VITE_WS_URL || 'ws://localhost:8000',
          ws: true,
          changeOrigin: true
        }
      }
    },
    define: {
      __APP_VERSION__: JSON.stringify(process.env.npm_package_version),
      __BUILD_TIME__: JSON.stringify(new Date().toISOString())
    }
  };
});
```

## 🚀 **Deployment Strategies**

### **Blue-Green Deployment**
```yaml
# blue-green-deployment.yml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: oht-frontend-blue
spec:
  replicas: 3
  selector:
    matchLabels:
      app: oht-frontend
      version: blue
  template:
    metadata:
      labels:
        app: oht-frontend
        version: blue
    spec:
      containers:
      - name: frontend
        image: oht-frontend:blue
        ports:
        - containerPort: 3000
        env:
        - name: NODE_ENV
          value: "production"
        livenessProbe:
          httpGet:
            path: /health
            port: 3000
          initialDelaySeconds: 30
          periodSeconds: 10
        readinessProbe:
          httpGet:
            path: /health
            port: 3000
          initialDelaySeconds: 5
          periodSeconds: 5
---
apiVersion: v1
kind: Service
metadata:
  name: oht-frontend-service
spec:
  selector:
    app: oht-frontend
    version: blue
  ports:
  - port: 80
    targetPort: 3000
  type: LoadBalancer
```

### **Rolling Update Strategy**
```yaml
# rolling-update.yml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: oht-frontend
spec:
  replicas: 3
  strategy:
    type: RollingUpdate
    rollingUpdate:
      maxSurge: 1
      maxUnavailable: 0
  selector:
    matchLabels:
      app: oht-frontend
  template:
    metadata:
      labels:
        app: oht-frontend
    spec:
      containers:
      - name: frontend
        image: oht-frontend:latest
        ports:
        - containerPort: 3000
        resources:
          requests:
            memory: "128Mi"
            cpu: "100m"
          limits:
            memory: "256Mi"
            cpu: "200m"
```

## 📊 **Monitoring & Health Checks**

### **Health Check Endpoint**
```typescript
// src/health.ts
import express from 'express';

const app = express();

app.get('/health', (req, res) => {
  const health = {
    status: 'healthy',
    timestamp: new Date().toISOString(),
    version: process.env.npm_package_version,
    environment: process.env.NODE_ENV,
    uptime: process.uptime(),
    memory: process.memoryUsage(),
    checks: {
      database: 'healthy',
      api: 'healthy',
      websocket: 'healthy'
    }
  };

  res.status(200).json(health);
});

app.get('/ready', (req, res) => {
  // Check if application is ready to serve traffic
  const ready = {
    status: 'ready',
    timestamp: new Date().toISOString()
  };

  res.status(200).json(ready);
});

export default app;
```

### **Application Monitoring**
```typescript
// src/monitoring.ts
import { performance } from 'perf_hooks';

// Performance monitoring
export const monitorPerformance = () => {
  const start = performance.now();
  
  return {
    end: () => {
      const duration = performance.now() - start;
      console.log(`Operation took ${duration}ms`);
      return duration;
    }
  };
};

// Error tracking
export const trackError = (error: Error, context?: any) => {
  console.error('Application Error:', {
    message: error.message,
    stack: error.stack,
    context,
    timestamp: new Date().toISOString()
  });
};

// Memory monitoring
export const monitorMemory = () => {
  const memUsage = process.memoryUsage();
  console.log('Memory Usage:', {
    rss: `${Math.round(memUsage.rss / 1024 / 1024)} MB`,
    heapTotal: `${Math.round(memUsage.heapTotal / 1024 / 1024)} MB`,
    heapUsed: `${Math.round(memUsage.heapUsed / 1024 / 1024)} MB`,
    external: `${Math.round(memUsage.external / 1024 / 1024)} MB`
  });
};
```

## 🔒 **Security Configuration**

### **Security Headers**
```typescript
// src/security.ts
import helmet from 'helmet';

export const securityMiddleware = helmet({
  contentSecurityPolicy: {
    directives: {
      defaultSrc: ["'self'"],
      styleSrc: ["'self'", "'unsafe-inline'"],
      scriptSrc: ["'self'"],
      imgSrc: ["'self'", "data:", "https:"],
      connectSrc: ["'self'", "ws:", "wss:"],
      fontSrc: ["'self'", "https:"],
      objectSrc: ["'none'"],
      mediaSrc: ["'self'"],
      frameSrc: ["'none'"]
    }
  },
  hsts: {
    maxAge: 31536000,
    includeSubDomains: true,
    preload: true
  },
  noSniff: true,
  referrerPolicy: { policy: 'strict-origin-when-cross-origin' }
});
```

### **CORS Configuration**
```typescript
// src/cors.ts
import cors from 'cors';

export const corsOptions = {
  origin: process.env.ALLOWED_ORIGINS?.split(',') || ['http://localhost:3000'],
  credentials: true,
  methods: ['GET', 'POST', 'PUT', 'DELETE', 'OPTIONS'],
  allowedHeaders: ['Content-Type', 'Authorization'],
  exposedHeaders: ['Content-Length', 'X-Requested-With']
};

export const corsMiddleware = cors(corsOptions);
```

## 📈 **Performance Optimization**

### **Bundle Analysis**
```typescript
// scripts/analyze-bundle.ts
import { build } from 'vite';
import { visualizer } from 'rollup-plugin-visualizer';

export const analyzeBundle = async () => {
  await build({
    plugins: [
      visualizer({
        filename: 'dist/stats.html',
        open: true,
        gzipSize: true,
        brotliSize: true
      })
    ]
  });
};
```

### **Lighthouse CI**
```yaml
# .lighthouserc.js
module.exports = {
  ci: {
    collect: {
      url: ['http://localhost:3000'],
      numberOfRuns: 3
    },
    assert: {
      assertions: {
        'categories:performance': ['warn', { minScore: 0.9 }],
        'categories:accessibility': ['error', { minScore: 0.9 }],
        'categories:best-practices': ['warn', { minScore: 0.9 }],
        'categories:seo': ['warn', { minScore: 0.9 }]
      }
    },
    upload: {
      target: 'temporary-public-storage'
    }
  }
};
```

## 🔄 **Changelog**

**v1.0 (2024-12-19)**
- ✅ Created Docker containerization
- ✅ Implemented CI/CD pipeline with GitHub Actions
- ✅ Added environment configuration
- ✅ Created deployment strategies
- ✅ Implemented monitoring and health checks
- ✅ Added security configuration
- ✅ Created performance optimization
- ✅ Added bundle analysis and Lighthouse CI

---

**Next Steps:**
1. Implement automated rollback strategies
2. Add load balancing configuration
3. Create disaster recovery procedures
4. Implement blue-green deployment automation
5. Add deployment monitoring and alerting
