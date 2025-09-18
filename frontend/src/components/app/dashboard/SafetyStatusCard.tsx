import type { Safety } from '@/types'
import { Badge } from '@/components/ui/badge'
import {
  Card,
  CardAction,
  CardContent,
  CardHeader,
  CardTitle,
} from '@/components/ui/card'
import { cn } from '@/lib/utils'

interface Props {
  safety: Safety
}

function StatusBadge({ state }: { state: Safety['safety_state'] }) {
  return (
    <Badge variant={state === 'SAFE' ? 'success' : state === 'WARNING' ? 'warning' : state === 'EMERGENCY' ? 'destructive' : 'secondary'} className="uppercase rounded-full w-fit">
      <span className="text-xs">{state}</span>
    </Badge>
  )
}

export default function SafetyStatusCard({ safety }: Props) {
  return (
    <Card className="gap-3 shadow">
      <CardHeader>
        <CardTitle>Safety Status</CardTitle>
        <CardAction>
          <StatusBadge state={safety.safety_state} />
        </CardAction>
      </CardHeader>
      <CardContent>
        <div className="flex flex-col space-y-1 text-sm">
          {/* Safety Zone */}
          <div className="flex justify-between">
            <span className="text-muted-foreground">Emergency Status</span>
            <span className={cn('font-medium', safety.emergency_status === 'E-STOP' ? 'text-destructive' : safety.emergency_status === 'Unknown (FW offline)' ? 'text-muted-foreground' : 'text-primary')}>
              {safety.emergency_status}
            </span>
          </div>
          {/* Obstacles */}
          <div className="flex justify-between">
            <span className="text-muted-foreground">Obstacles</span>
            <span className={cn('font-medium', safety.obstacles_present ? 'text-destructive' : safety.obstacles_present === false ? 'text-primary' : 'text-muted-foreground')}>{safety.obstacles_present ? 'Yes' : safety.obstacles_present === false ? 'No' : 'Unknown'}</span>
          </div>
          {/* Emergency Status */}
          <div className="flex justify-between">
            <span className="text-muted-foreground">Active Alerts</span>
            <span className="font-medium text-primary">
              {safety.active_alerts_count}
            </span>
          </div>
        </div>
      </CardContent>
    </Card>
  )
}
