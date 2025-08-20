import React from 'react'

type Toast = { id: number; message: string }
type Ctx = { push: (msg: string) => void }

const ToastContext = React.createContext<Ctx>({ push: () => {} })

export function useToast(){ return React.useContext(ToastContext) }

export function ToastProvider({ children }: { children: React.ReactNode }){
  const [toasts, setToasts] = React.useState<Toast[]>([])
  const push = (message: string) => {
    const id = Date.now()
    setToasts(t => [...t, { id, message }])
    setTimeout(() => setToasts(t => t.filter(x => x.id !== id)), 2500)
  }
  return (
    <ToastContext.Provider value={{ push }}>
      {children}
      <div className="fixed bottom-4 right-4 space-y-2 z-50">
        {toasts.map(t => (
          <div key={t.id} className="card p-3 shadow-card border" style={{ borderColor: 'var(--border)' }}>
            <div className="text-sm">{t.message}</div>
          </div>
        ))}
      </div>
    </ToastContext.Provider>
  )
}


