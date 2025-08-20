import React from 'react'
import { createRoot } from 'react-dom/client'
import { BrowserRouter, Routes, Route, Link } from 'react-router-dom'
import './styles.css'
import Fe06Chart from './features/Fe06Chart'
import Fe06Page from './features/fe06/Fe06Page'
import LogsPage from './features/LogsPage'
import LoginPage from './features/auth/LoginPage'
import Dashboard from './features/dashboard/Dashboard'
import UserSettingsPage from './features/user/UserSettingsPage'
import AdminPage from './features/admin/AdminPage'
import CenterConfigPage from './features/center/CenterConfigPage'
import JobsPage from './features/jobs/JobsPage'
import DiagnosticsPage from './features/diagnostics/DiagnosticsPage'
import Rs485AddressesPage from './features/rs485/Rs485AddressesPage'
import SafetyConfigPage from './features/safety/SafetyConfigPage'
import AuditTrailPage from './features/audit/AuditTrailPage'
import ConfigPage from './features/config/ConfigPage'
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
            <Route path="/fe06" element={<Guard><Fe06Page/></Guard>} />
            <Route path="/logs" element={<LogsPage />} />
            <Route path="/user" element={<Guard><UserSettingsPage/></Guard>} />
            <Route path="/admin" element={<Guard><AdminPage/></Guard>} />
            <Route path="/config" element={<Guard><ConfigPage/></Guard>} />
            <Route path="/center" element={<Guard><CenterConfigPage/></Guard>} />
            <Route path="/jobs" element={<Guard><JobsPage/></Guard>} />
            <Route path="/diag" element={<Guard><DiagnosticsPage/></Guard>} />
            <Route path="/rs485" element={<Guard><Rs485AddressesPage/></Guard>} />
            <Route path="/safety" element={<Guard><SafetyConfigPage/></Guard>} />
            <Route path="/audit" element={<Guard><AuditTrailPage/></Guard>} />
          </Routes>
        </BrowserRouter>
      </ToastProvider>
    </ErrorBoundary>
  </React.StrictMode>
)


