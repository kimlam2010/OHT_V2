import { useLocalStorage } from '@reactuses/core'
import { ArrowDown, ArrowLeft, ArrowRight, ArrowUp, Square } from 'lucide-react'
import { toast } from 'react-hot-toast'
import { Badge } from '@/components/ui/badge'
import { Button } from '@/components/ui/button'
import {
  Card,
  CardAction,
  CardContent,
  CardHeader,
  CardTitle,
} from '@/components/ui/card'
import { Input } from '@/components/ui/input'
import { Label } from '@/components/ui/label'
import { MANUAL_CONTROL_ANGLE, MANUAL_CONTROL_SPEED } from '@/constants/string'
import { useRobotMoveBackwardMutation, useRobotMoveForwardMutation, useRobotStopMutation, useRobotTurnLeftMutation, useRobotTurnRightMutation } from '@/hooks'
import { cn } from '@/lib/utils'

interface Props {
  isEnabled: boolean
}

const defaultSpeed = 100
const defaultAngle = 0
export default function ManualControlCard({ isEnabled }: Props) {
  const [speed, setSpeed] = useLocalStorage<number>(MANUAL_CONTROL_SPEED, defaultSpeed)
  const [angle, setAngle] = useLocalStorage<number>(MANUAL_CONTROL_ANGLE, defaultAngle)
  const { mutate: moveForward } = useRobotMoveForwardMutation()
  const { mutate: moveBackward } = useRobotMoveBackwardMutation()
  const { mutate: turnLeft } = useRobotTurnLeftMutation()
  const { mutate: turnRight } = useRobotTurnRightMutation()
  const { mutate: stop } = useRobotStopMutation()

  const handleMoveForward = () => {
    moveForward({ speed: speed || defaultSpeed }, {
      onSuccess: (data) => {
        toast.success(data.message)
      },
      onError: () => {
        toast.error('Move forward failed')
      },
    })
  }

  const handleMoveBackward = () => {
    moveBackward({ speed: speed || defaultSpeed }, {
      onSuccess: (data) => {
        toast.success(data.message)
      },
      onError: () => {
        toast.error('Move backward failed')
      },
    })
  }

  const handleTurnLeft = () => {
    turnLeft({ angle: angle || defaultAngle }, {
      onSuccess: (data) => {
        toast.success(data.message)
      },
      onError: () => {
        toast.error('Turn left failed')
      },
    })
  }

  const handleTurnRight = () => {
    turnRight({ angle: angle || defaultAngle }, {
      onSuccess: (data) => {
        toast.success(data.message)
      },
      onError: () => {
        toast.error('Turn right failed')
      },
    })
  }

  const handleStop = () => {
    stop(undefined, {
      onSuccess: (data) => {
        toast.success(data.message)
      },
      onError: () => {
        toast.error('Stop failed')
      },
    })
  }

  return (
    <Card className="shadow">
      <CardHeader>
        <CardTitle>Manual Control Panel</CardTitle>
        <CardAction><Badge variant={isEnabled ? 'success' : 'secondary'}>{isEnabled ? 'ENABLE' : 'DISABLE'}</Badge></CardAction>
      </CardHeader>
      <CardContent>
        <div className="flex flex-wrap gap-4">
          <div className="grid grid-cols-3 gap-4 w-fit">
            <Button variant="outline" className="flex flex-col col-start-2 gap-2 h-fit" disabled={!isEnabled} onClick={handleMoveForward}>
              <ArrowUp className="size-4" />
              <span className="text-xs">Forward</span>
            </Button>
            <Button variant="outline" className="flex flex-col row-start-2 gap-2 h-fit" disabled={!isEnabled} onClick={handleTurnLeft}>
              <ArrowLeft className="size-4" />
              <span className="text-xs">Left</span>
            </Button>
            <Button variant="outline" className="flex flex-col col-start-2 gap-2 h-fit" disabled={!isEnabled} onClick={handleStop}>
              <Square className="size-4" />
              <span className="text-xs">Stop</span>
            </Button>
            <Button variant="outline" className="flex flex-col gap-2 h-fit" disabled={!isEnabled} onClick={handleTurnRight}>
              <ArrowRight className="size-4" />
              <span className="text-xs">Right</span>
            </Button>
            <Button variant="outline" className="flex flex-col col-start-2 gap-2 h-fit" disabled={!isEnabled} onClick={handleMoveBackward}>
              <ArrowDown className="size-4" />
              <span className="text-xs">Backward</span>
            </Button>
          </div>
          <div className="grid grid-cols-3 gap-2 items-center h-fit">
            <Label className={cn(isEnabled ? 'text-foreground' : 'text-muted-foreground')}>Speed:</Label>
            <Input
              disabled={!isEnabled}
              type="number"
              step={0.01}
              min={0}
              max={100}
              value={speed ?? defaultSpeed}
              onChange={e => setSpeed(Number(e.target.value))}
            />
            <Label className={cn(isEnabled ? 'text-foreground' : 'text-muted-foreground')}>m/s</Label>
            <Label className={cn(isEnabled ? 'text-foreground' : 'text-muted-foreground')}>Angle:</Label>
            <Input
              disabled={!isEnabled}
              type="number"
              min={0}
              max={360}
              value={angle ?? defaultAngle}
              onChange={e => setAngle(Number(e.target.value))}
            />
            <Label className={cn(isEnabled ? 'text-foreground' : 'text-muted-foreground')}>° (degree)</Label>
          </div>
        </div>

      </CardContent>
    </Card>
  )
}
