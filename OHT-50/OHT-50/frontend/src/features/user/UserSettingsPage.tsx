import React from 'react'
import AppShell from '../../components/AppShell'

export default function UserSettingsPage(){
  function toggleDark(){
    const cur = document.body.getAttribute('data-theme')
    document.body.setAttribute('data-theme', cur==='dark'?'light':'dark')
  }
  return (
    <AppShell>
      <div className="space-y-6">
        <section className="grid grid-cols-1 lg:grid-cols-2 gap-6">
          <div className="card p-4">
            <h2 className="text-lg font-semibold mb-3">Hồ sơ</h2>
            <div className="flex items-center gap-3">
              <img src="https://images.unsplash.com/photo-1544005313-94ddf0286df2?w=200&h=200&fit=crop" alt="avatar" className="w-16 h-16 rounded-full border" style={{borderColor:'var(--border)'}}/>
              <div>
                <div className="font-semibold">operator</div>
                <div className="muted text-sm">operator@example.com</div>
              </div>
            </div>
            <div className="mt-4 grid gap-3">
              <label className="text-sm">Tên hiển thị<input className="w-full px-3 py-2 rounded border" style={{borderColor:'var(--input)'}} defaultValue="Operator"/></label>
              <label className="text-sm">Email<input className="w-full px-3 py-2 rounded border" style={{borderColor:'var(--input)'}} defaultValue="operator@example.com"/></label>
              <div className="flex justify-end"><button className="btn btn-primary" type="button">Lưu</button></div>
            </div>
          </div>

          <div className="card p-4">
            <h2 className="text-lg font-semibold mb-3">Bảo mật</h2>
            <div className="flex flex-wrap gap-2">
              <button className="btn btn-ghost" type="button">Đổi mật khẩu</button>
              <button className="btn btn-ghost" type="button" onClick={toggleDark}>Bật/Tắt Dark mode</button>
            </div>
          </div>
        </section>

        <section className="card p-4" id="adminCard">
          <h2 className="text-lg font-semibold mb-3">Quản trị</h2>
          <p className="muted text-sm mb-3">Hiển thị với Admin.</p>
          <a href="/admin" className="btn btn-secondary">Quản trị người dùng</a>
        </section>
      </div>
    </AppShell>
  )
}


