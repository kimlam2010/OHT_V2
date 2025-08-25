# OHT-50 Telemetry Charts Implementation

**Phiên bản:** v1.0  
**Ngày tạo:** 2024-08-24  
**Trạng thái:** Implementation Phase  
**Phase:** 3 - Data Display Components

---

## 📊 **TELEMETRY CHARTS WITH CHART.JS**

### **1. Base Chart Component (src/components/charts/BaseChart.tsx)**

```typescript
import React, { useRef, useEffect, useCallback } from 'react'
import { Chart, ChartConfiguration, ChartType } from 'chart.js/auto'
import { cn } from '@utils/helpers'

interface BaseChartProps {
  type: ChartType
  data: any
  options?: any
  className?: string
  height?: number
  width?: number
  onChartReady?: (chart: Chart) => void
}

export const BaseChart: React.FC<BaseChartProps> = ({
  type,
  data,
  options = {},
  className,
  height = 300,
  width,
  onChartReady,
}) => {
  const canvasRef = useRef<HTMLCanvasElement>(null)
  const chartRef = useRef<Chart | null>(null)

  const createChart = useCallback(() => {
    if (!canvasRef.current) return

    // Destroy existing chart
    if (chartRef.current) {
      chartRef.current.destroy()
    }

    const ctx = canvasRef.current.getContext('2d')
    if (!ctx) return

    const config: ChartConfiguration = {
      type,
      data,
      options: {
        responsive: true,
        maintainAspectRatio: false,
        animation: {
          duration: 300,
        },
        plugins: {
          legend: {
            position: 'top' as const,
            labels: {
              usePointStyle: true,
              padding: 20,
              font: {
                family: 'Inter, sans-serif',
                size: 12,
              },
            },
          },
          tooltip: {
            backgroundColor: 'rgba(0, 0, 0, 0.8)',
            titleColor: 'white',
            bodyColor: 'white',
            borderColor: 'rgba(255, 255, 255, 0.1)',
            borderWidth: 1,
            cornerRadius: 8,
            displayColors: true,
            mode: 'index' as const,
            intersect: false,
          },
        },
        ...options,
      },
    }

    chartRef.current = new Chart(ctx, config)
    
    if (onChartReady) {
      onChartReady(chartRef.current)
    }
  }, [type, data, options, onChartReady])

  useEffect(() => {
    createChart()
  }, [createChart])

  useEffect(() => {
    return () => {
      if (chartRef.current) {
        chartRef.current.destroy()
      }
    }
  }, [])

  return (
    <div className={cn('relative', className)} style={{ height, width }}>
      <canvas ref={canvasRef} />
    </div>
  )
}
```

### **2. Velocity-Acceleration-Position Chart (src/components/charts/VAPChart.tsx)**

```typescript
import React, { useMemo, useCallback } from 'react'
import { Line } from 'react-chartjs-2'
import { useTelemetryStore } from '@stores/telemetryStore'
import { cn, formatSpeed, formatAcceleration, formatPosition } from '@utils/helpers'
import { TelemetryData } from '@types'

interface VAPChartProps {
  className?: string
  height?: number
  timeRange?: number // seconds
  showVelocity?: boolean
  showAcceleration?: boolean
  showPosition?: boolean
  realTime?: boolean
}

export const VAPChart: React.FC<VAPChartProps> = ({
  className,
  height = 400,
  timeRange = 60, // 1 minute
  showVelocity = true,
  showAcceleration = true,
  showPosition = true,
  realTime = true,
}) => {
  const { telemetryData, isLoading } = useTelemetryStore()

  const chartData = useMemo(() => {
    if (!telemetryData.length) return { labels: [], datasets: [] }

    const now = Date.now()
    const filteredData = telemetryData.filter(
      (data) => now - data.timestamp <= timeRange * 1000
    )

    const labels = filteredData.map((data) => {
      const date = new Date(data.timestamp)
      return date.toLocaleTimeString('en-US', {
        hour12: false,
        hour: '2-digit',
        minute: '2-digit',
        second: '2-digit',
      })
    })

    const datasets = []

    if (showVelocity) {
      datasets.push({
        label: 'Velocity (m/s)',
        data: filteredData.map((data) => data.velocity),
        borderColor: 'rgb(59, 130, 246)',
        backgroundColor: 'rgba(59, 130, 246, 0.1)',
        borderWidth: 2,
        fill: false,
        tension: 0.4,
        yAxisID: 'y-velocity',
      })
    }

    if (showAcceleration) {
      datasets.push({
        label: 'Acceleration (m/s²)',
        data: filteredData.map((data) => data.acceleration),
        borderColor: 'rgb(239, 68, 68)',
        backgroundColor: 'rgba(239, 68, 68, 0.1)',
        borderWidth: 2,
        fill: false,
        tension: 0.4,
        yAxisID: 'y-acceleration',
      })
    }

    if (showPosition) {
      datasets.push({
        label: 'Position (m)',
        data: filteredData.map((data) => data.position),
        borderColor: 'rgb(34, 197, 94)',
        backgroundColor: 'rgba(34, 197, 94, 0.1)',
        borderWidth: 2,
        fill: false,
        tension: 0.4,
        yAxisID: 'y-position',
      })
    }

    return { labels, datasets }
  }, [telemetryData, timeRange, showVelocity, showAcceleration, showPosition])

  const chartOptions = useMemo(() => ({
    responsive: true,
    maintainAspectRatio: false,
    interaction: {
      mode: 'index' as const,
      intersect: false,
    },
    plugins: {
      title: {
        display: true,
        text: 'Velocity-Acceleration-Position Chart',
        font: {
          size: 16,
          weight: 'bold',
        },
        color: '#374151',
      },
      legend: {
        position: 'top' as const,
        labels: {
          usePointStyle: true,
          padding: 20,
        },
      },
      tooltip: {
        callbacks: {
          label: (context: any) => {
            const label = context.dataset.label || ''
            const value = context.parsed.y
            let formattedValue = value.toFixed(2)
            
            if (label.includes('Velocity')) {
              formattedValue = formatSpeed(value)
            } else if (label.includes('Acceleration')) {
              formattedValue = formatAcceleration(value)
            } else if (label.includes('Position')) {
              formattedValue = formatPosition(value)
            }
            
            return `${label}: ${formattedValue}`
          },
        },
      },
    },
    scales: {
      x: {
        type: 'time' as const,
        time: {
          unit: 'second' as const,
          displayFormats: {
            second: 'HH:mm:ss',
          },
        },
        title: {
          display: true,
          text: 'Time',
          color: '#6B7280',
        },
        grid: {
          color: 'rgba(107, 114, 128, 0.1)',
        },
      },
      'y-velocity': {
        type: 'linear' as const,
        display: showVelocity,
        position: 'left' as const,
        title: {
          display: true,
          text: 'Velocity (m/s)',
          color: '#3B82F6',
        },
        grid: {
          color: 'rgba(59, 130, 246, 0.1)',
        },
        ticks: {
          color: '#3B82F6',
          callback: (value: any) => formatSpeed(value),
        },
      },
      'y-acceleration': {
        type: 'linear' as const,
        display: showAcceleration,
        position: 'right' as const,
        title: {
          display: true,
          text: 'Acceleration (m/s²)',
          color: '#EF4444',
        },
        grid: {
          drawOnChartArea: false,
        },
        ticks: {
          color: '#EF4444',
          callback: (value: any) => formatAcceleration(value),
        },
      },
      'y-position': {
        type: 'linear' as const,
        display: showPosition,
        position: 'right' as const,
        title: {
          display: true,
          text: 'Position (m)',
          color: '#22C55E',
        },
        grid: {
          drawOnChartArea: false,
        },
        ticks: {
          color: '#22C55E',
          callback: (value: any) => formatPosition(value),
        },
      },
    },
  }), [showVelocity, showAcceleration, showPosition])

  if (isLoading) {
    return (
      <div className={cn('flex items-center justify-center', className)} style={{ height }}>
        <div className="text-center">
          <div className="animate-spin rounded-full h-8 w-8 border-b-2 border-primary-600 mx-auto mb-2"></div>
          <p className="text-sm text-industrial-600">Loading telemetry data...</p>
        </div>
      </div>
    )
  }

  return (
    <div className={cn('bg-white rounded-lg shadow-industrial border border-industrial-200 p-4', className)}>
      <Line data={chartData} options={chartOptions} height={height} />
    </div>
  )
}
```

### **3. Battery Monitoring Chart (src/components/charts/BatteryChart.tsx)**

```typescript
import React, { useMemo } from 'react'
import { Doughnut, Line } from 'react-chartjs-2'
import { Battery, Zap, AlertTriangle } from 'lucide-react'
import { useTelemetryStore } from '@stores/telemetryStore'
import { cn, formatBatteryLevel } from '@utils/helpers'

interface BatteryChartProps {
  className?: string
  height?: number
  showHistory?: boolean
  showTemperature?: boolean
  timeRange?: number
}

export const BatteryChart: React.FC<BatteryChartProps> = ({
  className,
  height = 300,
  showHistory = true,
  showTemperature = true,
  timeRange = 3600, // 1 hour
}) => {
  const { telemetryData, isLoading } = useTelemetryStore()

  const currentBattery = useMemo(() => {
    if (!telemetryData.length) return null
    return telemetryData[telemetryData.length - 1]
  }, [telemetryData])

  const batteryHistory = useMemo(() => {
    if (!telemetryData.length) return { labels: [], datasets: [] }

    const now = Date.now()
    const filteredData = telemetryData.filter(
      (data) => now - data.timestamp <= timeRange * 1000
    )

    const labels = filteredData.map((data) => {
      const date = new Date(data.timestamp)
      return date.toLocaleTimeString('en-US', {
        hour12: false,
        hour: '2-digit',
        minute: '2-digit',
      })
    })

    const datasets = [
      {
        label: 'Battery Level (%)',
        data: filteredData.map((data) => data.batteryLevel * 100),
        borderColor: 'rgb(34, 197, 94)',
        backgroundColor: 'rgba(34, 197, 94, 0.1)',
        borderWidth: 2,
        fill: true,
        tension: 0.4,
      },
    ]

    if (showTemperature) {
      datasets.push({
        label: 'Temperature (°C)',
        data: filteredData.map((data) => data.temperature),
        borderColor: 'rgb(239, 68, 68)',
        backgroundColor: 'rgba(239, 68, 68, 0.1)',
        borderWidth: 2,
        fill: false,
        tension: 0.4,
        yAxisID: 'y-temperature',
      })
    }

    return { labels, datasets }
  }, [telemetryData, timeRange, showTemperature])

  const getBatteryColor = (level: number) => {
    if (level >= 0.6) return '#22C55E' // Green
    if (level >= 0.3) return '#F59E0B' // Yellow
    return '#EF4444' // Red
  }

  const getBatteryStatus = (level: number) => {
    if (level >= 0.8) return 'Excellent'
    if (level >= 0.6) return 'Good'
    if (level >= 0.4) return 'Fair'
    if (level >= 0.2) return 'Low'
    return 'Critical'
  }

  if (isLoading) {
    return (
      <div className={cn('flex items-center justify-center', className)} style={{ height }}>
        <div className="text-center">
          <div className="animate-spin rounded-full h-8 w-8 border-b-2 border-primary-600 mx-auto mb-2"></div>
          <p className="text-sm text-industrial-600">Loading battery data...</p>
        </div>
      </div>
    )
  }

  return (
    <div className={cn('bg-white rounded-lg shadow-industrial border border-industrial-200 p-4', className)}>
      <div className="grid grid-cols-1 lg:grid-cols-2 gap-6">
        {/* Current Battery Status */}
        <div className="space-y-4">
          <h3 className="text-lg font-semibold text-industrial-900 flex items-center gap-2">
            <Battery size={20} />
            Battery Status
          </h3>
          
          {currentBattery && (
            <div className="space-y-4">
              {/* Battery Level Doughnut */}
              <div className="relative w-32 h-32 mx-auto">
                <Doughnut
                  data={{
                    labels: ['Battery', 'Empty'],
                    datasets: [{
                      data: [
                        currentBattery.batteryLevel * 100,
                        100 - currentBattery.batteryLevel * 100
                      ],
                      backgroundColor: [
                        getBatteryColor(currentBattery.batteryLevel),
                        '#E5E7EB'
                      ],
                      borderWidth: 0,
                    }]
                  }}
                  options={{
                    responsive: true,
                    maintainAspectRatio: false,
                    plugins: {
                      legend: { display: false },
                      tooltip: { enabled: false },
                    },
                    cutout: '70%',
                  }}
                />
                <div className="absolute inset-0 flex items-center justify-center">
                  <div className="text-center">
                    <div className="text-2xl font-bold" style={{ color: getBatteryColor(currentBattery.batteryLevel) }}>
                      {Math.round(currentBattery.batteryLevel * 100)}%
                    </div>
                    <div className="text-xs text-industrial-600">
                      {getBatteryStatus(currentBattery.batteryLevel)}
                    </div>
                  </div>
                </div>
              </div>

              {/* Battery Details */}
              <div className="space-y-2">
                <div className="flex justify-between items-center">
                  <span className="text-sm text-industrial-600">Voltage:</span>
                  <span className="text-sm font-medium">{currentBattery.voltage?.toFixed(1)}V</span>
                </div>
                <div className="flex justify-between items-center">
                  <span className="text-sm text-industrial-600">Current:</span>
                  <span className="text-sm font-medium">{currentBattery.current?.toFixed(2)}A</span>
                </div>
                <div className="flex justify-between items-center">
                  <span className="text-sm text-industrial-600">Temperature:</span>
                  <span className="text-sm font-medium">{currentBattery.temperature?.toFixed(1)}°C</span>
                </div>
                <div className="flex justify-between items-center">
                  <span className="text-sm text-industrial-600">Status:</span>
                  <span 
                    className="text-sm font-medium px-2 py-1 rounded"
                    style={{ 
                      backgroundColor: getBatteryColor(currentBattery.batteryLevel) + '20',
                      color: getBatteryColor(currentBattery.batteryLevel)
                    }}
                  >
                    {getBatteryStatus(currentBattery.batteryLevel)}
                  </span>
                </div>
              </div>
            </div>
          )}
        </div>

        {/* Battery History */}
        {showHistory && (
          <div className="space-y-4">
            <h3 className="text-lg font-semibold text-industrial-900 flex items-center gap-2">
              <Zap size={20} />
              Battery History
            </h3>
            
            <Line
              data={batteryHistory}
              options={{
                responsive: true,
                maintainAspectRatio: false,
                plugins: {
                  legend: {
                    position: 'top' as const,
                    labels: { usePointStyle: true },
                  },
                  tooltip: {
                    callbacks: {
                      label: (context: any) => {
                        const label = context.dataset.label || ''
                        const value = context.parsed.y
                        if (label.includes('Battery')) {
                          return `${label}: ${value.toFixed(1)}%`
                        }
                        if (label.includes('Temperature')) {
                          return `${label}: ${value.toFixed(1)}°C`
                        }
                        return `${label}: ${value}`
                      },
                    },
                  },
                },
                scales: {
                  x: {
                    title: { display: true, text: 'Time' },
                    grid: { color: 'rgba(107, 114, 128, 0.1)' },
                  },
                  y: {
                    title: { display: true, text: 'Battery Level (%)' },
                    grid: { color: 'rgba(107, 114, 128, 0.1)' },
                    min: 0,
                    max: 100,
                  },
                  'y-temperature': {
                    type: 'linear' as const,
                    display: showTemperature,
                    position: 'right' as const,
                    title: { display: true, text: 'Temperature (°C)' },
                    grid: { drawOnChartArea: false },
                  },
                },
              }}
              height={height - 100}
            />
          </div>
        )}
      </div>
    </div>
  )
}
```

### **4. System Performance Chart (src/components/charts/SystemPerformanceChart.tsx)**

```typescript
import React, { useMemo } from 'react'
import { Bar, Line } from 'react-chartjs-2'
import { Activity, Cpu, Memory, HardDrive } from 'lucide-react'
import { useTelemetryStore } from '@stores/telemetryStore'
import { cn } from '@utils/helpers'

interface SystemPerformanceChartProps {
  className?: string
  height?: number
  showCPU?: boolean
  showMemory?: boolean
  showDisk?: boolean
  showNetwork?: boolean
  timeRange?: number
}

export const SystemPerformanceChart: React.FC<SystemPerformanceChartProps> = ({
  className,
  height = 300,
  showCPU = true,
  showMemory = true,
  showDisk = true,
  showNetwork = true,
  timeRange = 300, // 5 minutes
}) => {
  const { telemetryData, isLoading } = useTelemetryStore()

  const performanceData = useMemo(() => {
    if (!telemetryData.length) return { labels: [], datasets: [] }

    const now = Date.now()
    const filteredData = telemetryData.filter(
      (data) => now - data.timestamp <= timeRange * 1000
    )

    const labels = filteredData.map((data) => {
      const date = new Date(data.timestamp)
      return date.toLocaleTimeString('en-US', {
        hour12: false,
        minute: '2-digit',
        second: '2-digit',
      })
    })

    const datasets = []

    if (showCPU && filteredData[0]?.cpuUsage !== undefined) {
      datasets.push({
        label: 'CPU Usage (%)',
        data: filteredData.map((data) => data.cpuUsage * 100),
        borderColor: 'rgb(59, 130, 246)',
        backgroundColor: 'rgba(59, 130, 246, 0.1)',
        borderWidth: 2,
        fill: true,
        tension: 0.4,
      })
    }

    if (showMemory && filteredData[0]?.memoryUsage !== undefined) {
      datasets.push({
        label: 'Memory Usage (%)',
        data: filteredData.map((data) => data.memoryUsage * 100),
        borderColor: 'rgb(239, 68, 68)',
        backgroundColor: 'rgba(239, 68, 68, 0.1)',
        borderWidth: 2,
        fill: true,
        tension: 0.4,
      })
    }

    if (showDisk && filteredData[0]?.diskUsage !== undefined) {
      datasets.push({
        label: 'Disk Usage (%)',
        data: filteredData.map((data) => data.diskUsage * 100),
        borderColor: 'rgb(34, 197, 94)',
        backgroundColor: 'rgba(34, 197, 94, 0.1)',
        borderWidth: 2,
        fill: true,
        tension: 0.4,
      })
    }

    if (showNetwork && filteredData[0]?.networkUsage !== undefined) {
      datasets.push({
        label: 'Network Usage (Mbps)',
        data: filteredData.map((data) => data.networkUsage),
        borderColor: 'rgb(168, 85, 247)',
        backgroundColor: 'rgba(168, 85, 247, 0.1)',
        borderWidth: 2,
        fill: true,
        tension: 0.4,
        yAxisID: 'y-network',
      })
    }

    return { labels, datasets }
  }, [telemetryData, timeRange, showCPU, showMemory, showDisk, showNetwork])

  const currentMetrics = useMemo(() => {
    if (!telemetryData.length) return null
    return telemetryData[telemetryData.length - 1]
  }, [telemetryData])

  const getUsageColor = (usage: number) => {
    if (usage >= 0.8) return '#EF4444' // Red
    if (usage >= 0.6) return '#F59E0B' // Yellow
    return '#22C55E' // Green
  }

  if (isLoading) {
    return (
      <div className={cn('flex items-center justify-center', className)} style={{ height }}>
        <div className="text-center">
          <div className="animate-spin rounded-full h-8 w-8 border-b-2 border-primary-600 mx-auto mb-2"></div>
          <p className="text-sm text-industrial-600">Loading performance data...</p>
        </div>
      </div>
    )
  }

  return (
    <div className={cn('bg-white rounded-lg shadow-industrial border border-industrial-200 p-4', className)}>
      <div className="space-y-6">
        {/* Current Metrics */}
        {currentMetrics && (
          <div className="grid grid-cols-2 lg:grid-cols-4 gap-4">
            {showCPU && currentMetrics.cpuUsage !== undefined && (
              <div className="text-center p-3 bg-industrial-50 rounded-lg">
                <Cpu size={24} className="mx-auto mb-2 text-blue-600" />
                <div className="text-lg font-bold" style={{ color: getUsageColor(currentMetrics.cpuUsage) }}>
                  {(currentMetrics.cpuUsage * 100).toFixed(1)}%
                </div>
                <div className="text-xs text-industrial-600">CPU Usage</div>
              </div>
            )}
            
            {showMemory && currentMetrics.memoryUsage !== undefined && (
              <div className="text-center p-3 bg-industrial-50 rounded-lg">
                <Memory size={24} className="mx-auto mb-2 text-red-600" />
                <div className="text-lg font-bold" style={{ color: getUsageColor(currentMetrics.memoryUsage) }}>
                  {(currentMetrics.memoryUsage * 100).toFixed(1)}%
                </div>
                <div className="text-xs text-industrial-600">Memory Usage</div>
              </div>
            )}
            
            {showDisk && currentMetrics.diskUsage !== undefined && (
              <div className="text-center p-3 bg-industrial-50 rounded-lg">
                <HardDrive size={24} className="mx-auto mb-2 text-green-600" />
                <div className="text-lg font-bold" style={{ color: getUsageColor(currentMetrics.diskUsage) }}>
                  {(currentMetrics.diskUsage * 100).toFixed(1)}%
                </div>
                <div className="text-xs text-industrial-600">Disk Usage</div>
              </div>
            )}
            
            {showNetwork && currentMetrics.networkUsage !== undefined && (
              <div className="text-center p-3 bg-industrial-50 rounded-lg">
                <Activity size={24} className="mx-auto mb-2 text-purple-600" />
                <div className="text-lg font-bold text-purple-600">
                  {currentMetrics.networkUsage.toFixed(1)}
                </div>
                <div className="text-xs text-industrial-600">Network (Mbps)</div>
              </div>
            )}
          </div>
        )}

        {/* Performance Chart */}
        <div>
          <h3 className="text-lg font-semibold text-industrial-900 mb-4 flex items-center gap-2">
            <Activity size={20} />
            System Performance
          </h3>
          
          <Line
            data={performanceData}
            options={{
              responsive: true,
              maintainAspectRatio: false,
              plugins: {
                legend: {
                  position: 'top' as const,
                  labels: { usePointStyle: true },
                },
                tooltip: {
                  callbacks: {
                    label: (context: any) => {
                      const label = context.dataset.label || ''
                      const value = context.parsed.y
                      if (label.includes('Usage (%)')) {
                        return `${label}: ${value.toFixed(1)}%`
                      }
                      if (label.includes('Network')) {
                        return `${label}: ${value.toFixed(1)} Mbps`
                      }
                      return `${label}: ${value}`
                    },
                  },
                },
              },
              scales: {
                x: {
                  title: { display: true, text: 'Time' },
                  grid: { color: 'rgba(107, 114, 128, 0.1)' },
                },
                y: {
                  title: { display: true, text: 'Usage (%)' },
                  grid: { color: 'rgba(107, 114, 128, 0.1)' },
                  min: 0,
                  max: 100,
                },
                'y-network': {
                  type: 'linear' as const,
                  display: showNetwork,
                  position: 'right' as const,
                  title: { display: true, text: 'Network (Mbps)' },
                  grid: { drawOnChartArea: false },
                },
              },
            }}
            height={height - 200}
          />
        </div>
      </div>
    </div>
  )
}
```

### **5. Charts Index (src/components/charts/index.ts)**

```typescript
export { BaseChart } from './BaseChart'
export { VAPChart } from './VAPChart'
export { BatteryChart } from './BatteryChart'
export { SystemPerformanceChart } from './SystemPerformanceChart'

// Re-export Chart.js types
export type { ChartConfiguration, ChartType } from 'chart.js/auto'
```

---

## 🔧 **NEXT STEPS**

### **Immediate Actions:**
1. ✅ Base Chart component với Chart.js integration
2. ✅ VAP Chart cho velocity-acceleration-position
3. ✅ Battery Chart với real-time monitoring
4. ✅ System Performance Chart
5. ✅ Charts integration với telemetry store

### **Phase 3 Week 4 Day 1-2 Checklist:**
- [x] Base Chart component
- [x] VAP Chart component
- [x] Battery Chart component
- [x] System Performance Chart component
- [x] Charts integration

### **Ready for Next Phase:**
- Data Tables (Day 3-4)
- Status Cards (Week 5)
- Log Viewers (Week 6)

---

**Changelog:**
- v1.0: Telemetry charts implementation completed

**Status:** Ready for data tables
