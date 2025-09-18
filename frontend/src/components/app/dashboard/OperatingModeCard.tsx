import type { RobotModeResponse } from '@/api/robot'
import { useQueryClient } from '@tanstack/react-query'
import { Hand, Play } from 'lucide-react'
import toast from 'react-hot-toast'
import { Button } from '@/components/ui/button'
import {
  Card,
  CardContent,
  CardHeader,
  CardTitle,
} from '@/components/ui/card'
import { ROBOT_MODE_KEY, useRobotModeMutation } from '@/hooks'

interface Props {
  mode: RobotModeResponse['mode']
}
export default function OperatingModeCard({ mode }: Props) {
  const { mutate: updateMode } = useRobotModeMutation()
  const queryClient = useQueryClient()

  const handleUpdateMode = (newMode: RobotModeResponse['mode']) => {
    if (newMode !== mode) {
      updateMode(newMode, {
        onSuccess: () => {
          // Invalidate the query to refetch the latest mode from the server
          queryClient.invalidateQueries({ queryKey: [ROBOT_MODE_KEY] })
        },
        onError: (error) => {
          toast.error((error as Error).message || 'Failed to update mode')
        },
      })
    }
  }
  return (
    <Card className="w-full gap-2 shadow">
      <CardHeader>
        <CardTitle>Operating mode</CardTitle>
      </CardHeader>
      <CardContent className="flex gap-4">
        <Button className="flex-1 h-12 gap-2" variant={mode === 'auto' ? 'default' : 'outline'} onClick={() => handleUpdateMode('auto')}>
          <Play />
          Auto
        </Button>
        <Button className="flex-1 h-12 gap-2" variant={mode === 'manual' ? 'default' : 'outline'} onClick={() => handleUpdateMode('manual')}>
          <Hand />
          Manual
        </Button>
      </CardContent>
    </Card>
  )
}
