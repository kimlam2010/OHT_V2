# OHT-50 Control Components Implementation

**Phiên bản:** v1.0  
**Ngày tạo:** 2024-08-24  
**Trạng thái:** Implementation Phase  
**Phase:** 2 - Core Components

---

## 🎮 **CONTROL COMPONENTS**

### **1. Directional Control Pad (src/components/control/DirectionalControlPad.tsx)**

```typescript
import React, { useState, useCallback, useRef } from 'react'
import { ChevronUp, ChevronDown, ChevronLeft, ChevronRight, Square } from 'lucide-react'
import { useControlStore } from '@stores/controlStore'
import { cn, throttle } from '@utils/helpers'
import { ControlCommand } from '@types'

interface DirectionalControlPadProps {
  className?: string
  size?: 'sm' | 'md' | 'lg'
  disabled?: boolean
  showLabels?: boolean
  continuousMode?: boolean
}

export const DirectionalControlPad: React.FC<DirectionalControlPadProps> = ({
  className,
  size = 'md',
  disabled = false,
  showLabels = true,
  continuousMode = false,
}) => {
  const { sendCommand, currentMode, isEmergencyStopActive } = useControlStore()
  const [activeDirection, setActiveDirection] = useState<string | null>(null)
  const [isPressed, setIsPressed] = useState(false)
  const pressTimeoutRef = useRef<NodeJS.Timeout>()
  const repeatIntervalRef = useRef<NodeJS.Timeout>()

  const isDisabled = disabled || isEmergencyStopActive || !currentMode

  const handleDirectionPress = useCallback(async (direction: string) => {
    if (isDisabled) return

    setActiveDirection(direction)
    setIsPressed(true)

    try {
      const command: Omit<ControlCommand, 'timestamp'> = {
        type: 'move',
        direction: direction as ControlCommand['direction'],
      }

      await sendCommand(command)

      if (continuousMode) {
        // Start repeating command
        pressTimeoutRef.current = setTimeout(() => {
          repeatIntervalRef.current = setInterval(async () => {
            if (!isDisabled) {
              await sendCommand(command)
            }
          }, 100) // Repeat every 100ms
        }, 500) // Start repeating after 500ms
      }
    } catch (error) {
      console.error('Failed to send direction command:', error)
    }
  }, [isDisabled, sendCommand, continuousMode])

  const handleDirectionRelease = useCallback(() => {
    setActiveDirection(null)
    setIsPressed(false)

    // Clear timers
    if (pressTimeoutRef.current) {
      clearTimeout(pressTimeoutRef.current)
    }
    if (repeatIntervalRef.current) {
      clearInterval(repeatIntervalRef.current)
    }

    // Send stop command
    if (!isDisabled) {
      sendCommand({ type: 'stop' }).catch(error => {
        console.error('Failed to send stop command:', error)
      })
    }
  }, [isDisabled, sendCommand])

  const handleStop = useCallback(async () => {
    if (isDisabled) return

    setActiveDirection(null)
    setIsPressed(false)

    // Clear timers
    if (pressTimeoutRef.current) {
      clearTimeout(pressTimeoutRef.current)
    }
    if (repeatIntervalRef.current) {
      clearInterval(repeatIntervalRef.current)
    }

    try {
      await sendCommand({ type: 'stop' })
    } catch (error) {
      console.error('Failed to send stop command:', error)
    }
  }, [isDisabled, sendCommand])

  const sizeClasses = {
    sm: 'w-32 h-32',
    md: 'w-40 h-40',
    lg: 'w-48 h-48',
  }

  const buttonSizeClasses = {
    sm: 'w-10 h-10',
    md: 'w-12 h-12',
    lg: 'w-14 h-14',
  }

  const directions = [
    { key: 'forward', icon: ChevronUp, label: 'Forward', position: 'top-0 left-1/2 -translate-x-1/2' },
    { key: 'backward', icon: ChevronDown, label: 'Backward', position: 'bottom-0 left-1/2 -translate-x-1/2' },
    { key: 'left', icon: ChevronLeft, label: 'Left', position: 'left-0 top-1/2 -translate-y-1/2' },
    { key: 'right', icon: ChevronRight, label: 'Right', position: 'right-0 top-1/2 -translate-y-1/2' },
  ]

  return (
    <div className={cn('relative', sizeClasses[size], className)}>
      {/* Directional Buttons */}
      {directions.map(({ key, icon: Icon, label, position }) => (
        <button
          key={key}
          className={cn(
            'control-button absolute rounded-full',
            'flex items-center justify-center',
            'transition-all duration-200 ease-in-out',
            'focus:outline-none focus:ring-2 focus:ring-primary-500',
            'active:scale-95',
            buttonSizeClasses[size],
            activeDirection === key && 'bg-primary-600 text-white scale-110',
            isDisabled && 'opacity-50 cursor-not-allowed',
            position
          )}
          onMouseDown={() => handleDirectionPress(key)}
          onMouseUp={handleDirectionRelease}
          onMouseLeave={handleDirectionRelease}
          onTouchStart={() => handleDirectionPress(key)}
          onTouchEnd={handleDirectionRelease}
          disabled={isDisabled}
          aria-label={label}
        >
          <Icon size={size === 'lg' ? 24 : size === 'md' ? 20 : 16} />
          {showLabels && (
            <span className="absolute -bottom-6 text-xs font-medium text-industrial-600">
              {label}
            </span>
          )}
        </button>
      ))}

      {/* Center Stop Button */}
      <button
        className={cn(
          'control-button absolute top-1/2 left-1/2 -translate-x-1/2 -translate-y-1/2',
          'rounded-full bg-safety-error text-white',
          'flex items-center justify-center',
          'transition-all duration-200 ease-in-out',
          'focus:outline-none focus:ring-2 focus:ring-red-300',
          'active:scale-95 hover:bg-red-700',
          buttonSizeClasses[size],
          isDisabled && 'opacity-50 cursor-not-allowed'
        )}
        onClick={handleStop}
        disabled={isDisabled}
        aria-label="Stop"
      >
        <Square size={size === 'lg' ? 20 : size === 'md' ? 16 : 14} />
        {showLabels && (
          <span className="absolute -bottom-6 text-xs font-medium text-industrial-600">
            Stop
          </span>
        )}
      </button>

      {/* Mode Indicator */}
      {currentMode && (
        <div className="absolute -top-8 left-1/2 -translate-x-1/2">
          <span className="text-xs font-medium text-industrial-600 bg-white px-2 py-1 rounded">
            {currentMode.name}
          </span>
        </div>
      )}
    </div>
  )
}
```

### **2. Speed Control Slider (src/components/control/SpeedControlSlider.tsx)**

```typescript
import React, { useState, useCallback, useEffect } from 'react'
import { Gauge, Zap, Turtle, Rabbit } from 'lucide-react'
import { useControlStore } from '@stores/controlStore'
import { cn, formatSpeed } from '@utils/helpers'
import { SYSTEM_CONSTANTS } from '@utils/constants'

interface SpeedControlSliderProps {
  className?: string
  size?: 'sm' | 'md' | 'lg'
  disabled?: boolean
  showPresets?: boolean
  showValue?: boolean
  step?: number
}

export const SpeedControlSlider: React.FC<SpeedControlSliderProps> = ({
  className,
  size = 'md',
  disabled = false,
  showPresets = true,
  showValue = true,
  step = 0.1,
}) => {
  const { sendCommand, currentMode, isEmergencyStopActive } = useControlStore()
  const [speed, setSpeed] = useState(0.5) // Default 50% of max speed
  const [isDragging, setIsDragging] = useState(false)

  const isDisabled = disabled || isEmergencyStopActive || !currentMode

  const maxSpeed = SYSTEM_CONSTANTS.MAX_SPEED
  const currentSpeed = speed * maxSpeed

  const speedPresets = [
    { label: 'Slow', value: 0.25, icon: Turtle, color: 'text-blue-600' },
    { label: 'Normal', value: 0.5, icon: Gauge, color: 'text-green-600' },
    { label: 'Fast', value: 0.75, icon: Zap, color: 'text-yellow-600' },
    { label: 'Max', value: 1.0, icon: Rabbit, color: 'text-red-600' },
  ]

  const handleSpeedChange = useCallback(async (newSpeed: number) => {
    if (isDisabled) return

    setSpeed(newSpeed)
    
    try {
      await sendCommand({
        type: 'move',
        speed: newSpeed * maxSpeed,
      })
    } catch (error) {
      console.error('Failed to send speed command:', error)
    }
  }, [isDisabled, sendCommand, maxSpeed])

  const handlePresetClick = useCallback((presetValue: number) => {
    handleSpeedChange(presetValue)
  }, [handleSpeedChange])

  const sizeClasses = {
    sm: 'h-32',
    md: 'h-40',
    lg: 'h-48',
  }

  const getSpeedColor = (speedValue: number) => {
    if (speedValue <= 0.25) return 'text-blue-600'
    if (speedValue <= 0.5) return 'text-green-600'
    if (speedValue <= 0.75) return 'text-yellow-600'
    return 'text-red-600'
  }

  return (
    <div className={cn('flex flex-col items-center gap-4', sizeClasses[size], className)}>
      {/* Speed Display */}
      {showValue && (
        <div className="text-center">
          <div className={cn('text-2xl font-bold font-mono', getSpeedColor(speed))}>
            {formatSpeed(currentSpeed)}
          </div>
          <div className="text-sm text-industrial-600">
            {Math.round(speed * 100)}% of max speed
          </div>
        </div>
      )}

      {/* Speed Slider */}
      <div className="relative w-full max-w-xs">
        <input
          type="range"
          min="0"
          max="1"
          step={step}
          value={speed}
          onChange={(e) => handleSpeedChange(parseFloat(e.target.value))}
          onMouseDown={() => setIsDragging(true)}
          onMouseUp={() => setIsDragging(false)}
          onTouchStart={() => setIsDragging(true)}
          onTouchEnd={() => setIsDragging(false)}
          disabled={isDisabled}
          className={cn(
            'w-full h-2 bg-industrial-200 rounded-lg appearance-none cursor-pointer',
            'focus:outline-none focus:ring-2 focus:ring-primary-500',
            isDisabled && 'opacity-50 cursor-not-allowed',
            'slider'
          )}
          style={{
            background: `linear-gradient(to right, #3b82f6 0%, #3b82f6 ${speed * 100}%, #e2e8f0 ${speed * 100}%, #e2e8f0 100%)`
          }}
        />
        
        {/* Speed Markers */}
        <div className="flex justify-between mt-2 text-xs text-industrial-500">
          <span>0 m/s</span>
          <span>{formatSpeed(maxSpeed * 0.25)}</span>
          <span>{formatSpeed(maxSpeed * 0.5)}</span>
          <span>{formatSpeed(maxSpeed * 0.75)}</span>
          <span>{formatSpeed(maxSpeed)}</span>
        </div>
      </div>

      {/* Speed Presets */}
      {showPresets && (
        <div className="grid grid-cols-4 gap-2 w-full max-w-xs">
          {speedPresets.map((preset) => {
            const Icon = preset.icon
            const isActive = Math.abs(speed - preset.value) < step / 2
            
            return (
              <button
                key={preset.label}
                onClick={() => handlePresetClick(preset.value)}
                disabled={isDisabled}
                className={cn(
                  'flex flex-col items-center gap-1 p-2 rounded-lg',
                  'transition-all duration-200 ease-in-out',
                  'focus:outline-none focus:ring-2 focus:ring-primary-500',
                  'hover:bg-industrial-100',
                  isActive && 'bg-primary-100 text-primary-700',
                  isDisabled && 'opacity-50 cursor-not-allowed',
                  !isActive && preset.color
                )}
                aria-label={`Set speed to ${preset.label}`}
              >
                <Icon size={16} />
                <span className="text-xs font-medium">{preset.label}</span>
              </button>
            )
          })}
        </div>
      )}

      {/* Mode Indicator */}
      {currentMode && (
        <div className="text-center">
          <span className="text-xs font-medium text-industrial-600 bg-white px-2 py-1 rounded">
            {currentMode.name} Mode
          </span>
        </div>
      )}
    </div>
  )
}
```

### **3. Mode Selector (src/components/control/ModeSelector.tsx)**

```typescript
import React, { useCallback } from 'react'
import { Settings, Shield, Zap, Wrench } from 'lucide-react'
import { useControlStore } from '@stores/controlStore'
import { cn } from '@utils/helpers'
import { ControlMode } from '@types'

interface ModeSelectorProps {
  className?: string
  size?: 'sm' | 'md' | 'lg'
  disabled?: boolean
  showDescriptions?: boolean
  layout?: 'horizontal' | 'vertical' | 'grid'
}

export const ModeSelector: React.FC<ModeSelectorProps> = ({
  className,
  size = 'md',
  disabled = false,
  showDescriptions = true,
  layout = 'horizontal',
}) => {
  const { availableModes, currentMode, setMode, isLoading } = useControlStore()

  const isDisabled = disabled || isLoading

  const handleModeSelect = useCallback(async (modeId: string) => {
    if (isDisabled || currentMode?.id === modeId) return

    try {
      await setMode(modeId)
    } catch (error) {
      console.error('Failed to set mode:', error)
    }
  }, [isDisabled, currentMode?.id, setMode])

  const getModeIcon = (mode: ControlMode) => {
    switch (mode.name.toLowerCase()) {
      case 'auto':
        return <Zap size={20} />
      case 'manual':
        return <Settings size={20} />
      case 'semi':
        return <Shield size={20} />
      case 'maint':
        return <Wrench size={20} />
      default:
        return <Settings size={20} />
    }
  }

  const getModeColor = (mode: ControlMode) => {
    switch (mode.safetyLevel) {
      case 'low':
        return 'border-green-500 bg-green-50 text-green-700'
      case 'medium':
        return 'border-yellow-500 bg-yellow-50 text-yellow-700'
      case 'high':
        return 'border-red-500 bg-red-50 text-red-700'
      default:
        return 'border-industrial-500 bg-industrial-50 text-industrial-700'
    }
  }

  const sizeClasses = {
    sm: 'text-sm',
    md: 'text-base',
    lg: 'text-lg',
  }

  const layoutClasses = {
    horizontal: 'flex gap-2',
    vertical: 'flex flex-col gap-2',
    grid: 'grid grid-cols-2 gap-2',
  }

  if (availableModes.length === 0) {
    return (
      <div className={cn('text-center text-industrial-500', sizeClasses[size], className)}>
        No control modes available
      </div>
    )
  }

  return (
    <div className={cn('w-full', layoutClasses[layout], className)}>
      {availableModes.map((mode) => {
        const Icon = getModeIcon(mode)
        const isActive = currentMode?.id === mode.id
        const isDisabled = isDisabled || !mode.allowedCommands.length

        return (
          <button
            key={mode.id}
            onClick={() => handleModeSelect(mode.id)}
            disabled={isDisabled}
            className={cn(
              'flex items-center gap-3 p-3 rounded-lg border-2 transition-all duration-200',
              'focus:outline-none focus:ring-2 focus:ring-primary-500',
              'hover:shadow-md',
              isActive && 'ring-2 ring-primary-500 shadow-lg',
              isDisabled && 'opacity-50 cursor-not-allowed',
              isActive ? getModeColor(mode) : 'border-industrial-200 bg-white hover:bg-industrial-50',
              sizeClasses[size]
            )}
            aria-label={`Select ${mode.name} mode`}
          >
            <div className="flex-shrink-0">
              {Icon}
            </div>
            
            <div className="flex-1 text-left">
              <div className="font-semibold">{mode.name}</div>
              {showDescriptions && (
                <div className="text-xs opacity-75 mt-1">
                  {mode.description}
                </div>
              )}
            </div>

            {/* Safety Level Indicator */}
            <div className="flex-shrink-0">
              <div className={cn(
                'w-2 h-2 rounded-full',
                mode.safetyLevel === 'low' && 'bg-green-500',
                mode.safetyLevel === 'medium' && 'bg-yellow-500',
                mode.safetyLevel === 'high' && 'bg-red-500'
              )} />
            </div>
          </button>
        )
      })}
    </div>
  )
}
```

### **4. Control Panel (src/components/control/ControlPanel.tsx)**

```typescript
import React, { useState } from 'react'
import { Play, Pause, RotateCcw, Home, Settings } from 'lucide-react'
import { useControlStore } from '@stores/controlStore'
import { cn } from '@utils/helpers'
import { DirectionalControlPad } from './DirectionalControlPad'
import { SpeedControlSlider } from './SpeedControlSlider'
import { ModeSelector } from './ModeSelector'
import { EmergencyStopButton } from '../safety/EmergencyStopButton'

interface ControlPanelProps {
  className?: string
  layout?: 'compact' | 'full' | 'mobile'
  showEmergencyStop?: boolean
  showModeSelector?: boolean
  showSpeedControl?: boolean
}

export const ControlPanel: React.FC<ControlPanelProps> = ({
  className,
  layout = 'full',
  showEmergencyStop = true,
  showModeSelector = true,
  showSpeedControl = true,
}) => {
  const { sendCommand, currentMode, isEmergencyStopActive } = useControlStore()
  const [activeTab, setActiveTab] = useState<'manual' | 'auto' | 'settings'>('manual')

  const handleControlAction = async (action: string) => {
    if (isEmergencyStopActive) return

    try {
      switch (action) {
        case 'play':
          await sendCommand({ type: 'move' })
          break
        case 'pause':
          await sendCommand({ type: 'stop' })
          break
        case 'reset':
          await sendCommand({ type: 'mode_change', mode: 'auto' })
          break
        case 'home':
          await sendCommand({ type: 'move', direction: 'up' })
          break
        default:
          console.warn('Unknown control action:', action)
      }
    } catch (error) {
      console.error('Failed to execute control action:', error)
    }
  }

  const layoutClasses = {
    compact: 'max-w-md',
    full: 'max-w-4xl',
    mobile: 'max-w-sm',
  }

  const isDisabled = isEmergencyStopActive || !currentMode

  return (
    <div className={cn('bg-white rounded-lg shadow-industrial border border-industrial-200 p-6', layoutClasses[layout], className)}>
      {/* Header */}
      <div className="flex items-center justify-between mb-6">
        <h3 className="text-lg font-semibold text-industrial-900">
          Control Panel
        </h3>
        
        {currentMode && (
          <div className="flex items-center gap-2">
            <span className="text-sm text-industrial-600">Mode:</span>
            <span className="text-sm font-medium text-primary-600">
              {currentMode.name}
            </span>
          </div>
        )}
      </div>

      {/* Tab Navigation */}
      <div className="flex border-b border-industrial-200 mb-6">
        {['manual', 'auto', 'settings'].map((tab) => (
          <button
            key={tab}
            onClick={() => setActiveTab(tab as any)}
            className={cn(
              'px-4 py-2 text-sm font-medium border-b-2 transition-colors',
              activeTab === tab
                ? 'border-primary-500 text-primary-600'
                : 'border-transparent text-industrial-600 hover:text-industrial-900'
            )}
          >
            {tab.charAt(0).toUpperCase() + tab.slice(1)}
          </button>
        ))}
      </div>

      {/* Manual Control Tab */}
      {activeTab === 'manual' && (
        <div className="space-y-6">
          {/* Mode Selector */}
          {showModeSelector && (
            <div>
              <h4 className="text-sm font-medium text-industrial-700 mb-3">Control Mode</h4>
              <ModeSelector size="sm" layout="horizontal" />
            </div>
          )}

          {/* Control Layout */}
          <div className="grid grid-cols-1 lg:grid-cols-3 gap-6">
            {/* Directional Control */}
            <div className="flex flex-col items-center">
              <h4 className="text-sm font-medium text-industrial-700 mb-4">Directional Control</h4>
              <DirectionalControlPad size="md" disabled={isDisabled} />
            </div>

            {/* Speed Control */}
            {showSpeedControl && (
              <div className="flex flex-col items-center">
                <h4 className="text-sm font-medium text-industrial-700 mb-4">Speed Control</h4>
                <SpeedControlSlider size="md" disabled={isDisabled} />
              </div>
            )}

            {/* Quick Actions */}
            <div className="flex flex-col items-center">
              <h4 className="text-sm font-medium text-industrial-700 mb-4">Quick Actions</h4>
              <div className="grid grid-cols-2 gap-3">
                <button
                  onClick={() => handleControlAction('play')}
                  disabled={isDisabled}
                  className="control-button flex flex-col items-center gap-1 p-3"
                >
                  <Play size={20} />
                  <span className="text-xs">Start</span>
                </button>
                
                <button
                  onClick={() => handleControlAction('pause')}
                  disabled={isDisabled}
                  className="control-button flex flex-col items-center gap-1 p-3"
                >
                  <Pause size={20} />
                  <span className="text-xs">Pause</span>
                </button>
                
                <button
                  onClick={() => handleControlAction('reset')}
                  disabled={isDisabled}
                  className="control-button flex flex-col items-center gap-1 p-3"
                >
                  <RotateCcw size={20} />
                  <span className="text-xs">Reset</span>
                </button>
                
                <button
                  onClick={() => handleControlAction('home')}
                  disabled={isDisabled}
                  className="control-button flex flex-col items-center gap-1 p-3"
                >
                  <Home size={20} />
                  <span className="text-xs">Home</span>
                </button>
              </div>
            </div>
          </div>

          {/* Emergency Stop */}
          {showEmergencyStop && (
            <div className="flex justify-center pt-4 border-t border-industrial-200">
              <div className="text-center">
                <EmergencyStopButton size="lg" />
                <p className="text-xs text-industrial-600 mt-2">
                  Emergency Stop
                </p>
              </div>
            </div>
          )}
        </div>
      )}

      {/* Auto Control Tab */}
      {activeTab === 'auto' && (
        <div className="text-center py-8">
          <Settings size={48} className="text-industrial-400 mx-auto mb-4" />
          <h4 className="text-lg font-medium text-industrial-700 mb-2">
            Automatic Control
          </h4>
          <p className="text-industrial-600">
            Automatic control features will be available in the next update.
          </p>
        </div>
      )}

      {/* Settings Tab */}
      {activeTab === 'settings' && (
        <div className="text-center py-8">
          <Settings size={48} className="text-industrial-400 mx-auto mb-4" />
          <h4 className="text-lg font-medium text-industrial-700 mb-2">
            Control Settings
          </h4>
          <p className="text-industrial-600">
            Control settings and configuration will be available here.
          </p>
        </div>
      )}
    </div>
  )
}
```

### **5. Control Components Index (src/components/control/index.ts)**

```typescript
export { DirectionalControlPad } from './DirectionalControlPad'
export { SpeedControlSlider } from './SpeedControlSlider'
export { ModeSelector } from './ModeSelector'
export { ControlPanel } from './ControlPanel'

// Re-export types
export type { ControlCommand, ControlMode } from '@types'
```

---

## 🔧 **NEXT STEPS**

### **Immediate Actions:**
1. ✅ Directional Control Pad với continuous mode
2. ✅ Speed Control Slider với presets
3. ✅ Mode Selector với safety levels
4. ✅ Control Panel với tabbed interface
5. ✅ Integration với safety components

### **Phase 2 Week 3 Day 3-4 Checklist:**
- [x] Directional Control Pad component
- [x] Speed Control Slider component
- [x] Mode Selector component
- [x] Control Panel component
- [x] Control components integration
- [x] Safety integration

### **Ready for Next Phase:**
- Data Display Components (Week 4)
- Layout Components (Week 5)

---

**Changelog:**
- v1.0: Control components implementation completed

**Status:** Ready for data display components
