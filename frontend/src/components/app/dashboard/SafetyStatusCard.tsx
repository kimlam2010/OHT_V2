// import { Badge } from '@/components/ui/badge'
import {
  Card,
  CardAction,
  CardContent,
  CardHeader,
  CardTitle,
} from '@/components/ui/card'

// function StatusBadge({ status }: { status: string }) {
//   return (
//     <Badge variant={status === 'safe' ? 'success' : 'destructive'} className="rounded-full w-fit">
//       <span className="text-xs">SAFE</span>
//     </Badge>
//   )
// }

export default function SafetyStatusCard() {
  return (
    <Card className="gap-3 shadow">
      <CardHeader>
        <CardTitle>Safety Status</CardTitle>
        <CardAction>
          {/* <StatusBadge status={data.status} /> */}
        </CardAction>
      </CardHeader>
      <CardContent>
        <div className="flex flex-col space-y-1">
          {/* Safety Zone */}
          <div className="flex justify-between">
            <span className="text-sm text-muted-foreground">Emergency Status</span>
            <span className="font-semibold text-primary">
              {/* {data?.status} */}
            </span>
          </div>
          {/* Obstacles */}
          <div className="flex justify-between">
            <span className="text-sm text-muted-foreground">Obstacles</span>
            <span className="font-semibold text-primary">0</span>
          </div>
          {/* Emergency Status */}
          <div className="flex justify-between">
            <span className="text-sm text-muted-foreground">Active Alerts</span>
            <span className="font-semibold text-primary">
              Normal
            </span>
          </div>
        </div>
      </CardContent>
    </Card>
  )
}
