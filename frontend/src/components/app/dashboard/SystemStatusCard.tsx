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

function StatusBadge({ status }: { status: Robot['status'] }) {
  return (
    <Badge variant={status === 'idle' ? 'secondary' : status === 'moving' ? 'default' : status === 'docking' ? 'warning' : status === 'charging' ? 'success' : status === 'error' ? 'destructive' : status === 'emergency_stop' ? 'destructive' : status === 'paused' ? 'outline' : 'secondary'} className="rounded-full w-fit">
      <span className="text-xs uppercase">{status}</span>
    </Badge>
  )
}

function BatteryLevelLabel({ level }: { level: number }) {
  const color = level > 80 ? 'text-success' : level > 20 ? 'text-warning' : 'text-destructive'
  return (
    <span className={`font-medium  ${color}`}>
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
          <StatusBadge status="paused" />
        </CardAction>
      </CardHeader>
      <CardContent className="my-0">
        <div className="flex flex-col space-y-1 text-sm">
          {/* Operating Mode */}
          <div className="flex justify-between">
            <span className=" text-muted-foreground">Operating Mode</span>
            <span className=" font-medium capitalize text-primary">{robot.mode}</span>
          </div>
          {/* Battery Level */}
          <div className="flex justify-between">
            <span className=" text-muted-foreground">Battery Level</span>
            <span className=" font-medium text-primary">
              <BatteryLevelLabel level={robot.battery_level} />
            </span>
          </div>
        </div>
      </CardContent>
    </Card>
  )
}
