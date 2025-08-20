import React from 'react'
import AppShell from '../../components/AppShell'
import { useToast } from '../../components/Toast'
import {
  getConfig,
  updateConfig,
  validateConfig,
  getConfigHistory,
  rollbackConfig,
  exportConfig,
  importConfig,
  SystemConfig,
  ConfigHistoryItem,
  ConfigResponse,
} from '../../services/config'
import { getCurrentUser } from '../auth/auth'

type TabType = 'current' | 'history' | 'import'

export default function ConfigPage(){
  const { push } = useToast()
  const [activeTab, setActiveTab] = React.useState<TabType>('current')
  const [loading, setLoading] = React.useState(true)
  const [saving, setSaving] = React.useState(false)
  const [validating, setValidating] = React.useState(false)
  const [historyLoading, setHistoryLoading] = React.useState(false)
  const [history, setHistory] = React.useState<ConfigHistoryItem[] | null>(null)
  const [cfg, setCfg] = React.useState<SystemConfig | null>(null)
  const [cfgText, setCfgText] = React.useState('')
  const [validateMsg, setValidateMsg] = React.useState<string>('')
  const [error, setError] = React.useState<string>('')
  const [importFormat, setImportFormat] = React.useState<'json'|'yaml'>('json')
  const [importText, setImportText] = React.useState('')
  const [exporting, setExporting] = React.useState(false)

  React.useEffect(()=>{
    (async ()=>{
      setLoading(true)
      try{
        const c = await getConfig()
        setCfg(c)
        setCfgText(JSON.stringify(c ?? {}, null, 2))
      } catch (e) {
        setError((e as Error)?.message || 'Lỗi tải cấu hình')
      } finally { setLoading(false) }
    })()
  }, [])

  async function loadHistory(){
    setHistoryLoading(true)
    const h = await getConfigHistory()
    setHistory(h)
    setHistoryLoading(false)
  }

  function parseCfgFromText(): SystemConfig | null {
    try{
      const obj = cfgText.trim() ? JSON.parse(cfgText) : {}
      return obj as SystemConfig
    } catch (e) {
      setError('JSON không hợp lệ');
      push('JSON không hợp lệ');
      return null
    }
  }

  async function onValidate(){
    const obj = parseCfgFromText(); if (!obj) return
    setValidating(true)
    const r = await validateConfig(obj)
    setValidating(false)
    if (!r){ setValidateMsg('Validate thất bại'); push('Validate thất bại'); return }
    setValidateMsg(r.success ? 'Validate PASS' : `Validate FAIL: ${r.message || ''}`)
    push(r.success ? 'Validate PASS' : 'Validate FAIL')
  }

  async function onSave(){
    const obj = parseCfgFromText(); if (!obj) return
    const user = getCurrentUser() || 'system'
    setSaving(true)
    const ok = await updateConfig(obj, user)
    setSaving(false)
    if (ok){
      push('Đã cập nhật cấu hình')
      setCfg(obj)
    } else {
      push('Cập nhật cấu hình thất bại')
    }
  }

  async function onRollback(v: number){
    const ok = window.confirm(`Rollback về version ${v}?`)
    if (!ok) return
    const r = await rollbackConfig(v)
    if (r?.success){
      push(`Rollback thành công về version ${r.version ?? v}`)
      if (r.config){
        setCfg(r.config)
        setCfgText(JSON.stringify(r.config, null, 2))
      }
      loadHistory()
    } else {
      push(`Rollback thất bại: ${r?.message || ''}`)
    }
  }

  async function onExport(format: 'json'|'yaml'){
    setExporting(true)
    const blob = await exportConfig(format)
    setExporting(false)
    if (!blob){ push('Export thất bại'); return }
    const a = document.createElement('a')
    a.href = URL.createObjectURL(blob)
    a.download = `config.${format}`
    document.body.appendChild(a); a.click(); document.body.removeChild(a)
    setTimeout(()=> URL.revokeObjectURL(a.href), 0)
  }

  async function onImport(){
    const user = getCurrentUser() || 'system'
    const confirm = window.confirm('Import sẽ GHI ĐÈ lên cấu hình hiện tại. Tiếp tục?')
    if (!confirm) return
    const r: ConfigResponse | null = await importConfig(importText, importFormat, user)
    if (r?.success){
      push('Import thành công')
      if (r.config){ setCfg(r.config); setCfgText(JSON.stringify(r.config, null, 2)) }
    } else {
      push(`Import thất bại: ${r?.message || ''}`)
    }
  }

  return (
    <AppShell>
      <section>
        <div className="card p-4 border mb-6" style={{borderColor:'var(--border)'}}>
          <div className="flex items-center justify-between mb-4">
            <h2 className="text-lg font-semibold">Cấu hình hệ thống</h2>
            <div className="text-sm muted">
              {loading ? 'Đang tải…' : ''} 
              {error ? `Lỗi: ${error}` : ''}
              {cfg?.version ? `Version: ${cfg.version}` : ''}
            </div>
          </div>

          {/* Tab Navigation */}
          <div className="flex border-b" style={{borderColor:'var(--border)'}}>
            <button 
              className={`px-4 py-2 text-sm font-medium ${activeTab === 'current' ? 'border-b-2 text-primary' : 'text-muted-foreground'}`}
              style={{borderColor: activeTab === 'current' ? 'var(--primary)' : 'transparent'}}
              onClick={() => setActiveTab('current')}
            >
              Cấu hình hiện tại
            </button>
            <button 
              className={`px-4 py-2 text-sm font-medium ${activeTab === 'history' ? 'border-b-2 text-primary' : 'text-muted-foreground'}`}
              style={{borderColor: activeTab === 'history' ? 'var(--primary)' : 'transparent'}}
              onClick={() => {
                setActiveTab('history')
                if (!history) loadHistory()
              }}
            >
              Lịch sử
            </button>
            <button 
              className={`px-4 py-2 text-sm font-medium ${activeTab === 'import' ? 'border-b-2 text-primary' : 'text-muted-foreground'}`}
              style={{borderColor: activeTab === 'import' ? 'var(--primary)' : 'transparent'}}
              onClick={() => setActiveTab('import')}
            >
              Import/Export
            </button>
          </div>

          {/* Tab Content */}
          <div className="mt-4">
            {/* Tab 1: Current Config */}
            {activeTab === 'current' && (
              <div>
                <div className="flex items-center gap-2 mb-3">
                  <button className="btn" onClick={onValidate} disabled={validating}>
                    {validating ? 'Validating…' : 'Validate'}
                  </button>
                  <button className="btn btn-primary" onClick={onSave} disabled={saving}>
                    {saving ? 'Đang lưu…' : 'Cập nhật'}
                  </button>
                </div>
                {validateMsg && (
                  <div className={`text-sm mb-3 p-2 rounded ${validateMsg.includes('PASS') ? 'bg-green-100' : 'bg-red-100'}`}>
                    {validateMsg}
                  </div>
                )}
                <textarea 
                  className="w-full h-96 rounded border font-mono text-sm p-3" 
                  style={{borderColor:'var(--input)'}} 
                  value={cfgText} 
                  onChange={e=>setCfgText(e.target.value)}
                  placeholder="Nhập cấu hình JSON..."
                />
              </div>
            )}

            {/* Tab 2: History */}
            {activeTab === 'history' && (
              <div>
                <div className="flex items-center gap-2 mb-3">
                  <h3 className="font-semibold">Lịch sử cấu hình</h3>
                  <button className="btn btn-ghost" onClick={loadHistory} disabled={historyLoading}>
                    {historyLoading ? 'Đang tải…' : 'Tải lại'}
                  </button>
                </div>
                <div className="overflow-x-auto">
                  <table className="w-full">
                    <thead>
                      <tr className="border-b" style={{borderColor:'var(--border)'}}>
                        <th className="text-left py-2 pr-4">Version</th>
                        <th className="text-left py-2 pr-4">Thời gian</th>
                        <th className="text-left py-2 pr-4">Hành động</th>
                        <th className="text-left py-2 pr-4">Người dùng</th>
                        <th className="text-left py-2">Thao tác</th>
                      </tr>
                    </thead>
                    <tbody>
                      {history?.length ? history.map(h => (
                        <tr key={h.version} className="border-b" style={{borderColor:'var(--border)'}}>
                          <td className="py-2 pr-4 font-mono">v{h.version}</td>
                          <td className="py-2 pr-4">{new Date(h.timestamp).toLocaleString()}</td>
                          <td className="py-2 pr-4">{h.action}</td>
                          <td className="py-2 pr-4">{h.user}</td>
                          <td className="py-2">
                            <button className="btn btn-sm" onClick={()=>onRollback(h.version)}>
                              Rollback
                            </button>
                          </td>
                        </tr>
                      )) : (
                        <tr>
                          <td colSpan={5} className="py-4 text-center text-muted-foreground">
                            Chưa có lịch sử
                          </td>
                        </tr>
                      )}
                    </tbody>
                  </table>
                </div>
              </div>
            )}

            {/* Tab 3: Import/Export */}
            {activeTab === 'import' && (
              <div className="grid lg:grid-cols-2 gap-6">
                {/* Export Section */}
                <div className="card border p-4" style={{borderColor:'var(--border)'}}>
                  <h3 className="font-semibold mb-3">Export</h3>
                  <div className="flex items-center gap-2 mb-3">
                    <label className="text-sm">Định dạng:
                      <select 
                        className="ml-2 rounded border px-2 py-1" 
                        style={{borderColor:'var(--input)'}}
                        value={importFormat} 
                        onChange={e=>setImportFormat(e.target.value as 'json'|'yaml')}
                      >
                        <option value="json">JSON</option>
                        <option value="yaml">YAML</option>
                      </select>
                    </label>
                    <button 
                      className="btn btn-primary ml-auto" 
                      onClick={()=>onExport(importFormat)} 
                      disabled={exporting}
                    >
                      {exporting ? 'Đang export…' : 'Export'}
                    </button>
                  </div>
                  <div className="text-sm text-muted-foreground">
                    Xuất cấu hình hiện tại ra file {importFormat.toUpperCase()}
                  </div>
                </div>

                {/* Import Section */}
                <div className="card border p-4" style={{borderColor:'var(--border)'}}>
                  <h3 className="font-semibold mb-3">Import</h3>
                  <div className="flex items-center gap-2 mb-3">
                    <label className="text-sm">Định dạng:
                      <select 
                        className="ml-2 rounded border px-2 py-1" 
                        style={{borderColor:'var(--input)'}}
                        value={importFormat} 
                        onChange={e=>setImportFormat(e.target.value as 'json'|'yaml')}
                      >
                        <option value="json">JSON</option>
                        <option value="yaml">YAML</option>
                      </select>
                    </label>
                    <button 
                      className="btn btn-danger ml-auto" 
                      onClick={onImport}
                    >
                      Import
                    </button>
                  </div>
                  <textarea 
                    className="w-full h-40 rounded border font-mono text-sm p-2" 
                    style={{borderColor:'var(--input)'}} 
                    placeholder={importFormat==='json' ? '{"version":2, ...}' : 'version: 2\n...'} 
                    value={importText} 
                    onChange={e=>setImportText(e.target.value)}
                  />
                  <div className="text-sm text-muted-foreground mt-2">
                    ⚠️ Import sẽ ghi đè lên cấu hình hiện tại
                  </div>
                </div>
              </div>
            )}
          </div>
        </div>
      </section>
    </AppShell>
  )
}


