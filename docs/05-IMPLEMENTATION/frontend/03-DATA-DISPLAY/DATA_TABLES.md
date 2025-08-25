# OHT-50 Data Tables Implementation

**Phiên bản:** v1.0  
**Ngày tạo:** 2024-08-24  
**Trạng thái:** Implementation Phase  
**Phase:** 3 - Data Display Components

---

## 📊 **DATA TABLES WITH SORTING & FILTERING**

### **1. Base Data Table Component (src/components/tables/BaseDataTable.tsx)**

```typescript
import React, { useState, useMemo, useCallback } from 'react'
import { ChevronUp, ChevronDown, Search, Filter, Download, RefreshCw } from 'lucide-react'
import { cn } from '@utils/helpers'

interface Column<T> {
  key: keyof T
  label: string
  sortable?: boolean
  filterable?: boolean
  width?: string
  render?: (value: any, row: T) => React.ReactNode
  align?: 'left' | 'center' | 'right'
}

interface BaseDataTableProps<T> {
  data: T[]
  columns: Column<T>[]
  className?: string
  sortable?: boolean
  filterable?: boolean
  searchable?: boolean
  pagination?: boolean
  pageSize?: number
  loading?: boolean
  onSort?: (key: keyof T, direction: 'asc' | 'desc') => void
  onFilter?: (filters: Record<string, any>) => void
  onSearch?: (query: string) => void
  onRefresh?: () => void
  onExport?: () => void
  emptyMessage?: string
  rowKey?: keyof T
  selectable?: boolean
  onSelectionChange?: (selectedRows: T[]) => void
}

export function BaseDataTable<T extends Record<string, any>>({
  data,
  columns,
  className,
  sortable = true,
  filterable = true,
  searchable = true,
  pagination = true,
  pageSize = 10,
  loading = false,
  onSort,
  onFilter,
  onSearch,
  onRefresh,
  onExport,
  emptyMessage = 'No data available',
  rowKey = 'id' as keyof T,
  selectable = false,
  onSelectionChange,
}: BaseDataTableProps<T>) {
  const [sortConfig, setSortConfig] = useState<{ key: keyof T; direction: 'asc' | 'desc' } | null>(null)
  const [filters, setFilters] = useState<Record<string, any>>({})
  const [searchQuery, setSearchQuery] = useState('')
  const [currentPage, setCurrentPage] = useState(1)
  const [selectedRows, setSelectedRows] = useState<Set<any>>(new Set())

  // Filter and search data
  const filteredData = useMemo(() => {
    let result = [...data]

    // Apply search
    if (searchQuery) {
      result = result.filter(row =>
        Object.values(row).some(value =>
          String(value).toLowerCase().includes(searchQuery.toLowerCase())
        )
      )
    }

    // Apply filters
    Object.entries(filters).forEach(([key, value]) => {
      if (value !== '' && value !== null && value !== undefined) {
        result = result.filter(row => {
          const cellValue = row[key]
          if (typeof value === 'string') {
            return String(cellValue).toLowerCase().includes(value.toLowerCase())
          }
          return cellValue === value
        })
      }
    })

    return result
  }, [data, searchQuery, filters])

  // Sort data
  const sortedData = useMemo(() => {
    if (!sortConfig) return filteredData

    return [...filteredData].sort((a, b) => {
      const aValue = a[sortConfig.key]
      const bValue = b[sortConfig.key]

      if (aValue < bValue) {
        return sortConfig.direction === 'asc' ? -1 : 1
      }
      if (aValue > bValue) {
        return sortConfig.direction === 'asc' ? 1 : -1
      }
      return 0
    })
  }, [filteredData, sortConfig])

  // Paginate data
  const paginatedData = useMemo(() => {
    if (!pagination) return sortedData

    const startIndex = (currentPage - 1) * pageSize
    return sortedData.slice(startIndex, startIndex + pageSize)
  }, [sortedData, currentPage, pageSize, pagination])

  const totalPages = Math.ceil(filteredData.length / pageSize)

  // Handle sorting
  const handleSort = useCallback((key: keyof T) => {
    if (!sortable) return

    const direction = sortConfig?.key === key && sortConfig.direction === 'asc' ? 'desc' : 'asc'
    setSortConfig({ key, direction })
    onSort?.(key, direction)
  }, [sortable, sortConfig, onSort])

  // Handle filtering
  const handleFilter = useCallback((key: keyof T, value: any) => {
    setFilters(prev => ({
      ...prev,
      [key]: value,
    }))
    onFilter?.({ ...filters, [key]: value })
  }, [filters, onFilter])

  // Handle search
  const handleSearch = useCallback((query: string) => {
    setSearchQuery(query)
    setCurrentPage(1)
    onSearch?.(query)
  }, [onSearch])

  // Handle row selection
  const handleRowSelection = useCallback((row: T) => {
    const newSelected = new Set(selectedRows)
    if (newSelected.has(row[rowKey])) {
      newSelected.delete(row[rowKey])
    } else {
      newSelected.add(row[rowKey])
    }
    setSelectedRows(newSelected)
    onSelectionChange?.(data.filter(row => newSelected.has(row[rowKey])))
  }, [selectedRows, rowKey, onSelectionChange, data])

  // Handle select all
  const handleSelectAll = useCallback(() => {
    if (selectedRows.size === paginatedData.length) {
      setSelectedRows(new Set())
      onSelectionChange?.([])
    } else {
      const newSelected = new Set(paginatedData.map(row => row[rowKey]))
      setSelectedRows(newSelected)
      onSelectionChange?.(paginatedData)
    }
  }, [selectedRows, paginatedData, rowKey, onSelectionChange])

  const getSortIcon = (key: keyof T) => {
    if (!sortConfig || sortConfig.key !== key) {
      return <ChevronUp size={16} className="text-industrial-400" />
    }
    return sortConfig.direction === 'asc' ? (
      <ChevronUp size={16} className="text-primary-600" />
    ) : (
      <ChevronDown size={16} className="text-primary-600" />
    )
  }

  return (
    <div className={cn('bg-white rounded-lg shadow-industrial border border-industrial-200', className)}>
      {/* Table Header */}
      <div className="p-4 border-b border-industrial-200">
        <div className="flex flex-col sm:flex-row gap-4 items-start sm:items-center justify-between">
          <div className="flex items-center gap-4">
            <h3 className="text-lg font-semibold text-industrial-900">
              Data Table ({filteredData.length} records)
            </h3>
            {loading && (
              <div className="flex items-center gap-2 text-sm text-industrial-600">
                <RefreshCw size={16} className="animate-spin" />
                Loading...
              </div>
            )}
          </div>
          
          <div className="flex items-center gap-2">
            {searchable && (
              <div className="relative">
                <Search size={16} className="absolute left-3 top-1/2 -translate-y-1/2 text-industrial-400" />
                <input
                  type="text"
                  placeholder="Search..."
                  value={searchQuery}
                  onChange={(e) => handleSearch(e.target.value)}
                  className="pl-10 pr-4 py-2 border border-industrial-300 rounded-lg focus:outline-none focus:ring-2 focus:ring-primary-500 focus:border-transparent"
                />
              </div>
            )}
            
            {onRefresh && (
              <button
                onClick={onRefresh}
                disabled={loading}
                className="p-2 text-industrial-600 hover:text-industrial-900 hover:bg-industrial-100 rounded-lg transition-colors"
              >
                <RefreshCw size={16} className={loading ? 'animate-spin' : ''} />
              </button>
            )}
            
            {onExport && (
              <button
                onClick={onExport}
                className="p-2 text-industrial-600 hover:text-industrial-900 hover:bg-industrial-100 rounded-lg transition-colors"
              >
                <Download size={16} />
              </button>
            )}
          </div>
        </div>
      </div>

      {/* Table */}
      <div className="overflow-x-auto">
        <table className="w-full">
          <thead className="bg-industrial-50">
            <tr>
              {selectable && (
                <th className="px-4 py-3 text-left">
                  <input
                    type="checkbox"
                    checked={selectedRows.size === paginatedData.length && paginatedData.length > 0}
                    onChange={handleSelectAll}
                    className="rounded border-industrial-300 text-primary-600 focus:ring-primary-500"
                  />
                </th>
              )}
              
              {columns.map((column) => (
                <th
                  key={String(column.key)}
                  className={cn(
                    'px-4 py-3 text-left text-sm font-medium text-industrial-700',
                    column.width && `w-${column.width}`,
                    column.align === 'center' && 'text-center',
                    column.align === 'right' && 'text-right',
                    sortable && column.sortable && 'cursor-pointer hover:bg-industrial-100'
                  )}
                  onClick={() => sortable && column.sortable && handleSort(column.key)}
                >
                  <div className={cn(
                    'flex items-center gap-2',
                    column.align === 'center' && 'justify-center',
                    column.align === 'right' && 'justify-end'
                  )}>
                    <span>{column.label}</span>
                    {sortable && column.sortable && getSortIcon(column.key)}
                  </div>
                </th>
              ))}
            </tr>
          </thead>
          
          <tbody className="divide-y divide-industrial-200">
            {paginatedData.map((row, index) => (
              <tr
                key={row[rowKey] || index}
                className={cn(
                  'hover:bg-industrial-50 transition-colors',
                  selectedRows.has(row[rowKey]) && 'bg-primary-50'
                )}
              >
                {selectable && (
                  <td className="px-4 py-3">
                    <input
                      type="checkbox"
                      checked={selectedRows.has(row[rowKey])}
                      onChange={() => handleRowSelection(row)}
                      className="rounded border-industrial-300 text-primary-600 focus:ring-primary-500"
                    />
                  </td>
                )}
                
                {columns.map((column) => (
                  <td
                    key={String(column.key)}
                    className={cn(
                      'px-4 py-3 text-sm text-industrial-900',
                      column.align === 'center' && 'text-center',
                      column.align === 'right' && 'text-right'
                    )}
                  >
                    {column.render ? column.render(row[column.key], row) : row[column.key]}
                  </td>
                ))}
              </tr>
            ))}
          </tbody>
        </table>
        
        {paginatedData.length === 0 && (
          <div className="text-center py-8 text-industrial-500">
            {emptyMessage}
          </div>
        )}
      </div>

      {/* Pagination */}
      {pagination && totalPages > 1 && (
        <div className="px-4 py-3 border-t border-industrial-200">
          <div className="flex items-center justify-between">
            <div className="text-sm text-industrial-600">
              Showing {((currentPage - 1) * pageSize) + 1} to {Math.min(currentPage * pageSize, filteredData.length)} of {filteredData.length} results
            </div>
            
            <div className="flex items-center gap-2">
              <button
                onClick={() => setCurrentPage(prev => Math.max(prev - 1, 1))}
                disabled={currentPage === 1}
                className="px-3 py-1 text-sm border border-industrial-300 rounded hover:bg-industrial-50 disabled:opacity-50 disabled:cursor-not-allowed"
              >
                Previous
              </button>
              
              {Array.from({ length: totalPages }, (_, i) => i + 1).map((page) => (
                <button
                  key={page}
                  onClick={() => setCurrentPage(page)}
                  className={cn(
                    'px-3 py-1 text-sm border rounded',
                    currentPage === page
                      ? 'bg-primary-600 text-white border-primary-600'
                      : 'border-industrial-300 hover:bg-industrial-50'
                  )}
                >
                  {page}
                </button>
              ))}
              
              <button
                onClick={() => setCurrentPage(prev => Math.min(prev + 1, totalPages))}
                disabled={currentPage === totalPages}
                className="px-3 py-1 text-sm border border-industrial-300 rounded hover:bg-industrial-50 disabled:opacity-50 disabled:cursor-not-allowed"
              >
                Next
              </button>
            </div>
          </div>
        </div>
      )}
    </div>
  )
}
```

### **2. Telemetry Data Table (src/components/tables/TelemetryDataTable.tsx)**

```typescript
import React, { useMemo, useCallback } from 'react'
import { Clock, Activity, Battery, Thermometer } from 'lucide-react'
import { BaseDataTable } from './BaseDataTable'
import { useTelemetryStore } from '@stores/telemetryStore'
import { cn, formatSpeed, formatAcceleration, formatPosition, formatBatteryLevel } from '@utils/helpers'
import { TelemetryData } from '@types'

interface TelemetryDataTableProps {
  className?: string
  timeRange?: number
  showFilters?: boolean
  showSearch?: boolean
  pageSize?: number
  onExport?: () => void
}

export const TelemetryDataTable: React.FC<TelemetryDataTableProps> = ({
  className,
  timeRange = 3600, // 1 hour
  showFilters = true,
  showSearch = true,
  pageSize = 20,
  onExport,
}) => {
  const { telemetryData, isLoading, refreshData } = useTelemetryStore()

  const filteredData = useMemo(() => {
    if (!timeRange) return telemetryData

    const now = Date.now()
    return telemetryData.filter(data => now - data.timestamp <= timeRange * 1000)
  }, [telemetryData, timeRange])

  const columns = useMemo(() => [
    {
      key: 'timestamp' as keyof TelemetryData,
      label: 'Timestamp',
      sortable: true,
      width: '48',
      render: (value: number) => (
        <div className="flex items-center gap-2">
          <Clock size={14} className="text-industrial-400" />
          <span className="font-mono text-xs">
            {new Date(value).toLocaleTimeString('en-US', {
              hour12: false,
              hour: '2-digit',
              minute: '2-digit',
              second: '2-digit',
            })}
          </span>
        </div>
      ),
    },
    {
      key: 'status' as keyof TelemetryData,
      label: 'Status',
      sortable: true,
      width: '32',
      render: (value: string) => (
        <span className={cn(
          'px-2 py-1 text-xs font-medium rounded-full',
          value === 'idle' && 'bg-green-100 text-green-800',
          value === 'moving' && 'bg-blue-100 text-blue-800',
          value === 'docking' && 'bg-yellow-100 text-yellow-800',
          value === 'fault' && 'bg-red-100 text-red-800',
          value === 'estop' && 'bg-red-200 text-red-900',
        )}>
          {value.toUpperCase()}
        </span>
      ),
    },
    {
      key: 'position' as keyof TelemetryData,
      label: 'Position (m)',
      sortable: true,
      width: '32',
      render: (value: number) => (
        <span className="font-mono text-sm">
          {formatPosition(value)}
        </span>
      ),
    },
    {
      key: 'velocity' as keyof TelemetryData,
      label: 'Velocity (m/s)',
      sortable: true,
      width: '32',
      render: (value: number) => (
        <div className="flex items-center gap-2">
          <Activity size={14} className="text-blue-600" />
          <span className="font-mono text-sm">
            {formatSpeed(value)}
          </span>
        </div>
      ),
    },
    {
      key: 'acceleration' as keyof TelemetryData,
      label: 'Acceleration (m/s²)',
      sortable: true,
      width: '40',
      render: (value: number) => (
        <span className="font-mono text-sm">
          {formatAcceleration(value)}
        </span>
      ),
    },
    {
      key: 'batteryLevel' as keyof TelemetryData,
      label: 'Battery',
      sortable: true,
      width: '32',
      render: (value: number) => (
        <div className="flex items-center gap-2">
          <Battery size={14} className="text-green-600" />
          <span className="font-mono text-sm">
            {formatBatteryLevel(value)}
          </span>
        </div>
      ),
    },
    {
      key: 'temperature' as keyof TelemetryData,
      label: 'Temperature',
      sortable: true,
      width: '32',
      render: (value: number) => (
        <div className="flex items-center gap-2">
          <Thermometer size={14} className="text-red-600" />
          <span className="font-mono text-sm">
            {value.toFixed(1)}°C
          </span>
        </div>
      ),
    },
    {
      key: 'voltage' as keyof TelemetryData,
      label: 'Voltage (V)',
      sortable: true,
      width: '32',
      render: (value: number) => (
        <span className="font-mono text-sm">
          {value.toFixed(1)}V
        </span>
      ),
    },
    {
      key: 'current' as keyof TelemetryData,
      label: 'Current (A)',
      sortable: true,
      width: '32',
      render: (value: number) => (
        <span className="font-mono text-sm">
          {value.toFixed(2)}A
        </span>
      ),
    },
  ], [])

  const handleExport = useCallback(() => {
    if (onExport) {
      onExport()
    } else {
      // Default export to CSV
      const csvContent = [
        // CSV header
        columns.map(col => col.label).join(','),
        // CSV data
        ...filteredData.map(row => 
          columns.map(col => {
            const value = row[col.key]
            if (col.key === 'timestamp') {
              return new Date(value).toISOString()
            }
            if (col.key === 'batteryLevel') {
              return `${(value * 100).toFixed(1)}%`
            }
            return String(value)
          }).join(',')
        )
      ].join('\n')

      const blob = new Blob([csvContent], { type: 'text/csv' })
      const url = URL.createObjectURL(blob)
      const a = document.createElement('a')
      a.href = url
      a.download = `telemetry-data-${new Date().toISOString().split('T')[0]}.csv`
      a.click()
      URL.revokeObjectURL(url)
    }
  }, [filteredData, columns, onExport])

  return (
    <BaseDataTable
      data={filteredData}
      columns={columns}
      className={className}
      sortable={true}
      filterable={showFilters}
      searchable={showSearch}
      pagination={true}
      pageSize={pageSize}
      loading={isLoading}
      onRefresh={refreshData}
      onExport={handleExport}
      emptyMessage="No telemetry data available"
      rowKey="timestamp"
    />
  )
}
```

### **3. System Logs Table (src/components/tables/SystemLogsTable.tsx)**

```typescript
import React, { useMemo, useCallback } from 'react'
import { AlertTriangle, Info, CheckCircle, XCircle, Clock, User } from 'lucide-react'
import { BaseDataTable } from './BaseDataTable'
import { useSystemStore } from '@stores/systemStore'
import { cn, formatRelativeTime } from '@utils/helpers'
import { SystemLog } from '@types'

interface SystemLogsTableProps {
  className?: string
  logLevel?: 'all' | 'error' | 'warning' | 'info' | 'debug'
  timeRange?: number
  showFilters?: boolean
  showSearch?: boolean
  pageSize?: number
  onExport?: () => void
}

export const SystemLogsTable: React.FC<SystemLogsTableProps> = ({
  className,
  logLevel = 'all',
  timeRange = 86400, // 24 hours
  showFilters = true,
  showSearch = true,
  pageSize = 50,
  onExport,
}) => {
  const { systemLogs, isLoading, refreshLogs } = useSystemStore()

  const filteredLogs = useMemo(() => {
    let filtered = systemLogs

    // Filter by log level
    if (logLevel !== 'all') {
      filtered = filtered.filter(log => log.level === logLevel)
    }

    // Filter by time range
    if (timeRange) {
      const now = Date.now()
      filtered = filtered.filter(log => now - log.timestamp <= timeRange * 1000)
    }

    return filtered
  }, [systemLogs, logLevel, timeRange])

  const getLogLevelIcon = (level: string) => {
    switch (level) {
      case 'error':
        return <XCircle size={14} className="text-red-600" />
      case 'warning':
        return <AlertTriangle size={14} className="text-yellow-600" />
      case 'info':
        return <Info size={14} className="text-blue-600" />
      case 'debug':
        return <CheckCircle size={14} className="text-green-600" />
      default:
        return <Info size={14} className="text-industrial-400" />
    }
  }

  const getLogLevelColor = (level: string) => {
    switch (level) {
      case 'error':
        return 'bg-red-100 text-red-800'
      case 'warning':
        return 'bg-yellow-100 text-yellow-800'
      case 'info':
        return 'bg-blue-100 text-blue-800'
      case 'debug':
        return 'bg-green-100 text-green-800'
      default:
        return 'bg-industrial-100 text-industrial-800'
    }
  }

  const columns = useMemo(() => [
    {
      key: 'timestamp' as keyof SystemLog,
      label: 'Timestamp',
      sortable: true,
      width: '48',
      render: (value: number) => (
        <div className="flex items-center gap-2">
          <Clock size={14} className="text-industrial-400" />
          <div className="flex flex-col">
            <span className="font-mono text-xs">
              {new Date(value).toLocaleTimeString('en-US', {
                hour12: false,
                hour: '2-digit',
                minute: '2-digit',
                second: '2-digit',
              })}
            </span>
            <span className="text-xs text-industrial-500">
              {formatRelativeTime(value)}
            </span>
          </div>
        </div>
      ),
    },
    {
      key: 'level' as keyof SystemLog,
      label: 'Level',
      sortable: true,
      width: '24',
      render: (value: string) => (
        <div className="flex items-center gap-2">
          {getLogLevelIcon(value)}
          <span className={cn(
            'px-2 py-1 text-xs font-medium rounded-full',
            getLogLevelColor(value)
          )}>
            {value.toUpperCase()}
          </span>
        </div>
      ),
    },
    {
      key: 'source' as keyof SystemLog,
      label: 'Source',
      sortable: true,
      width: '32',
      render: (value: string) => (
        <span className="text-sm font-medium text-industrial-700">
          {value}
        </span>
      ),
    },
    {
      key: 'message' as keyof SystemLog,
      label: 'Message',
      sortable: false,
      render: (value: string) => (
        <div className="max-w-md">
          <p className="text-sm text-industrial-900 line-clamp-2">
            {value}
          </p>
        </div>
      ),
    },
    {
      key: 'user' as keyof SystemLog,
      label: 'User',
      sortable: true,
      width: '32',
      render: (value: string) => (
        <div className="flex items-center gap-2">
          <User size={14} className="text-industrial-400" />
          <span className="text-sm text-industrial-700">
            {value || 'System'}
          </span>
        </div>
      ),
    },
    {
      key: 'category' as keyof SystemLog,
      label: 'Category',
      sortable: true,
      width: '32',
      render: (value: string) => (
        <span className="text-xs px-2 py-1 bg-industrial-100 text-industrial-700 rounded">
          {value}
        </span>
      ),
    },
  ], [])

  const handleExport = useCallback(() => {
    if (onExport) {
      onExport()
    } else {
      // Default export to CSV
      const csvContent = [
        // CSV header
        columns.map(col => col.label).join(','),
        // CSV data
        ...filteredLogs.map(log => 
          [
            new Date(log.timestamp).toISOString(),
            log.level,
            log.source,
            `"${log.message.replace(/"/g, '""')}"`,
            log.user || 'System',
            log.category,
          ].join(',')
        )
      ].join('\n')

      const blob = new Blob([csvContent], { type: 'text/csv' })
      const url = URL.createObjectURL(blob)
      const a = document.createElement('a')
      a.href = url
      a.download = `system-logs-${new Date().toISOString().split('T')[0]}.csv`
      a.click()
      URL.revokeObjectURL(url)
    }
  }, [filteredLogs, columns, onExport])

  return (
    <BaseDataTable
      data={filteredLogs}
      columns={columns}
      className={className}
      sortable={true}
      filterable={showFilters}
      searchable={showSearch}
      pagination={true}
      pageSize={pageSize}
      loading={isLoading}
      onRefresh={refreshLogs}
      onExport={handleExport}
      emptyMessage="No system logs available"
      rowKey="id"
    />
  )
}
```

### **4. Configuration Data Table (src/components/tables/ConfigurationDataTable.tsx)**

```typescript
import React, { useMemo, useCallback } from 'react'
import { Settings, Edit, Eye, Trash2, CheckCircle, XCircle } from 'lucide-react'
import { BaseDataTable } from './BaseDataTable'
import { useConfigStore } from '@stores/configStore'
import { cn, formatRelativeTime } from '@utils/helpers'
import { Configuration } from '@types'

interface ConfigurationDataTableProps {
  className?: string
  showFilters?: boolean
  showSearch?: boolean
  pageSize?: number
  onEdit?: (config: Configuration) => void
  onView?: (config: Configuration) => void
  onDelete?: (config: Configuration) => void
  onExport?: () => void
}

export const ConfigurationDataTable: React.FC<ConfigurationDataTableProps> = ({
  className,
  showFilters = true,
  showSearch = true,
  pageSize = 20,
  onEdit,
  onView,
  onDelete,
  onExport,
}) => {
  const { configurations, isLoading, refreshConfigs } = useConfigStore()

  const columns = useMemo(() => [
    {
      key: 'name' as keyof Configuration,
      label: 'Name',
      sortable: true,
      width: '48',
      render: (value: string, row: Configuration) => (
        <div className="flex items-center gap-2">
          <Settings size={16} className="text-primary-600" />
          <div>
            <div className="font-medium text-industrial-900">{value}</div>
            <div className="text-xs text-industrial-500">{row.description}</div>
          </div>
        </div>
      ),
    },
    {
      key: 'category' as keyof Configuration,
      label: 'Category',
      sortable: true,
      width: '32',
      render: (value: string) => (
        <span className="text-xs px-2 py-1 bg-primary-100 text-primary-700 rounded">
          {value}
        </span>
      ),
    },
    {
      key: 'version' as keyof Configuration,
      label: 'Version',
      sortable: true,
      width: '24',
      render: (value: string) => (
        <span className="font-mono text-sm text-industrial-700">
          v{value}
        </span>
      ),
    },
    {
      key: 'isActive' as keyof Configuration,
      label: 'Status',
      sortable: true,
      width: '24',
      render: (value: boolean) => (
        <div className="flex items-center gap-2">
          {value ? (
            <CheckCircle size={16} className="text-green-600" />
          ) : (
            <XCircle size={16} className="text-red-600" />
          )}
          <span className={cn(
            'text-sm font-medium',
            value ? 'text-green-700' : 'text-red-700'
          )}>
            {value ? 'Active' : 'Inactive'}
          </span>
        </div>
      ),
    },
    {
      key: 'lastModified' as keyof Configuration,
      label: 'Last Modified',
      sortable: true,
      width: '48',
      render: (value: number) => (
        <div className="flex flex-col">
          <span className="text-sm text-industrial-900">
            {new Date(value).toLocaleDateString()}
          </span>
          <span className="text-xs text-industrial-500">
            {formatRelativeTime(value)}
          </span>
        </div>
      ),
    },
    {
      key: 'modifiedBy' as keyof Configuration,
      label: 'Modified By',
      sortable: true,
      width: '32',
      render: (value: string) => (
        <span className="text-sm text-industrial-700">
          {value}
        </span>
      ),
    },
    {
      key: 'actions' as keyof Configuration,
      label: 'Actions',
      sortable: false,
      width: '32',
      render: (value: any, row: Configuration) => (
        <div className="flex items-center gap-1">
          {onView && (
            <button
              onClick={() => onView(row)}
              className="p-1 text-industrial-600 hover:text-blue-600 hover:bg-blue-50 rounded transition-colors"
              title="View configuration"
            >
              <Eye size={14} />
            </button>
          )}
          
          {onEdit && (
            <button
              onClick={() => onEdit(row)}
              className="p-1 text-industrial-600 hover:text-yellow-600 hover:bg-yellow-50 rounded transition-colors"
              title="Edit configuration"
            >
              <Edit size={14} />
            </button>
          )}
          
          {onDelete && (
            <button
              onClick={() => onDelete(row)}
              className="p-1 text-industrial-600 hover:text-red-600 hover:bg-red-50 rounded transition-colors"
              title="Delete configuration"
            >
              <Trash2 size={14} />
            </button>
          )}
        </div>
      ),
    },
  ], [onView, onEdit, onDelete])

  const handleExport = useCallback(() => {
    if (onExport) {
      onExport()
    } else {
      // Default export to JSON
      const jsonContent = JSON.stringify(configurations, null, 2)
      const blob = new Blob([jsonContent], { type: 'application/json' })
      const url = URL.createObjectURL(blob)
      const a = document.createElement('a')
      a.href = url
      a.download = `configurations-${new Date().toISOString().split('T')[0]}.json`
      a.click()
      URL.revokeObjectURL(url)
    }
  }, [configurations, onExport])

  return (
    <BaseDataTable
      data={configurations}
      columns={columns}
      className={className}
      sortable={true}
      filterable={showFilters}
      searchable={showSearch}
      pagination={true}
      pageSize={pageSize}
      loading={isLoading}
      onRefresh={refreshConfigs}
      onExport={handleExport}
      emptyMessage="No configurations available"
      rowKey="id"
    />
  )
}
```

### **5. Tables Index (src/components/tables/index.ts)**

```typescript
export { BaseDataTable } from './BaseDataTable'
export { TelemetryDataTable } from './TelemetryDataTable'
export { SystemLogsTable } from './SystemLogsTable'
export { ConfigurationDataTable } from './ConfigurationDataTable'

// Re-export types
export type { Column } from './BaseDataTable'
```

---

## 🔧 **NEXT STEPS**

### **Immediate Actions:**
1. ✅ Base Data Table component với sorting/filtering
2. ✅ Telemetry Data Table với real-time data
3. ✅ System Logs Table với log levels
4. ✅ Configuration Data Table với actions
5. ✅ Tables integration với stores

### **Phase 3 Week 4 Day 3-4 Checklist:**
- [x] Base Data Table component
- [x] Telemetry Data Table component
- [x] System Logs Table component
- [x] Configuration Data Table component
- [x] Tables integration

### **Ready for Next Phase:**
- Status Cards (Week 5)
- Log Viewers (Week 6)

---

**Changelog:**
- v1.0: Data tables implementation completed

**Status:** Ready for status cards
