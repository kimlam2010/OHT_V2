import http from '@/lib/http'

interface LoginRequest {
  username: string
  password: string
}

interface User {
  id: number
  username: string
  email: string
  role: string
  is_active: boolean
}

interface LoginResponse {
  access_token: string
  token_type: string
  expires_in: number
  user: User
}
interface RegisterRequest {
  email: string
  username: string
  password: string
}

export const authApi = {
  login: (data: LoginRequest): Promise<LoginResponse> => {
    return http.post('/auth/login', data, {
      headers: {
        'x-api': 'software',
      },
    })
  },
  register: (data: RegisterRequest) => {
    return http.post('/auth/register', data, {
      headers: {
        'x-api': 'software',
      },
    })
  },
}
