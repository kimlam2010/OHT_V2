export type MessageHandler = (message: string) => void

interface WebSocketClientConfig {
  url: string
  onMessage: MessageHandler
  reconnectDelay?: number
  maxReconnectDelay?: number
}

export class WebSocketClient {
  public socket: WebSocket | null = null
  private reconnectAttempts = 0
  private readonly config: Required<WebSocketClientConfig>

  constructor({
    url,
    onMessage,
    reconnectDelay = 1000,
    maxReconnectDelay = 10000,
  }: WebSocketClientConfig) {
    this.config = {
      url,
      onMessage,
      reconnectDelay,
      maxReconnectDelay,
    }
    this.connect()
  }

  private connect(): void {
    this.socket = new WebSocket(this.config.url)
    this.setupEventListeners()
  }

  private setupEventListeners(): void {
    if (!this.socket)
      return

    this.socket.onopen = this.handleOpen.bind(this)
    this.socket.onmessage = this.handleMessage.bind(this)
    this.socket.onclose = this.handleClose.bind(this)
    this.socket.onerror = this.handleError.bind(this)
  }

  private handleOpen(): void {
    this.reconnectAttempts = 0
  }

  private handleMessage(event: MessageEvent): void {
    this.config.onMessage(event.data)
  }

  private handleClose(): void {
    this.scheduleReconnect()
  }

  private handleError(error: Event): void {
    console.error('⚠️ WebSocket error:', error)
    this.socket?.close()
  }

  private scheduleReconnect(): void {
    this.reconnectAttempts++
    const delay = Math.min(
      this.config.reconnectDelay * this.reconnectAttempts,
      this.config.maxReconnectDelay,
    )
    setTimeout(() => this.connect(), delay)
  }

  public close(): void {
    this.socket?.close()
    this.socket = null
  }
}
