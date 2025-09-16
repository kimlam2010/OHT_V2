import type { User } from '@/types'
import http from '@/lib/http'

const _ROLES = ['operator', 'viewer', 'admin']
type Role = typeof _ROLES[number]

interface LoginRequest {
  username: string
  password: string
}

interface LogoutResponse {
  success: boolean
  message: string
}

interface LoginResponse {
  access_token: string
  refresh_token: string
  token_type: string
  expires_in: number
  user: User
}

interface RegisterRequest {
  username: string
  email: string
  full_name: string
  password: string
  confirm_password: string
  role: Role
}

interface RegisterResponse {
  success: boolean
  message: string
  user_id: number
  user: User
}

interface RefreshTokenRequest {
  refresh_token: string
}

interface RefreshTokenResponse {
  access_token: string
  token_type: string
  expires_in: number
}

export const authApi = {
  login: (data: LoginRequest): Promise<LoginResponse> => {
    return http.post('/auth/login', data, {
      headers: {
        'x-api': 'software',
      },
    })
  },
  logout: (): Promise<LogoutResponse> => {
    return http.post('/auth/logout', undefined, {
      headers: {
        'x-api': 'software',
      },
    })
  },
  register: (data: RegisterRequest): Promise<RegisterResponse> => {
    return http.post('/auth/register', data, {
      headers: {
        'x-api': 'software',
      },
    })
  },
  refreshToken: (data: RefreshTokenRequest): Promise<RefreshTokenResponse> => {
    return http.post('/auth/refresh', data, {
      headers: {
        'x-api': 'software',
      },
    })
  },
}
