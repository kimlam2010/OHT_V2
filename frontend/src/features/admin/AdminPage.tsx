import React from 'react'
import AppShell from '../../components/AppShell'
import { useToast } from '../../components/Toast'
import { apiFetchJson } from '../../services/http'

export default function AdminPage(){
  const { push } = useToast()
  const [users, setUsers] = React.useState<{username:string; role:string; last?:string}[]>([])
  const [q, setQ] = React.useState('')
  const [showCreate, setShowCreate] = React.useState(false)
  const [newUser, setNewUser] = React.useState({ username: '', password: '', role: 'operator' })
  const [busy, setBusy] = React.useState(false)

  const filtered = users.filter(u => !q || u.username.toLowerCase().includes(q.toLowerCase()))

  React.useEffect(()=>{
    (async()=>{
      const r = await apiFetchJson<any>('/api/v1/auth/users')
      if (r.ok){
        const data = r.data as any
        setUsers(Array.isArray(data) ? data : (data?.users || []))
      }
    })()
  },[])

  async function createUser(){
    if (!newUser.username || newUser.password.length < 4){
      push('Tên hoặc mật khẩu không hợp lệ')
      return
    }
    setBusy(true)
    try{
      const r = await apiFetchJson<any>('/api/v1/auth/users', {
        method: 'POST',
        body: JSON.stringify(newUser)
      })
      if (r.ok){
        push('Đã tạo tài khoản')
        setShowCreate(false)
        setNewUser({ username: '', password: '', role: 'operator' })
        const data = r.data as any
        setUsers(Array.isArray(data) ? data : (data?.users || []))
      } else {
        push('Tạo tài khoản thất bại')
      }
    } finally {
      setBusy(false)
    }
  }
  return (
    <AppShell>
      <div className="space-y-6">
        <section className="card p-4">
          <div className="flex flex-wrap items-center gap-2">
            <input value={q} onChange={e=>setQ(e.target.value)} className="px-3 py-2 rounded border" style={{borderColor:'var(--input)'}} placeholder="Tìm theo tên đăng nhập" aria-label="Tìm người dùng" />
            <button className="btn btn-secondary ml-auto" type="button" onClick={()=>setShowCreate(true)}>Tạo tài khoản</button>
          </div>
        </section>
        {showCreate && (
          <section className="card p-4">
            <h3 className="text-lg font-semibold mb-3">Tạo tài khoản</h3>
            <div className="grid grid-cols-1 sm:grid-cols-3 gap-3">
              <input className="px-3 py-2 rounded border" style={{borderColor:'var(--input)'}} placeholder="username" value={newUser.username} onChange={e=>setNewUser({...newUser, username:e.target.value})} />
              <input className="px-3 py-2 rounded border" style={{borderColor:'var(--input)'}} placeholder="password" type="password" value={newUser.password} onChange={e=>setNewUser({...newUser, password:e.target.value})} />
              <select className="px-3 py-2 rounded border" style={{borderColor:'var(--input)'}} value={newUser.role} onChange={e=>setNewUser({...newUser, role:e.target.value})}>
                <option value="operator">operator</option>
                <option value="maintainer">maintainer</option>
                <option value="admin">admin</option>
              </select>
            </div>
            <div className="mt-3 flex gap-2">
              <button className="btn btn-primary" disabled={busy} onClick={createUser}>{busy? 'Đang tạo...' : 'Tạo'}</button>
              <button className="btn" onClick={()=>setShowCreate(false)}>Hủy</button>
            </div>
          </section>
        )}
        <section className="card p-4">
          <div className="overflow-x-auto">
            <table>
              <thead>
                <tr><th className="py-2 pr-4">Tên đăng nhập</th><th className="py-2 pr-4">Vai trò</th><th className="py-2 pr-4">Lần đăng nhập cuối</th><th className="py-2">Thao tác</th></tr>
              </thead>
              <tbody>
                {filtered.map(u=> (
                  <tr key={u.username}>
                    <td className="py-2 pr-4">{u.username}</td>
                    <td className="py-2 pr-4">{u.role}</td>
                    <td className="py-2 pr-4">{u.last || '-'}</td>
                    <td className="py-2">
                      <button className="btn btn-ghost" onClick={async()=>{
                        const pwd = prompt(`Nhập mật khẩu mới cho ${u.username}`) || ''
                        if (!pwd) return
                        const r = await apiFetchJson(`/api/v1/auth/users/${u.username}/password`, { method:'POST', body: JSON.stringify({password: pwd}) })
                        push(r.ok? 'Đã đặt lại mật khẩu' : 'Đặt lại mật khẩu thất bại')
                      }}>Reset mật khẩu</button>
                      <button className="btn btn-ghost" onClick={async()=>{
                        if (!confirm(`Xóa tài khoản ${u.username}?`)) return
                        const r = await apiFetchJson<any>(`/api/v1/auth/users/${u.username}`, { method:'DELETE' })
                        if (r.ok){
                          const data = r.data as any
                          setUsers(Array.isArray(data) ? data : (data?.users || []))
                          push('Đã xóa tài khoản')
                        } else {
                          push('Xóa thất bại')
                        }
                      }}>Xóa</button>
                    </td>
                  </tr>
                ))}
              </tbody>
            </table>
          </div>
        </section>
      </div>
    </AppShell>
  )
}


