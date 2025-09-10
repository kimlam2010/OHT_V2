import type { InternalAxiosRequestConfig } from 'axios'
import axios, { isAxiosError } from 'axios'
import { authApi } from '@/api/auth'
import { ACCESS_TOKEN, REFRESH_TOKEN } from '@/constants/string'
import { NProgressCustom } from '@/plugins/nprogress'
import { useUserStore } from '@/stores'
import { HTTPError } from '@/types'

const nprogress = NProgressCustom()

const http = axios.create({
  timeout: 20000,
  headers: { 'Content-Type': 'application/json' },
  withCredentials: true,
})

http.interceptors.request.use(
  (config: InternalAxiosRequestConfig) => {
    const accessToken = localStorage.getItem(ACCESS_TOKEN)
    if (accessToken) {
      config.headers.Authorization = `Bearer ${accessToken}`
    }
    if (config.headers['x-api'] === 'software') {
      config.baseURL = `${import.meta.env.VITE_BACKEND_URL}/api/v1`
    }
    else {
      config.baseURL = `${import.meta.env.VITE_FIRMWARE_URL}/api/v1`
    }
    if (!config.doNotShowLoading) {
      nprogress.start()
    }
    return config
  },
  (err) => {
    return Promise.reject(err)
  },
)

let isRefreshing = false

interface QueueItem {
  resolve: (value?: unknown) => void
  reject: (error?: unknown) => void
}

let failedQueue: QueueItem[] = []

function processQueue(error: unknown, token: string | null = null) {
  failedQueue.forEach((prom) => {
    if (error) {
      prom.reject(error)
    }
    else {
      prom.resolve(token)
    }
  })
  failedQueue = []
}

http.interceptors.response.use(
  (response) => {
    nprogress.done()
    return Promise.resolve(response.data)
  },
  async (error) => {
    nprogress.done()
    const originalRequest = error.config

    if (error.response?.status === 401 && !originalRequest._retry) {
      if (isRefreshing) {
        return new Promise((resolve, reject) => {
          failedQueue.push({ resolve, reject })
        })
          .then((token) => {
            originalRequest.headers.Authorization = `Bearer ${token}`
            return http(originalRequest)
          })
          .catch(err => Promise.reject(err))
      }

      originalRequest._retry = true
      isRefreshing = true

      const refreshToken = localStorage.getItem(REFRESH_TOKEN)
      if (!refreshToken) {
        useUserStore.getState().clearUser()
        localStorage.removeItem(ACCESS_TOKEN)
        localStorage.removeItem(REFRESH_TOKEN)
        window.location.href = '/login'
        return Promise.reject(error)
      }

      try {
        const response = await authApi.refreshToken({ refresh_token: refreshToken })
        const { access_token } = response
        localStorage.setItem(ACCESS_TOKEN, access_token)
        http.defaults.headers.common.Authorization = `Bearer ${access_token}`
        originalRequest.headers.Authorization = `Bearer ${access_token}`
        processQueue(null, access_token)
        return http(originalRequest)
      }
      catch (err) {
        processQueue(err, null)
        useUserStore.getState().clearUser()
        localStorage.removeItem(ACCESS_TOKEN)
        localStorage.removeItem(REFRESH_TOKEN)
        window.location.href = '/login'
        return Promise.reject(err)
      }
      finally {
        isRefreshing = false
      }
    }

    if (isAxiosError(error) && error.response && error.response.data) {
      const { status } = error.response
      const { detail } = error.response.data

      return Promise.reject(new HTTPError(detail || 'Unexpected error', status))
    }

    return Promise.reject(error)
  },
)

export default http
