import { Badge } from '@/components/ui/badge'
import {
  Card,
  CardContent,
  CardHeader,
  CardTitle,
} from '@/components/ui/card'
import { formatDate } from '@/lib/date'

export default function SystemHealthCard() {
  return (
    <Card className="gap-3 shadow">
      <CardHeader>
        <CardTitle>System Health</CardTitle>
      </CardHeader>
      <CardContent className="my-0">
        <div className="flex flex-col space-y-1 text-sm">
          <div className="flex justify-between items-center">
            <span className="text-muted-foreground">Overall status</span>
            <Badge variant="success" className="leading-none">Healthy</Badge>
          </div>
          <div className="flex justify-between items-center">
            <span className="text-muted-foreground ">Last Restart</span>
            <span className="font-medium">
              {formatDate('2025-09-17T10:30:00')}
            </span>
          </div>
          <div className="flex justify-between items-center">
            <span className="text-muted-foreground">System Load</span>
            <span className="font-medium">0.45</span>
          </div>
        </div>
      </CardContent>
    </Card>
  )
}
