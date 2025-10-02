import { useInterval } from '@reactuses/core'
import { useState } from 'react'
import { Badge } from '@/components/ui/badge'
import {
  Card,
  CardContent,
  CardHeader,
  CardTitle,
} from '@/components/ui/card'

export default function CommunicationCard() {
  const rs485Status = 'Active'
  const [wifiSignal, setWifiSignal] = useState(-10)
  const [latency, setLatency] = useState(100)

  useInterval(() => {
    setWifiSignal(Math.floor(Math.random() * 100) - 100)
    setLatency(Math.floor(Math.random() * 100) + 100)
  }, 2000)

  return (
    <Card className="gap-3 shadow">
      <CardHeader>
        <CardTitle>Communication</CardTitle>
      </CardHeader>
      <CardContent className="my-0">
        <div className="flex flex-col space-y-1 text-sm">

          <div className="flex justify-between items-center">
            <span className="text-muted-foreground">Center Connection</span>
            <Badge variant="success" className="leading-none">Connected</Badge>
          </div>
          <div className="flex justify-between items-center">
            <span className="text-muted-foreground">RS485 Status</span>
            <Badge variant={rs485Status === 'Active' ? 'success' : 'destructive'} className="leading-none">
              {rs485Status}
            </Badge>
          </div>
          <div className="flex justify-between items-center">
            <span className="text-muted-foreground">WiFi Signal</span>
            <span className="font-medium text-sm">
              {wifiSignal}
              {' '}
              dBm
            </span>
          </div>
          <div className="flex justify-between items-center">
            <span className="text-muted-foreground">
              Latency
            </span>
            <span className="font-medium text-sm">
              {latency}
              {' '}
              ms
            </span>
          </div>
        </div>
      </CardContent>
    </Card>
  )
}
