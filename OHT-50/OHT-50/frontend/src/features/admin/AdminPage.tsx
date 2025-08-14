import React from 'react'
import AppShell from '../../components/AppShell'

export default function AdminPage(){
  const [users, setUsers] = React.useState<{username:string; last:string}[]>([
    {username:'alice', last:'10:03'},
    {username:'bob', last:'09:55'},
    {username:'carol', last:'09:30'},
  ])
  const [q, setQ] = React.useState('')
  const filtered = users.filter(u => !q || u.username.toLowerCase().includes(q.toLowerCase()))
  return (
    <AppShell>
      <div className="space-y-6">
        <section className="card p-4">
          <div className="flex flex-wrap items-center gap-2">
            <input value={q} onChange={e=>setQ(e.target.value)} className="px-3 py-2 rounded border" style={{borderColor:'var(--input)'}} placeholder="Tìm theo tên đăng nhập" aria-label="Tìm người dùng" />
            <button className="btn btn-secondary ml-auto" type="button">Tạo tài khoản</button>
          </div>
        </section>
        <section className="card p-4">
          <div className="overflow-x-auto">
            <table>
              <thead>
                <tr><th className="py-2 pr-4">Tên đăng nhập</th><th className="py-2 pr-4">Lần đăng nhập cuối</th><th className="py-2">Thao tác</th></tr>
              </thead>
              <tbody>
                {filtered.map(u=> (
                  <tr key={u.username}>
                    <td className="py-2 pr-4">{u.username}</td>
                    <td className="py-2 pr-4">{u.last}</td>
                    <td className="py-2"><button className="btn btn-ghost">Đặt lại mật khẩu</button> <button className="btn btn-ghost">Xóa</button></td>
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


