import React from 'react'
import { Link, useNavigate } from 'react-router-dom'
import { clearToken, getCurrentUser } from '../features/auth/auth'

export default function AppShell({ children }: { children: React.ReactNode }){
  const nav = useNavigate()
  const user = getCurrentUser() || 'operator'
  const [sidebarOpen, setSidebarOpen] = React.useState(false)

  function toggleTheme(){
    const cur = document.body.getAttribute('data-theme')
    document.body.setAttribute('data-theme', cur === 'dark' ? 'light' : 'dark')
  }
  return (
    <div className="min-h-screen">
      <header className="w-full sticky top-0 z-40 border-b" style={{borderColor: 'var(--border)'}}>
        <div className="max-w-screen-2xl mx-auto px-4 sm:px-6 lg:px-8 py-3 flex items-center justify-between">
          <div className="flex items-center gap-3">
            <button className="md:hidden btn btn-ghost" aria-label="Toggle menu" onClick={()=> setSidebarOpen(v=>!v)}>☰</button>
            <h1 className="text-xl font-semibold" style={{fontFamily: 'Merriweather, serif'}}>OHT-50 Dashboard</h1>
            <span className="badge" title="Kết nối Center">
              <span className="inline-block w-2 h-2 rounded-full" style={{background: 'color-mix(in oklab, var(--chart-4) 80%, black)'}}></span>
              Conn: Online
            </span>
          </div>
          <div className="flex items-center gap-3">
            <button className="btn btn-secondary" type="button">Mock: On</button>
            <button className="btn btn-ghost" type="button" title="Theme" onClick={toggleTheme}>Theme</button>
            <div className="text-sm">{user}</div>
            <button className="btn btn-ghost" onClick={()=>{ clearToken(); nav('/login') }}>Logout</button>
          </div>
        </div>
      </header>

      <div className="w-full max-w-screen-2xl mx-auto flex">
        <aside className={`${sidebarOpen ? '' : '-translate-x-full'} md:translate-x-0 transition-transform duration-300 ease-out fixed md:static top-[57px] left-0 md:top-auto md:left-auto z-30 md:z-auto w-64 min-h-[calc(100vh-57px)] sidebar`}>
          <nav className="p-4 space-y-1">
            <Link className="block px-3 py-2 rounded hover:bg-[var(--muted)]" to="/dashboard">Tổng quan</Link>
            <Link className="block px-3 py-2 rounded hover:bg-[var(--muted)]" to="/fe06">Đồ thị v‑a‑x</Link>
            <Link className="block px-3 py-2 rounded hover:bg-[var(--muted)]" to="/logs">Nhật ký</Link>
            <Link className="block px-3 py-2 rounded hover:bg-[var(--muted)]" to="/audit">Audit Trail</Link>
            <Link className="block px-3 py-2 rounded hover:bg-[var(--muted)]" to="/center">Cấu hình Center</Link>
            <Link className="block px-3 py-2 rounded hover:bg-[var(--muted)]" to="/safety">Cấu hình an toàn</Link>
            <Link className="block px-3 py-2 rounded hover:bg-[var(--muted)]" to="/jobs">Job Monitor</Link>
            <Link className="block px-3 py-2 rounded hover:bg-[var(--muted)]" to="/diag">Chẩn đoán</Link>
            <Link className="block px-3 py-2 rounded hover:bg-[var(--muted)]" to="/rs485">RS485 Addresses</Link>
            <Link className="block px-3 py-2 rounded hover:bg-[var(--muted)]" to="/user">User Settings</Link>
          </nav>
        </aside>
        <main className="flex-1 px-4 sm:px-6 lg:px-8 py-6">
          {children}
        </main>
      </div>
    </div>
  )
}


