// useWebSocket.ts
import { useCallback, useEffect, useRef, useState } from 'react'
import { WebSocketClient } from '@/lib/web-socket-client'

interface WebSocketOptions {
  reconnectDelay?: number
  maxReconnectDelay?: number
  onMessage?: (msg: string) => void
}

export function useWebSocket(url: string, options: WebSocketOptions = {}) {
  const {
    reconnectDelay = 1000,
    maxReconnectDelay = 10000,
    onMessage,
  } = options

  const clientRef = useRef<WebSocketClient | null>(null)
  const [isConnected, setIsConnected] = useState(false)

  const handleOpen = useCallback(() => setIsConnected(true), [])
  const handleClose = useCallback(() => setIsConnected(false), [])

  useEffect(() => {
    clientRef.current = new WebSocketClient({
      url,
      onMessage: msg => onMessage?.(msg),
      reconnectDelay,
      maxReconnectDelay,
    })

    const socket = clientRef.current.socket

    if (socket) {
      socket.addEventListener('open', handleOpen)
      socket.addEventListener('close', handleClose)
    }

    return () => {
      if (socket) {
        socket.removeEventListener('open', handleOpen)
        socket.removeEventListener('close', handleClose)
      }
      clientRef.current?.close()
    }
  }, [url, reconnectDelay, maxReconnectDelay, onMessage, handleOpen, handleClose])

  return { isConnected }
}
