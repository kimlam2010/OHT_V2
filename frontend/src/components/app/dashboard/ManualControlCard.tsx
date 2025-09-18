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
import { MANUAL_CONTROL_POSITION, MANUAL_CONTROL_SPEED } from '@/constants/string'
import { useRobotCargoDownMutation, useRobotCargoUpMutation, useRobotMoveBackwardMutation, useRobotMoveForwardMutation, useRobotStopMutation } from '@/hooks'
import { cn } from '@/lib/utils'

interface Props {
  isEnabled: boolean
}

const defaultSpeed = 100
const defaultPosition = 0
export default function ManualControlCard({ isEnabled }: Props) {
  const [speed, setSpeed] = useLocalStorage<number>(MANUAL_CONTROL_SPEED, defaultSpeed)
  const [position, setPosition] = useLocalStorage<number>(MANUAL_CONTROL_POSITION, defaultPosition)
  const { mutate: moveForward } = useRobotMoveForwardMutation()
  const { mutate: moveBackward } = useRobotMoveBackwardMutation()
  const { mutate: cargoDown } = useRobotCargoDownMutation()
  const { mutate: cargoUp } = useRobotCargoUpMutation()
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

  const handleCargoLower = () => {
    cargoDown({ position: position || defaultPosition }, {
      onSuccess: (data) => {
        toast.success(data.message)
      },
      onError: () => {
        toast.error('Cargo down failed')
      },
    })
  }

  const handleCargoLift = () => {
    cargoUp({ position: position || defaultPosition }, {
      onSuccess: (data) => {
        toast.success(data.message)
      },
      onError: () => {
        toast.error('Cargo up failed')
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
    <Card className="gap-3 shadow">
      <CardHeader>
        <CardTitle>Manual Control Panel</CardTitle>
        <CardAction><Badge variant={isEnabled ? 'success' : 'secondary'}>{isEnabled ? 'ENABLE' : 'DISABLE'}</Badge></CardAction>
      </CardHeader>
      <CardContent>
        <div className="flex flex-wrap gap-4">
          <div className="grid grid-cols-3 gap-4 w-fit">
            <Button variant="outline" className="flex flex-col col-start-2 gap-2 h-fit" disabled={!isEnabled} onClick={handleCargoLift}>
              <ArrowUp className="size-4" />
              <span className="text-xs">Cargo up</span>
            </Button>
            <Button variant="outline" className="flex flex-col row-start-2 gap-2 h-fit" disabled={!isEnabled} onClick={handleMoveBackward}>
              <ArrowLeft className="size-4" />
              <span className="text-xs">Backward</span>
            </Button>
            <Button variant="outline" className="flex flex-col col-start-2 gap-2 h-fit" disabled={!isEnabled} onClick={handleStop}>
              <Square className="size-4" />
              <span className="text-xs">Stop</span>
            </Button>
            <Button variant="outline" className="flex flex-col gap-2 h-fit" disabled={!isEnabled} onClick={handleMoveForward}>
              <ArrowRight className="size-4" />
              <span className="text-xs">Forward</span>
            </Button>
            <Button variant="outline" className="flex flex-col col-start-2 gap-2 h-fit" disabled={!isEnabled} onClick={handleCargoLower}>
              <ArrowDown className="size-4" />
              <span className="text-xs">Cargo down</span>
            </Button>
          </div>
          <div className="grid grid-cols-3 gap-4 items-center h-fit">
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
            <Label className={cn(isEnabled ? 'text-foreground' : 'text-muted-foreground')}>Position:</Label>
            <Input
              disabled={!isEnabled}
              type="number"
              min={0}
              max={100}
              value={position ?? defaultPosition}
              onChange={e => setPosition(Number(e.target.value))}
            />
            <Label className={cn(isEnabled ? 'text-foreground' : 'text-muted-foreground')}> %</Label>
          </div>
        </div>

      </CardContent>
    </Card>
  )
}
