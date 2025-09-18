import type { Robot } from '@/types'
import { Badge } from '@/components/ui/badge'
import {
  Card,
  CardAction,
  CardContent,
  CardHeader,
  CardTitle,
} from '@/components/ui/card'

interface Props {
  robot: Robot
}

function StatusBadge({ status }: { status: string }) {
  return (
    <Badge variant="secondary" className="rounded-full w-fit">
      <span className="text-xs uppercase">{status}</span>
    </Badge>
  )
}

function BatteryLevelLabel({ level }: { level: number }) {
  const color = level > 80 ? 'text-success' : level > 20 ? 'text-warning' : 'text-destructive'
  return (
    <span className={`font-semibold font-mono ${color}`}>
      {level}
      %
    </span>
  )
}

export default function SystemStatusCard(props: Props) {
  const { robot } = props

  return (
    <Card className="gap-3 shadow">
      <CardHeader>
        <CardTitle>System Status</CardTitle>
        <CardAction>
          <StatusBadge status={robot.status} />
        </CardAction>
      </CardHeader>
      <CardContent className="my-0">
        <div className="flex flex-col space-y-1">
          {/* Operating Mode */}
          <div className="flex justify-between">
            <span className="text-sm text-muted-foreground">Operating Mode</span>
            <span className="font-mono font-semibold uppercase text-primary">{robot.mode}</span>
          </div>
          {/* Battery Level */}
          <div className="flex justify-between">
            <span className="text-sm text-muted-foreground">Battery Level</span>
            <span className="font-mono font-semibold text-primary">
              <BatteryLevelLabel level={robot.battery_level} />
            </span>
          </div>
        </div>
      </CardContent>
    </Card>
  )
}
