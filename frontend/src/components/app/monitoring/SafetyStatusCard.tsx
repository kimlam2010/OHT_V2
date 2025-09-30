import { Badge } from '@/components/ui/badge'
import {
  Card,
  CardContent,
  CardHeader,
  CardTitle,
} from '@/components/ui/card'

export default function SafetyStatusCard() {
  return (
    <Card className="gap-3 shadow">
      <CardHeader>
        <CardTitle>Safety Status</CardTitle>
      </CardHeader>
      <CardContent className="my-0">
        <div className="flex flex-col space-y-1 text-sm">

          <div className="flex justify-between items-center">
            <span className="text-muted-foreground">E-Stop Status</span>
            <Badge variant="success" className="leading-none">Ready</Badge>
          </div>
          <div className="flex justify-between items-center">
            <span className="text-muted-foreground">Safety Zone</span>
            <Badge variant="success" className="leading-none">
              Clear
            </Badge>
          </div>
          <div className="flex justify-between items-center">
            <span className="text-muted-foreground">Emergency Contact</span>
            <span className="font-medium text-sm">
              +84 123 456 789
            </span>
          </div>
        </div>
      </CardContent>
    </Card>
  )
}
