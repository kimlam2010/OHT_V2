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

function StatusBadge({ isMoving }: { isMoving: boolean }) {
  return (
    <Badge
      variant={isMoving ? 'default' : 'secondary'}
      className="rounded-full w-fit"
    >
      <span className="text-xs">{isMoving ? 'MOVING' : 'STOP'}</span>
    </Badge>
  )
}

export default function PositionAndSpeedCard(props: Props) {
  const { robot } = props
  return (
    <Card className="gap-3 shadow">
      <CardHeader>
        <CardTitle>Position & Speed</CardTitle>
        <CardAction>
          <StatusBadge isMoving={robot.speed > 0} />
        </CardAction>
      </CardHeader>
      <CardContent>
        <div className="flex flex-col space-y-1">
          {/* Current Speed */}
          <div className="flex justify-between">
            <span className="text-sm text-muted-foreground">Current Speed</span>
            <span className="font-mono font-semibold text-primary">
              {robot.speed.toFixed(2)}
              {' '}
              m/s
            </span>
          </div>
          {/* Position */}
          <div className="flex items-center justify-between">
            <span className="text-sm text-muted-foreground">Position</span>
            <div className="grid grid-cols-3 text-center divide-x">
              <div className="px-1">
                <span className="text-xs text-muted-foreground">X</span>
                <div className="font-mono font-semibold text-primary">
                  {robot.position.x}
                  {' '}
                  m
                </div>
              </div>
              <div className="px-1">
                <span className="text-xs text-muted-foreground">Y</span>
                <div className="font-mono font-semibold text-primary">
                  {robot.position.y}
                  {' '}
                  m
                </div>
              </div>
              <div className="px-1">
                <span className="text-xs text-muted-foreground">Z</span>
                <div className="font-mono font-semibold text-primary">
                  {robot.position.z}
                  {' '}
                  m
                </div>
              </div>
            </div>
          </div>
        </div>
      </CardContent>
    </Card>
  )
}
