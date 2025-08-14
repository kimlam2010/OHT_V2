import React from 'react'
import { createRoot } from 'react-dom/client'
import { BrowserRouter, Routes, Route, Link } from 'react-router-dom'
import './styles.css'
import Fe06Chart from './features/Fe06Chart'
import LogsPage from './features/LogsPage'
import LoginPage from './features/auth/LoginPage'
import Dashboard from './features/dashboard/Dashboard'
import { getToken } from './features/auth/auth'
import { ToastProvider } from './components/Toast'
import ErrorBoundary from './components/ErrorBoundary'

function Home() { return (<div className="p-6"><Link className="text-[var(--primary)] underline" to="/login">Đi đến đăng nhập</Link></div>) }
function Fe06Demo() { return (<div className="p-0"><Fe06Chart/></div>) }

function Guard({ children }: { children: JSX.Element }){
  const token = getToken()
  if (!token) {
    return (<div className="p-6">Chưa đăng nhập. <Link className="text-[var(--primary)] underline" to="/login">Đăng nhập</Link></div>)
  }
  return children
}

createRoot(document.getElementById('root')!).render(
  <React.StrictMode>
    <ErrorBoundary>
      <ToastProvider>
        <BrowserRouter>
          <Routes>
            <Route path="/" element={<Home />} />
            <Route path="/login" element={<LoginPage />} />
            <Route path="/dashboard" element={<Guard><Dashboard /></Guard>} />
            <Route path="/fe06" element={<Fe06Demo />} />
            <Route path="/logs" element={<LogsPage />} />
          </Routes>
        </BrowserRouter>
      </ToastProvider>
    </ErrorBoundary>
  </React.StrictMode>
)


