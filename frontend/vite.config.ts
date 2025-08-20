import { defineConfig } from 'vite'
import { configDefaults } from 'vitest/config'
import react from '@vitejs/plugin-react'

// https://vitejs.dev/config/
export default defineConfig({
  plugins: [react()],
  test: {
    environment: 'jsdom',
    setupFiles: [],
    exclude: [...configDefaults.exclude],
  },
  server: {
    port: 5173,
    proxy: {
      // HTTP APIs → FastAPI backend
      '/api': {
        target: process.env.VITE_BACKEND_URL || 'http://localhost:8000',
        changeOrigin: true,
        secure: false,
        ws: true,
      },
      // WebSocket telemetry
      '/ws': {
        target: process.env.VITE_BACKEND_URL || 'http://localhost:8000',
        ws: true,
        changeOrigin: true,
        secure: false,
      }
    }
  }
})


