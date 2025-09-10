export interface User {
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
