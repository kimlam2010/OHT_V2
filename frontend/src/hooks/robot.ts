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

export function useRobotCargoLowerMutation() {
  return useMutation({
    mutationFn: robotApi.cargoLowerCommand,
  })
}

export function useRobotCargoLiftMutation() {
  return useMutation({
    mutationFn: robotApi.cargoLiftCommand,
  })
}

export function useRobotStopMutation() {
  return useMutation({
    mutationFn: robotApi.StopCommand,
  })
}
