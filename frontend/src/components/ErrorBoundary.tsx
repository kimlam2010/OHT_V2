import React from 'react'

type Props = { children: React.ReactNode }
type State = { hasError: boolean; message?: string }

export default class ErrorBoundary extends React.Component<Props, State> {
  constructor(props: Props){
    super(props)
    this.state = { hasError: false }
  }
  static getDerivedStateFromError(err: unknown){
    return { hasError: true, message: (err as any)?.message || 'Unexpected error' }
  }
  componentDidCatch(err: Error){
    console.error('UI ErrorBoundary caught:', err)
  }
  render(){
    if (this.state.hasError) {
      return (
        <div className="p-6">
          <h2 className="text-lg font-semibold mb-2">Đã xảy ra lỗi giao diện</h2>
          <div className="text-sm">{this.state.message}</div>
          <button className="btn btn-ghost mt-3" onClick={()=> location.reload()}>Tải lại</button>
        </div>
      )
    }
    return this.props.children
  }
}


