import http from '@/lib/http'

interface LoginRequest {
  username: string
  password: string
}

interface RegisterRequest {
  email: string
  username: string
  password: string
}

export const authApi = {
  login: (data: LoginRequest) => {
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
