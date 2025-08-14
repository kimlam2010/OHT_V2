export type LoginResponse = { token: string }

export function getToken(): string | null {
  return localStorage.getItem('oht50_token')
}

export function setToken(token: string): void {
  localStorage.setItem('oht50_token', token)
}

export function clearToken(): void {
  localStorage.removeItem('oht50_token')
  localStorage.removeItem('oht50_user')
}

export function setCurrentUser(username: string){
  localStorage.setItem('oht50_user', username)
}
export function getCurrentUser(): string | null {
  return localStorage.getItem('oht50_user')
}

export async function login(username: string, password: string): Promise<string> {
  try {
    const res = await fetch('/api/v1/auth/login', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ username, password })
    })
    if (res.ok) {
      const data: LoginResponse = await res.json()
      setToken(data.token)
      setCurrentUser(username)
      return data.token
    }
  } catch {
    // ignore and fallback to mock below
  }
  // Fallback mock until backend auth is ready
  const mock = `mock-${Date.now()}`
  setToken(mock)
  setCurrentUser(username)
  return mock
}


