import http from '@/lib/http'

export interface MoveRobotRequest {
  speed: number
}
export interface MoveRobotResponse {
  success: boolean
  message: string
  direction: 'forward' | 'backward' | 'left' | 'right' | 'stop'
  speed: number
  estimated_duration: number
  timestamp: string
}

export interface CargoParamsRequest {
  position: number
}

export interface RobotModeResponse {
  success: boolean
  mode: 'auto' | 'manual'
  timestamp: string
}

export interface UpdateRobotModeResponse {
  success: boolean
  message: string
  previous_mode: RobotModeResponse['mode']
  current_mode: RobotModeResponse['mode']
  timestamp: string
}

export const robotApi = {
  getRobotMode: (): Promise<RobotModeResponse> => {
    return http.get('/robot/mode', {
      headers: {
        'x-api': 'software',
      },
    })
  },
  updateRobotMode: (mode: RobotModeResponse['mode']): Promise<UpdateRobotModeResponse> => {
    return http.post('/robot/mode', { mode }, {
      headers: {
        'x-api': 'software',
      },
    })
  },
  moveForwardCommand: (params: MoveRobotRequest): Promise<MoveRobotResponse> => {
    return http.post('/robot/move/forward', undefined, {
      params,
      headers: {
        'x-api': 'software',
      },
    })
  },
  moveBackCommand: (params: MoveRobotRequest): Promise<MoveRobotResponse> => {
    return http.post('/robot/move/backward', undefined, {
      params,
      headers: {
        'x-api': 'software',
      },
    })
  },
  cargoLowerCommand: (params: CargoParamsRequest): Promise<MoveRobotResponse> => {
    return http.post('/robot/cargo/lower', undefined, {
      params,
      headers: {
        'x-api': 'software',
      },
    })
  },
  cargoLiftCommand: (params: CargoParamsRequest): Promise<MoveRobotResponse> => {
    return http.post('/robot/cargo/lift', undefined, {
      params,
      headers: {
        'x-api': 'software',
      },
    })
  },
  StopCommand: (): Promise<MoveRobotResponse> => {
    return http.post('/robot/move/stop', {
      headers: {
        'x-api': 'software',
      },
    })
  },
}
