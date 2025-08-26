# OHT-50 Safety Components Implementation

**Phiên bản:** v1.0  
**Ngày tạo:** 2024-08-24  
**Trạng thái:** Implementation Phase  
**Phase:** 2 - Core Components

---

## 🚨 **SAFETY-FIRST COMPONENTS**

### **1. Emergency Stop Button (src/components/safety/EmergencyStopButton.tsx)**

```typescript
import React, { useState, useCallback } from 'react'
import { AlertTriangle, Power } from 'lucide-react'
import { useControlStore } from '@stores/controlStore'
import { cn } from '@utils/helpers'
import { SAFETY_CONSTANTS } from '@utils/constants'

interface EmergencyStopButtonProps {
  className?: string
  size?: 'sm' | 'md' | 'lg'
  variant?: 'primary' | 'secondary'
  disabled?: boolean
  showConfirmation?: boolean
}

export const EmergencyStopButton: React.FC<EmergencyStopButtonProps> = ({
  className,
  size = 'lg',
  variant = 'primary',
  disabled = false,
  showConfirmation = true,
}) => {
  const { emergencyStop, isEmergencyStopActive, emergencyStopCooldown } = useControlStore()
  const [showConfirmDialog, setShowConfirmDialog] = useState(false)
  const [isPressed, setIsPressed] = useState(false)

  const handleEmergencyStop = useCallback(async () => {
    if (disabled || isEmergencyStopActive || emergencyStopCooldown) {
      return
    }

    if (showConfirmation && !showConfirmDialog) {
      setShowConfirmDialog(true)
      return
    }

    setIsPressed(true)
    
    try {
      await emergencyStop()
      
      // Visual feedback
      setTimeout(() => {
        setIsPressed(false)
        setShowConfirmDialog(false)
      }, 500)
      
    } catch (error) {
      setIsPressed(false)
      setShowConfirmDialog(false)
      console.error('Emergency stop failed:', error)
    }
  }, [disabled, isEmergencyStopActive, emergencyStopCooldown, showConfirmation, showConfirmDialog, emergencyStop])

  const handleKeyPress = useCallback((event: React.KeyboardEvent) => {
    if (event.key === 'Escape') {
      setShowConfirmDialog(false)
    } else if (event.key === 'Enter' && showConfirmDialog) {
      handleEmergencyStop()
    }
  }, [showConfirmDialog, handleEmergencyStop])

  const sizeClasses = {
    sm: 'w-16 h-16 text-sm',
    md: 'w-20 h-20 text-base',
    lg: 'w-24 h-24 text-lg',
  }

  const variantClasses = {
    primary: 'bg-safety-error hover:bg-red-700 text-white',
    secondary: 'bg-industrial-100 hover:bg-industrial-200 text-safety-error border-2 border-safety-error',
  }

  const isDisabled = disabled || isEmergencyStopActive || emergencyStopCooldown

  return (
    <>
      <button
        className={cn(
          'emergency-stop safety-button rounded-full font-bold font-mono uppercase tracking-wider',
          'flex flex-col items-center justify-center gap-1',
          'transition-all duration-200 ease-in-out',
          'focus:outline-none focus:ring-4 focus:ring-red-300',
          'active:scale-95',
          sizeClasses[size],
          variantClasses[variant],
          isDisabled && 'opacity-50 cursor-not-allowed',
          isEmergencyStopActive && 'animate-pulse-emergency',
          isPressed && 'animate-shake',
          className
        )}
        onClick={handleEmergencyStop}
        disabled={isDisabled}
        onKeyDown={handleKeyPress}
        aria-label="Emergency Stop"
        role="button"
        tabIndex={0}
      >
        <Power 
          size={size === 'lg' ? 32 : size === 'md' ? 24 : 20} 
          className={cn(
            'transition-transform duration-200',
            isEmergencyStopActive && 'animate-pulse'
          )}
        />
        <span className="text-xs font-bold">E-STOP</span>
      </button>

      {/* Confirmation Dialog */}
      {showConfirmDialog && (
        <div className="fixed inset-0 bg-black bg-opacity-50 flex items-center justify-center z-50 animate-fade-in">
          <div className="bg-white rounded-lg p-6 max-w-md mx-4 animate-slide-in-up">
            <div className="flex items-center gap-3 mb-4">
              <AlertTriangle className="text-safety-error" size={24} />
              <h3 className="text-lg font-bold text-industrial-900">
                Emergency Stop Confirmation
              </h3>
            </div>
            
            <p className="text-industrial-700 mb-6">
              Are you sure you want to activate emergency stop? This will immediately halt all system operations.
            </p>
            
            <div className="flex gap-3 justify-end">
              <button
                className="px-4 py-2 text-industrial-600 hover:text-industrial-800 transition-colors"
                onClick={() => setShowConfirmDialog(false)}
              >
                Cancel
              </button>
              <button
                className="px-4 py-2 bg-safety-error text-white rounded hover:bg-red-700 transition-colors"
                onClick={handleEmergencyStop}
              >
                Activate E-Stop
              </button>
            </div>
          </div>
        </div>
      )}

      {/* Cooldown Indicator */}
      {emergencyStopCooldown && (
        <div className="absolute -top-2 -right-2 w-6 h-6 bg-yellow-500 rounded-full flex items-center justify-center animate-pulse">
          <span className="text-xs font-bold text-white">C</span>
        </div>
      )}
    </>
  )
}
```

### **2. Alert Banner System (src/components/safety/AlertBanner.tsx)**

```typescript
import React, { useEffect, useState } from 'react'
import { X, AlertTriangle, Info, CheckCircle, XCircle } from 'lucide-react'
import { useAlertStore } from '@stores/alertStore'
import { cn, formatRelativeTime } from '@utils/helpers'
import { Alert } from '@types'

interface AlertBannerProps {
  className?: string
  maxAlerts?: number
  autoDismiss?: boolean
  dismissDelay?: number
}

export const AlertBanner: React.FC<AlertBannerProps> = ({
  className,
  maxAlerts = 3,
  autoDismiss = true,
  dismissDelay = 10000,
}) => {
  const { alerts, unacknowledgedCount, acknowledgeAlert, removeAlert } = useAlertStore()
  const [visibleAlerts, setVisibleAlerts] = useState<Alert[]>([])

  useEffect(() => {
    const unacknowledged = alerts.filter(alert => !alert.acknowledged)
    setVisibleAlerts(unacknowledged.slice(0, maxAlerts))
  }, [alerts, maxAlerts])

  useEffect(() => {
    if (!autoDismiss) return

    const timers = visibleAlerts.map(alert => {
      return setTimeout(() => {
        if (alert.type === 'info' || alert.type === 'success') {
          acknowledgeAlert(alert.id)
        }
      }, dismissDelay)
    })

    return () => {
      timers.forEach(timer => clearTimeout(timer))
    }
  }, [visibleAlerts, autoDismiss, dismissDelay, acknowledgeAlert])

  const getAlertIcon = (type: Alert['type']) => {
    switch (type) {
      case 'critical':
      case 'error':
        return <XCircle size={20} />
      case 'warning':
        return <AlertTriangle size={20} />
      case 'info':
        return <Info size={20} />
      case 'success':
        return <CheckCircle size={20} />
      default:
        return <Info size={20} />
    }
  }

  const getAlertStyles = (type: Alert['type']) => {
    switch (type) {
      case 'critical':
        return 'bg-red-600 text-white border-red-700'
      case 'error':
        return 'bg-red-500 text-white border-red-600'
      case 'warning':
        return 'bg-yellow-500 text-white border-yellow-600'
      case 'info':
        return 'bg-blue-500 text-white border-blue-600'
      case 'success':
        return 'bg-green-500 text-white border-green-600'
      default:
        return 'bg-industrial-500 text-white border-industrial-600'
    }
  }

  const handleAcknowledge = async (alertId: string) => {
    try {
      await acknowledgeAlert(alertId)
    } catch (error) {
      console.error('Failed to acknowledge alert:', error)
    }
  }

  const handleDismiss = (alertId: string) => {
    removeAlert(alertId)
  }

  if (visibleAlerts.length === 0) {
    return null
  }

  return (
    <div className={cn('fixed top-0 left-0 right-0 z-40 animate-slide-down', className)}>
      <div className="max-w-7xl mx-auto p-4 space-y-2">
        {visibleAlerts.map((alert) => (
          <div
            key={alert.id}
            className={cn(
              'flex items-start gap-3 p-4 rounded-lg border-l-4 shadow-lg',
              'animate-slide-in-up',
              getAlertStyles(alert.type)
            )}
          >
            <div className="flex-shrink-0 mt-0.5">
              {getAlertIcon(alert.type)}
            </div>
            
            <div className="flex-1 min-w-0">
              <div className="flex items-center justify-between">
                <h4 className="text-sm font-semibold">
                  {alert.title}
                </h4>
                <div className="flex items-center gap-2">
                  <span className="text-xs opacity-75">
                    {formatRelativeTime(alert.timestamp)}
                  </span>
                  <button
                    onClick={() => handleDismiss(alert.id)}
                    className="opacity-75 hover:opacity-100 transition-opacity"
                    aria-label="Dismiss alert"
                  >
                    <X size={16} />
                  </button>
                </div>
              </div>
              
              <p className="text-sm mt-1 opacity-90">
                {alert.message}
              </p>
              
              <div className="flex items-center gap-4 mt-2 text-xs opacity-75">
                <span>Source: {alert.source}</span>
                <span>Category: {alert.category}</span>
              </div>
            </div>
            
            {!alert.acknowledged && (
              <button
                onClick={() => handleAcknowledge(alert.id)}
                className="px-3 py-1 text-xs bg-white bg-opacity-20 rounded hover:bg-opacity-30 transition-colors"
              >
                Acknowledge
              </button>
            )}
          </div>
        ))}
        
        {unacknowledgedCount > maxAlerts && (
          <div className="text-center text-sm text-industrial-600 bg-white bg-opacity-90 rounded-lg p-2">
            +{unacknowledgedCount - maxAlerts} more alerts
          </div>
        )}
      </div>
    </div>
  )
}
```

### **3. Status Indicators (src/components/safety/StatusIndicator.tsx)**

```typescript
import React from 'react'
import { cn, getStatusColor } from '@utils/helpers'
import { SystemStatus } from '@types'

interface StatusIndicatorProps {
  status: SystemStatus['status']
  size?: 'sm' | 'md' | 'lg'
  showLabel?: boolean
  showIcon?: boolean
  className?: string
}

export const StatusIndicator: React.FC<StatusIndicatorProps> = ({
  status,
  size = 'md',
  showLabel = true,
  showIcon = true,
  className,
}) => {
  const getStatusIcon = (status: SystemStatus['status']) => {
    switch (status) {
      case 'idle':
        return '⏸️'
      case 'moving':
        return '▶️'
      case 'docking':
        return '🎯'
      case 'fault':
        return '⚠️'
      case 'estop':
        return '🛑'
      default:
        return '❓'
    }
  }

  const getStatusLabel = (status: SystemStatus['status']) => {
    switch (status) {
      case 'idle':
        return 'Idle'
      case 'moving':
        return 'Moving'
      case 'docking':
        return 'Docking'
      case 'fault':
        return 'Fault'
      case 'estop':
        return 'E-Stop'
      default:
        return 'Unknown'
    }
  }

  const sizeClasses = {
    sm: 'px-2 py-1 text-xs',
    md: 'px-3 py-1.5 text-sm',
    lg: 'px-4 py-2 text-base',
  }

  const iconSize = {
    sm: 'text-xs',
    md: 'text-sm',
    lg: 'text-base',
  }

  return (
    <div
      className={cn(
        'status-indicator inline-flex items-center gap-2',
        getStatusColor(status),
        sizeClasses[size],
        status === 'estop' && 'animate-pulse-emergency',
        status === 'fault' && 'animate-shake',
        className
      )}
    >
      {showIcon && (
        <span className={cn('flex-shrink-0', iconSize[size])}>
          {getStatusIcon(status)}
        </span>
      )}
      
      {showLabel && (
        <span className="font-medium">
          {getStatusLabel(status)}
        </span>
      )}
    </div>
  )
}
```

### **4. Safety Confirmation Dialog (src/components/safety/SafetyConfirmationDialog.tsx)**

```typescript
import React, { useEffect, useRef } from 'react'
import { AlertTriangle, Shield, Clock } from 'lucide-react'
import { cn } from '@utils/helpers'
import { SAFETY_CONSTANTS } from '@utils/constants'

interface SafetyConfirmationDialogProps {
  isOpen: boolean
  onConfirm: () => void
  onCancel: () => void
  title: string
  message: string
  type: 'warning' | 'danger' | 'critical'
  timeout?: number
  requireTyping?: string
  className?: string
}

export const SafetyConfirmationDialog: React.FC<SafetyConfirmationDialogProps> = ({
  isOpen,
  onConfirm,
  onCancel,
  title,
  message,
  type,
  timeout = SAFETY_CONSTANTS.SAFETY_CONFIRMATION_TIMEOUT,
  requireTyping,
  className,
}) => {
  const [timeLeft, setTimeLeft] = React.useState(timeout / 1000)
  const [typedText, setTypedText] = React.useState('')
  const [canConfirm, setCanConfirm] = React.useState(false)
  const timeoutRef = useRef<NodeJS.Timeout>()
  const intervalRef = useRef<NodeJS.Timeout>()

  useEffect(() => {
    if (!isOpen) {
      setTimeLeft(timeout / 1000)
      setTypedText('')
      setCanConfirm(false)
      return
    }

    // Start countdown
    intervalRef.current = setInterval(() => {
      setTimeLeft(prev => {
        if (prev <= 1) {
          onCancel()
          return 0
        }
        return prev - 1
      })
    }, 1000)

    // Auto-cancel after timeout
    timeoutRef.current = setTimeout(() => {
      onCancel()
    }, timeout)

    return () => {
      if (timeoutRef.current) clearTimeout(timeoutRef.current)
      if (intervalRef.current) clearInterval(intervalRef.current)
    }
  }, [isOpen, timeout, onCancel])

  useEffect(() => {
    if (requireTyping) {
      setCanConfirm(typedText.toLowerCase() === requireTyping.toLowerCase())
    } else {
      setCanConfirm(true)
    }
  }, [typedText, requireTyping])

  const getTypeStyles = () => {
    switch (type) {
      case 'warning':
        return 'border-yellow-500 bg-yellow-50'
      case 'danger':
        return 'border-red-500 bg-red-50'
      case 'critical':
        return 'border-red-600 bg-red-100'
      default:
        return 'border-industrial-500 bg-industrial-50'
    }
  }

  const getTypeIcon = () => {
    switch (type) {
      case 'warning':
        return <AlertTriangle className="text-yellow-600" size={24} />
      case 'danger':
        return <Shield className="text-red-600" size={24} />
      case 'critical':
        return <AlertTriangle className="text-red-700" size={24} />
      default:
        return <Shield className="text-industrial-600" size={24} />
    }
  }

  if (!isOpen) return null

  return (
    <div className="fixed inset-0 bg-black bg-opacity-50 flex items-center justify-center z-50 animate-fade-in">
      <div
        className={cn(
          'bg-white rounded-lg shadow-xl max-w-md mx-4 w-full animate-slide-in-up',
          getTypeStyles(),
          className
        )}
      >
        <div className="p-6">
          <div className="flex items-center gap-3 mb-4">
            {getTypeIcon()}
            <h3 className="text-lg font-bold text-industrial-900">
              {title}
            </h3>
          </div>
          
          <p className="text-industrial-700 mb-6">
            {message}
          </p>
          
          {requireTyping && (
            <div className="mb-4">
              <label className="block text-sm font-medium text-industrial-700 mb-2">
                Type "{requireTyping}" to confirm:
              </label>
              <input
                type="text"
                value={typedText}
                onChange={(e) => setTypedText(e.target.value)}
                className="w-full px-3 py-2 border border-industrial-300 rounded-md focus:outline-none focus:ring-2 focus:ring-primary-500"
                placeholder={requireTyping}
                autoFocus
              />
            </div>
          )}
          
          <div className="flex items-center justify-between mb-6">
            <div className="flex items-center gap-2 text-sm text-industrial-600">
              <Clock size={16} />
              <span>Time remaining: {timeLeft}s</span>
            </div>
          </div>
          
          <div className="flex gap-3 justify-end">
            <button
              onClick={onCancel}
              className="px-4 py-2 text-industrial-600 hover:text-industrial-800 transition-colors"
            >
              Cancel
            </button>
            <button
              onClick={onConfirm}
              disabled={!canConfirm}
              className={cn(
                'px-4 py-2 rounded transition-colors',
                canConfirm
                  ? 'bg-safety-error text-white hover:bg-red-700'
                  : 'bg-industrial-200 text-industrial-400 cursor-not-allowed'
              )}
            >
              Confirm
            </button>
          </div>
        </div>
      </div>
    </div>
  )
}
```

### **5. Safety Zone Indicator (src/components/safety/SafetyZoneIndicator.tsx)**

```typescript
import React from 'react'
import { MapPin, AlertTriangle, Shield } from 'lucide-react'
import { cn } from '@utils/helpers'
import { SafetyZone } from '@types'

interface SafetyZoneIndicatorProps {
  zone: SafetyZone
  isActive: boolean
  className?: string
}

export const SafetyZoneIndicator: React.FC<SafetyZoneIndicatorProps> = ({
  zone,
  isActive,
  className,
}) => {
  const getZoneIcon = (type: SafetyZone['type']) => {
    switch (type) {
      case 'restricted':
        return <AlertTriangle size={16} />
      case 'warning':
        return <AlertTriangle size={16} />
      case 'safe':
        return <Shield size={16} />
      default:
        return <MapPin size={16} />
    }
  }

  const getZoneStyles = (type: SafetyZone['type']) => {
    switch (type) {
      case 'restricted':
        return 'bg-red-100 text-red-800 border-red-300'
      case 'warning':
        return 'bg-yellow-100 text-yellow-800 border-yellow-300'
      case 'safe':
        return 'bg-green-100 text-green-800 border-green-300'
      default:
        return 'bg-industrial-100 text-industrial-800 border-industrial-300'
    }
  }

  return (
    <div
      className={cn(
        'flex items-center gap-2 px-3 py-2 rounded-lg border',
        getZoneStyles(zone.type),
        isActive && 'ring-2 ring-offset-2 ring-current',
        className
      )}
    >
      {getZoneIcon(zone.type)}
      <span className="text-sm font-medium">
        {zone.name}
      </span>
      <span className="text-xs opacity-75">
        ({zone.type})
      </span>
    </div>
  )
}
```

### **6. Safety Components Index (src/components/safety/index.ts)**

```typescript
export { EmergencyStopButton } from './EmergencyStopButton'
export { AlertBanner } from './AlertBanner'
export { StatusIndicator } from './StatusIndicator'
export { SafetyConfirmationDialog } from './SafetyConfirmationDialog'
export { SafetyZoneIndicator } from './SafetyZoneIndicator'

// Re-export types
export type { Alert } from '@types'
```

---

## 🔧 **NEXT STEPS**

### **Immediate Actions:**
1. ✅ Emergency Stop Button với confirmation dialog
2. ✅ Alert Banner system với auto-dismiss
3. ✅ Status Indicators với animations
4. ✅ Safety Confirmation Dialog với timeout
5. ✅ Safety Zone Indicator

### **Phase 2 Week 3 Day 1-2 Checklist:**
- [x] Emergency Stop Button component
- [x] Alert Banner system
- [x] Status Indicators
- [x] Safety Confirmation Dialog
- [x] Safety Zone Indicator
- [x] Safety components integration

### **Ready for Next Phase:**
- Control Components (Day 3-4)
- Data Display Components (Week 4)

---

**Changelog:**
- v1.0: Safety components implementation completed

**Status:** Ready for control components
