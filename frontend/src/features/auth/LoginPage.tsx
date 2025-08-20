import React from 'react'
import { useNavigate } from 'react-router-dom'
import { login } from './auth'

export default function LoginPage() {
  const nav = useNavigate()
  const [username, setUsername] = React.useState('operator')
  const [password, setPassword] = React.useState('')
  const [loading, setLoading] = React.useState(false)
  const [error, setError] = React.useState('')

  async function onSubmit(e: React.FormEvent){
    e.preventDefault()
    setLoading(true); setError('')
    try {
      await login(username, password)
      nav('/dashboard')
    } catch (e: any) {
      setError(e.message || 'Login failed')
    } finally { setLoading(false) }
  }

  return (
    <div className="min-h-screen flex items-center justify-center p-6">
      <form onSubmit={onSubmit} className="card p-6 w-full max-w-sm">
        <h1 className="text-xl font-semibold mb-4">Đăng nhập</h1>
        {error && <div className="text-red-600 text-sm mb-2">{error}</div>}
        <label className="text-sm mb-2 block">Tên đăng nhập
          <input className="w-full px-3 py-2 rounded border mt-1" style={{borderColor:'var(--input)'}} value={username} onChange={e=>setUsername(e.target.value)} />
        </label>
        <label className="text-sm mb-4 block">Mật khẩu
          <input type="password" className="w-full px-3 py-2 rounded border mt-1" style={{borderColor:'var(--input)'}} value={password} onChange={e=>setPassword(e.target.value)} />
        </label>
        <button type="submit" className="btn btn-primary w-full" disabled={loading}>{loading? 'Đang đăng nhập...' : 'Đăng nhập'}</button>
      </form>
    </div>
  )
}


