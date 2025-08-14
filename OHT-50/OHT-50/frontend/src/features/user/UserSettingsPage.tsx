import React from 'react'
import AppShell from '../../components/AppShell'
import { getCurrentUser } from '../auth/auth'
import { getUserSettings, updateUserSettings, type UserSettings } from '../../services/user'
import { useToast } from '../../components/Toast'

export default function UserSettingsPage(){
  const username = getCurrentUser() || 'operator'
  const { push } = useToast()
  const [settings, setSettings] = React.useState<UserSettings | null>(null)
  const [displayName, setDisplayName] = React.useState('')
  const [email, setEmail] = React.useState('')
  const [theme, setTheme] = React.useState<'light'|'dark'>('light')
  const [isLoading, setIsLoading] = React.useState(true)
  const [isSaving, setIsSaving] = React.useState(false)

  React.useEffect(()=>{
    (async()=>{
      setIsLoading(true)
      try{
        const s = await getUserSettings(username)
        if (s){
          setSettings(s)
          setDisplayName(s.display_name || '')
          setEmail(s.email || '')
          setTheme((s.theme as any) || 'light')
          document.body.setAttribute('data-theme', (s.theme as any) || 'light')
        } else {
          push('Không đọc được User Settings từ API')
        }
      } catch {
        push('Lỗi kết nối API User Settings')
      } finally {
        setIsLoading(false)
      }
    })()
  },[username])

  function toggleDark(){
    const newTheme = (theme === 'dark') ? 'light' : 'dark'
    setTheme(newTheme)
    document.body.setAttribute('data-theme', newTheme)
  }

  async function onSave(){
    if (!email || !/^[^\s@]+@[^\s@]+\.[^\s@]+$/.test(email)){
      push('Email không hợp lệ')
      return
    }
    setIsSaving(true)
    try{
      const updated = await updateUserSettings(username, {
        username,
        display_name: displayName,
        email,
        theme
      })
      if (updated){
        setSettings(updated)
        push('Đã lưu User Settings')
      } else {
        push('Lưu thất bại (API trả về lỗi)')
      }
    } catch {
      push('Lưu thất bại (lỗi kết nối API)')
    } finally {
      setIsSaving(false)
    }
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
                <div className="font-semibold">{username}</div>
                <div className="muted text-sm">{email || `${username}@example.com`}</div>
              </div>
            </div>
            <div className="mt-4 grid gap-3">
              <label className="text-sm">Tên hiển thị<input disabled={isLoading||isSaving} className="w-full px-3 py-2 rounded border" style={{borderColor:'var(--input)'}} value={displayName} onChange={e=>setDisplayName(e.target.value)} /></label>
              <label className="text-sm">Email<input disabled={isLoading||isSaving} className="w-full px-3 py-2 rounded border" style={{borderColor:'var(--input)'}} value={email} onChange={e=>setEmail(e.target.value)} /></label>
              <div className="flex items-center gap-3">
                <label className="text-sm flex items-center gap-2">Theme
                  <select disabled={isLoading||isSaving} className="px-3 py-2 rounded border" style={{borderColor:'var(--input)'}} value={theme} onChange={e=> setTheme(e.target.value as 'light'|'dark')}>
                    <option value="light">Light</option>
                    <option value="dark">Dark</option>
                  </select>
                </label>
                <button className="btn btn-ghost" type="button" onClick={toggleDark} disabled={isLoading||isSaving}>Xem nhanh</button>
                <div className="flex-1" />
                <button className="btn btn-primary" type="button" onClick={onSave} disabled={isLoading||isSaving}>{isSaving? 'Đang lưu...' : 'Lưu'}</button>
              </div>
            </div>
          </div>

          <div className="card p-4">
            <h2 className="text-lg font-semibold mb-3">Bảo mật</h2>
            <div className="flex flex-wrap gap-2">
              <button className="btn btn-ghost" type="button" disabled={isLoading||isSaving}>Đổi mật khẩu</button>
              <button className="btn btn-ghost" type="button" onClick={toggleDark} disabled={isLoading||isSaving}>Bật/Tắt Dark mode</button>
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


