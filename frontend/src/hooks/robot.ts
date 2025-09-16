import { useMutation, useQuery } from '@tanstack/react-query'
import { robotApi } from '@/api/robot'

export const ROBOT_MODE_KEY = 'robot-mode'
export function useRobotModeQuery() {
  return useQuery({
    queryKey: [ROBOT_MODE_KEY],
    queryFn: robotApi.getRobotMode,
  })
}

export function useRobotModeMutation() {
  return useMutation({
    mutationFn: robotApi.updateRobotMode,
  })
}

export function useRobotMoveForwardMutation() {
  return useMutation({
    mutationFn: robotApi.moveForwardCommand,
  })
}

export function useRobotMoveBackwardMutation() {
  return useMutation({
    mutationFn: robotApi.moveBackCommand,
  })
}

export function useRobotTurnLeftMutation() {
  return useMutation({
    mutationFn: robotApi.turnLeftCommand,
  })
}

export function useRobotTurnRightMutation() {
  return useMutation({
    mutationFn: robotApi.turnRightCommand,
  })
}

export function useRobotStopMutation() {
  return useMutation({
    mutationFn: robotApi.StopCommand,
  })
}
