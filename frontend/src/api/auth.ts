import http from '@/lib/http'

const _ROLES = ['operator', 'viewer', 'admin']
type Role = typeof _ROLES[number]

interface LoginRequest {
  username: string
  password: string
}

interface User {
  id: number
  username: string
  email: string
  full_name: string
  role: string
  status: string
  is_active: boolean
  created_at: string
  updated_at: string
  last_login: string | null
  permissions: object
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

export const authApi = {
  login: (data: LoginRequest): Promise<LoginResponse> => {
    return http.post('/auth/login', data, {
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
}
