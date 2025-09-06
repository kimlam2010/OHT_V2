import type { InternalAxiosRequestConfig } from 'axios'
import axios, { isAxiosError } from 'axios'
import { ACCESS_TOKEN } from '@/constants/string'
import { NProgressCustom } from '@/plugins/nprogress'
import { HTTPError } from '@/types/error'

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

http.interceptors.response.use(
  (response) => {
    nprogress.done()
    return Promise.resolve(response.data)
  },
  (error) => {
    nprogress.done()

    if (isAxiosError(error) && error.response && error.response.data) {
      const { status } = error.response
      const { detail } = error.response.data

      return Promise.reject(new HTTPError(detail || 'Unexpected error', status))
    }

    return Promise.reject(error)
  },
)

export default http
