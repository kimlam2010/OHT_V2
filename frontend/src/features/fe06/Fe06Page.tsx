import React from 'react'
import AppShell from '../../components/AppShell'
import Fe06Chart from '../Fe06Chart'
import { Link } from 'react-router-dom'

export default function Fe06Page(){
  return (
    <AppShell>
      <div className="space-y-4">
        <div className="flex items-center justify-between">
          <h2 className="text-lg font-semibold">FE‑06 • Đồ thị v‑a‑x</h2>
          <Link to="/dashboard" className="btn btn-ghost">Tổng quan</Link>
        </div>
        <Fe06Chart />
      </div>
    </AppShell>
  )
}



