import { Badge } from '@/components/ui/badge'
import {
  Card,
  CardAction,
  CardContent,
  CardHeader,
  CardTitle,
} from '@/components/ui/card'

function StatusBadge() {
  return (
    <Badge variant="success" className="rounded-full w-fit">
      <span className="text-xs">ONLINE</span>
    </Badge>
  )
}

export default function CommunicationCard() {
  return (
    <Card className="gap-3 shadow">
      <CardHeader>
        <CardTitle>Communication</CardTitle>
        <CardAction>
          <StatusBadge />
        </CardAction>
      </CardHeader>
      <CardContent>
        <div className="flex flex-col space-y-1 text-sm">
          {/* Connection */}
          <div className="flex justify-between">
            <span className="text-muted-foreground">Connection</span>
            <span className="font-medium text-primary">
              Ethernet
            </span>
          </div>
          {/* Signal Strength */}
          <div className="flex justify-between">
            <span className="text-muted-foreground">Signal Strength</span>
            <span className="font-medium text-primary">
              -45
              {' '}
              dBm
            </span>
          </div>
          {/* Last Updated */}
          <div className="flex justify-between">
            <span className="text-muted-foreground">Last Updated</span>
            <span className="font-medium text-primary">
              10:24:31
            </span>
          </div>
        </div>
      </CardContent>
    </Card>
  )
}
