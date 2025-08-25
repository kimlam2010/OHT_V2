# Component Library Implementation

**Version:** 1.0  
**Date:** 2024-12-19  
**Team:** Frontend  
**Status:** In Progress

## 📋 **Overview**

Component library cho OHT-50 frontend, tuân thủ design system và safety-first principles.

## 🎨 **Design System Integration**

### **Design Tokens**
```css
:root {
  /* Primary Colors */
  --primary-50: #fff7ed;
  --primary-100: #ffedd5;
  --primary-500: #ff6b35;
  --primary-600: #ea580c;
  --primary-700: #c2410c;
  
  /* Industrial Grays */
  --gray-50: #f9fafb;
  --gray-100: #f3f4f6;
  --gray-800: #1f2937;
  --gray-900: #111827;
  
  /* Safety Colors */
  --safety-red: #dc2626;
  --safety-yellow: #f59e0b;
  --safety-green: #059669;
  
  /* Typography */
  --font-sans: 'Inter', system-ui, sans-serif;
  --font-mono: 'JetBrains Mono', monospace;
  
  /* Spacing */
  --spacing-1: 0.25rem;
  --spacing-2: 0.5rem;
  --spacing-4: 1rem;
  --spacing-6: 1.5rem;
  --spacing-8: 2rem;
  
  /* Border Radius */
  --radius-sm: 0.375rem;
  --radius-md: 0.5rem;
  --radius-lg: 0.75rem;
  
  /* Shadows */
  --shadow-sm: 0 1px 2px 0 rgb(0 0 0 / 0.05);
  --shadow-md: 0 4px 6px -1px rgb(0 0 0 / 0.1);
  --shadow-lg: 0 10px 15px -3px rgb(0 0 0 / 0.1);
}
```

## 🧩 **Core Components**

### **1. Button Components**

#### **Primary Button**
```tsx
interface PrimaryButtonProps {
  children: React.ReactNode;
  onClick: () => void;
  disabled?: boolean;
  loading?: boolean;
  size?: 'sm' | 'md' | 'lg';
  variant?: 'solid' | 'outline' | 'ghost';
}

const PrimaryButton: React.FC<PrimaryButtonProps> = ({
  children,
  onClick,
  disabled = false,
  loading = false,
  size = 'md',
  variant = 'solid'
}) => {
  return (
    <button
      className={`
        inline-flex items-center justify-center
        font-medium rounded-md
        transition-all duration-200
        focus:outline-none focus:ring-2 focus:ring-offset-2
        disabled:opacity-50 disabled:cursor-not-allowed
        
        ${size === 'sm' ? 'px-3 py-1.5 text-sm' : ''}
        ${size === 'md' ? 'px-4 py-2 text-base' : ''}
        ${size === 'lg' ? 'px-6 py-3 text-lg' : ''}
        
        ${variant === 'solid' ? 'bg-primary-500 text-white hover:bg-primary-600 focus:ring-primary-500' : ''}
        ${variant === 'outline' ? 'border border-primary-500 text-primary-500 hover:bg-primary-50' : ''}
        ${variant === 'ghost' ? 'text-primary-500 hover:bg-primary-50' : ''}
      `}
      onClick={onClick}
      disabled={disabled || loading}
    >
      {loading && <Spinner className="mr-2" />}
      {children}
    </button>
  );
};
```

#### **Emergency Stop Button**
```tsx
interface EmergencyStopButtonProps {
  onEmergencyStop: () => void;
  disabled?: boolean;
}

const EmergencyStopButton: React.FC<EmergencyStopButtonProps> = ({
  onEmergencyStop,
  disabled = false
}) => {
  return (
    <button
      className={`
        relative w-24 h-24 rounded-full
        bg-safety-red border-4 border-white
        shadow-lg hover:shadow-xl
        transition-all duration-200
        focus:outline-none focus:ring-4 focus:ring-red-300
        disabled:opacity-50 disabled:cursor-not-allowed
        
        ${!disabled ? 'hover:scale-105 active:scale-95' : ''}
      `}
      onClick={onEmergencyStop}
      disabled={disabled}
      aria-label="Emergency Stop"
    >
      <div className="absolute inset-0 flex items-center justify-center">
        <span className="text-white font-bold text-lg">STOP</span>
      </div>
      
      {/* Emergency glow effect */}
      <div className="absolute inset-0 rounded-full bg-red-500 animate-pulse opacity-20" />
    </button>
  );
};
```

### **2. Control Components**

#### **Directional Control Pad**
```tsx
interface DirectionalControlProps {
  onMove: (direction: 'forward' | 'backward' | 'left' | 'right' | 'stop') => void;
  disabled?: boolean;
}

const DirectionalControl: React.FC<DirectionalControlProps> = ({
  onMove,
  disabled = false
}) => {
  return (
    <div className="grid grid-cols-3 gap-2 w-48 h-48">
      {/* Forward */}
      <div className="col-start-2">
        <button
          className="w-full h-16 bg-gray-800 text-white rounded-lg
                     hover:bg-gray-700 focus:outline-none focus:ring-2 focus:ring-primary-500
                     disabled:opacity-50 disabled:cursor-not-allowed"
          onClick={() => onMove('forward')}
          disabled={disabled}
        >
          <ChevronUp className="w-8 h-8 mx-auto" />
        </button>
      </div>
      
      {/* Left */}
      <div className="row-start-2">
        <button
          className="w-16 h-16 bg-gray-800 text-white rounded-lg
                     hover:bg-gray-700 focus:outline-none focus:ring-2 focus:ring-primary-500
                     disabled:opacity-50 disabled:cursor-not-allowed"
          onClick={() => onMove('left')}
          disabled={disabled}
        >
          <ChevronLeft className="w-8 h-8 mx-auto" />
        </button>
      </div>
      
      {/* Stop */}
      <div className="row-start-2">
        <button
          className="w-16 h-16 bg-safety-red text-white rounded-lg
                     hover:bg-red-700 focus:outline-none focus:ring-2 focus:ring-red-500
                     disabled:opacity-50 disabled:cursor-not-allowed"
          onClick={() => onMove('stop')}
          disabled={disabled}
        >
          <Square className="w-6 h-6 mx-auto" />
        </button>
      </div>
      
      {/* Right */}
      <div className="row-start-2">
        <button
          className="w-16 h-16 bg-gray-800 text-white rounded-lg
                     hover:bg-gray-700 focus:outline-none focus:ring-2 focus:ring-primary-500
                     disabled:opacity-50 disabled:cursor-not-allowed"
          onClick={() => onMove('right')}
          disabled={disabled}
        >
          <ChevronRight className="w-8 h-8 mx-auto" />
        </button>
      </div>
      
      {/* Backward */}
      <div className="col-start-2 row-start-3">
        <button
          className="w-full h-16 bg-gray-800 text-white rounded-lg
                     hover:bg-gray-700 focus:outline-none focus:ring-2 focus:ring-primary-500
                     disabled:opacity-50 disabled:cursor-not-allowed"
          onClick={() => onMove('backward')}
          disabled={disabled}
        >
          <ChevronDown className="w-8 h-8 mx-auto" />
        </button>
      </div>
    </div>
  );
};
```

#### **Speed Control Slider**
```tsx
interface SpeedControlProps {
  speed: number;
  onSpeedChange: (speed: number) => void;
  disabled?: boolean;
}

const SpeedControl: React.FC<SpeedControlProps> = ({
  speed,
  onSpeedChange,
  disabled = false
}) => {
  const speedPresets = [
    { label: 'Slow', value: 25, color: 'bg-green-500' },
    { label: 'Normal', value: 50, color: 'bg-yellow-500' },
    { label: 'Fast', value: 75, color: 'bg-orange-500' },
    { label: 'Max', value: 100, color: 'bg-red-500' }
  ];

  return (
    <div className="space-y-4">
      <div className="flex items-center justify-between">
        <label className="text-sm font-medium text-gray-700">Speed Control</label>
        <span className="text-lg font-bold text-primary-600">{speed}%</span>
      </div>
      
      {/* Speed Slider */}
      <input
        type="range"
        min="0"
        max="100"
        value={speed}
        onChange={(e) => onSpeedChange(Number(e.target.value))}
        disabled={disabled}
        className="w-full h-2 bg-gray-200 rounded-lg appearance-none cursor-pointer
                   disabled:opacity-50 disabled:cursor-not-allowed
                   [&::-webkit-slider-thumb]:appearance-none
                   [&::-webkit-slider-thumb]:w-6
                   [&::-webkit-slider-thumb]:h-6
                   [&::-webkit-slider-thumb]:bg-primary-500
                   [&::-webkit-slider-thumb]:rounded-full
                   [&::-webkit-slider-thumb]:cursor-pointer"
      />
      
      {/* Speed Presets */}
      <div className="grid grid-cols-4 gap-2">
        {speedPresets.map((preset) => (
          <button
            key={preset.value}
            className={`
              px-3 py-2 text-sm font-medium rounded-md
              transition-all duration-200
              focus:outline-none focus:ring-2 focus:ring-offset-2
              disabled:opacity-50 disabled:cursor-not-allowed
              ${speed === preset.value 
                ? `${preset.color} text-white` 
                : 'bg-gray-100 text-gray-700 hover:bg-gray-200'
              }
            `}
            onClick={() => onSpeedChange(preset.value)}
            disabled={disabled}
          >
            {preset.label}
          </button>
        ))}
      </div>
    </div>
  );
};
```

### **3. Status Components**

#### **Status Card**
```tsx
interface StatusCardProps {
  title: string;
  value: string | number;
  unit?: string;
  status: 'normal' | 'warning' | 'error' | 'offline';
  icon?: React.ReactNode;
  trend?: 'up' | 'down' | 'stable';
}

const StatusCard: React.FC<StatusCardProps> = ({
  title,
  value,
  unit,
  status,
  icon,
  trend
}) => {
  const statusColors = {
    normal: 'bg-green-50 border-green-200 text-green-800',
    warning: 'bg-yellow-50 border-yellow-200 text-yellow-800',
    error: 'bg-red-50 border-red-200 text-red-800',
    offline: 'bg-gray-50 border-gray-200 text-gray-800'
  };

  const trendIcons = {
    up: <TrendingUp className="w-4 h-4 text-green-600" />,
    down: <TrendingDown className="w-4 h-4 text-red-600" />,
    stable: <Minus className="w-4 h-4 text-gray-600" />
  };

  return (
    <div className={`
      p-4 border rounded-lg
      transition-all duration-200
      hover:shadow-md
      ${statusColors[status]}
    `}>
      <div className="flex items-center justify-between">
        <div className="flex items-center space-x-2">
          {icon && <div className="text-gray-600">{icon}</div>}
          <h3 className="text-sm font-medium">{title}</h3>
        </div>
        {trend && trendIcons[trend]}
      </div>
      
      <div className="mt-2">
        <span className="text-2xl font-bold">
          {value}
          {unit && <span className="text-sm font-normal ml-1">{unit}</span>}
        </span>
      </div>
    </div>
  );
};
```

#### **Alert Banner**
```tsx
interface AlertBannerProps {
  level: 'critical' | 'warning' | 'info' | 'success';
  title: string;
  message: string;
  onDismiss?: () => void;
  dismissible?: boolean;
}

const AlertBanner: React.FC<AlertBannerProps> = ({
  level,
  title,
  message,
  onDismiss,
  dismissible = true
}) => {
  const alertStyles = {
    critical: 'bg-red-50 border-red-200 text-red-800',
    warning: 'bg-yellow-50 border-yellow-200 text-yellow-800',
    info: 'bg-blue-50 border-blue-200 text-blue-800',
    success: 'bg-green-50 border-green-200 text-green-800'
  };

  const icons = {
    critical: <AlertTriangle className="w-5 h-5" />,
    warning: <AlertCircle className="w-5 h-5" />,
    info: <Info className="w-5 h-5" />,
    success: <CheckCircle className="w-5 h-5" />
  };

  return (
    <div className={`
      border rounded-lg p-4
      ${alertStyles[level]}
    `}>
      <div className="flex items-start">
        <div className="flex-shrink-0 mr-3">
          {icons[level]}
        </div>
        
        <div className="flex-1">
          <h3 className="text-sm font-medium">{title}</h3>
          <p className="text-sm mt-1">{message}</p>
        </div>
        
        {dismissible && onDismiss && (
          <button
            onClick={onDismiss}
            className="flex-shrink-0 ml-3 text-gray-400 hover:text-gray-600
                     focus:outline-none focus:ring-2 focus:ring-offset-2 focus:ring-gray-500"
          >
            <X className="w-4 h-4" />
          </button>
        )}
      </div>
    </div>
  );
};
```

### **4. Data Display Components**

#### **Telemetry Chart**
```tsx
interface TelemetryChartProps {
  data: Array<{ timestamp: string; value: number }>;
  title: string;
  unit: string;
  color?: string;
  height?: number;
}

const TelemetryChart: React.FC<TelemetryChartProps> = ({
  data,
  title,
  unit,
  color = '#ff6b35',
  height = 200
}) => {
  const chartRef = useRef<HTMLCanvasElement>(null);
  const chartInstance = useRef<Chart | null>(null);

  useEffect(() => {
    if (chartRef.current) {
      const ctx = chartRef.current.getContext('2d');
      if (ctx) {
        // Destroy existing chart
        if (chartInstance.current) {
          chartInstance.current.destroy();
        }

        // Create new chart
        chartInstance.current = new Chart(ctx, {
          type: 'line',
          data: {
            labels: data.map(d => new Date(d.timestamp).toLocaleTimeString()),
            datasets: [{
              label: title,
              data: data.map(d => d.value),
              borderColor: color,
              backgroundColor: `${color}20`,
              borderWidth: 2,
              fill: true,
              tension: 0.4
            }]
          },
          options: {
            responsive: true,
            maintainAspectRatio: false,
            plugins: {
              legend: {
                display: false
              }
            },
            scales: {
              y: {
                beginAtZero: true,
                ticks: {
                  callback: (value) => `${value}${unit}`
                }
              }
            }
          }
        });
      }
    }

    return () => {
      if (chartInstance.current) {
        chartInstance.current.destroy();
      }
    };
  }, [data, title, unit, color]);

  return (
    <div className="bg-white border rounded-lg p-4">
      <h3 className="text-lg font-medium text-gray-900 mb-4">{title}</h3>
      <div style={{ height }}>
        <canvas ref={chartRef} />
      </div>
    </div>
  );
};
```

#### **Data Table**
```tsx
interface DataTableProps<T> {
  data: T[];
  columns: Array<{
    key: keyof T;
    header: string;
    render?: (value: any, row: T) => React.ReactNode;
  }>;
  onRowClick?: (row: T) => void;
  loading?: boolean;
}

const DataTable = <T extends Record<string, any>>({
  data,
  columns,
  onRowClick,
  loading = false
}: DataTableProps<T>) => {
  if (loading) {
    return (
      <div className="bg-white border rounded-lg p-4">
        <div className="animate-pulse space-y-3">
          {[...Array(5)].map((_, i) => (
            <div key={i} className="h-4 bg-gray-200 rounded" />
          ))}
        </div>
      </div>
    );
  }

  return (
    <div className="bg-white border rounded-lg overflow-hidden">
      <table className="min-w-full divide-y divide-gray-200">
        <thead className="bg-gray-50">
          <tr>
            {columns.map((column) => (
              <th
                key={String(column.key)}
                className="px-6 py-3 text-left text-xs font-medium text-gray-500 uppercase tracking-wider"
              >
                {column.header}
              </th>
            ))}
          </tr>
        </thead>
        <tbody className="bg-white divide-y divide-gray-200">
          {data.map((row, index) => (
            <tr
              key={index}
              onClick={() => onRowClick?.(row)}
              className={onRowClick ? 'cursor-pointer hover:bg-gray-50' : ''}
            >
              {columns.map((column) => (
                <td
                  key={String(column.key)}
                  className="px-6 py-4 whitespace-nowrap text-sm text-gray-900"
                >
                  {column.render 
                    ? column.render(row[column.key], row)
                    : String(row[column.key])
                  }
                </td>
              ))}
            </tr>
          ))}
        </tbody>
      </table>
    </div>
  );
};
```

## 🔧 **Utility Components**

### **Loading Spinner**
```tsx
interface SpinnerProps {
  size?: 'sm' | 'md' | 'lg';
  color?: string;
}

const Spinner: React.FC<SpinnerProps> = ({
  size = 'md',
  color = 'text-primary-500'
}) => {
  const sizeClasses = {
    sm: 'w-4 h-4',
    md: 'w-6 h-6',
    lg: 'w-8 h-8'
  };

  return (
    <div className={`animate-spin ${sizeClasses[size]} ${color}`}>
      <svg className="w-full h-full" fill="none" viewBox="0 0 24 24">
        <circle
          className="opacity-25"
          cx="12"
          cy="12"
          r="10"
          stroke="currentColor"
          strokeWidth="4"
        />
        <path
          className="opacity-75"
          fill="currentColor"
          d="M4 12a8 8 0 018-8V0C5.373 0 0 5.373 0 12h4zm2 5.291A7.962 7.962 0 014 12H0c0 3.042 1.135 5.824 3 7.938l3-2.647z"
        />
      </svg>
    </div>
  );
};
```

### **Modal Component**
```tsx
interface ModalProps {
  isOpen: boolean;
  onClose: () => void;
  title: string;
  children: React.ReactNode;
  size?: 'sm' | 'md' | 'lg' | 'xl';
}

const Modal: React.FC<ModalProps> = ({
  isOpen,
  onClose,
  title,
  children,
  size = 'md'
}) => {
  const sizeClasses = {
    sm: 'max-w-md',
    md: 'max-w-lg',
    lg: 'max-w-2xl',
    xl: 'max-w-4xl'
  };

  if (!isOpen) return null;

  return (
    <div className="fixed inset-0 z-50 overflow-y-auto">
      <div className="flex items-center justify-center min-h-screen px-4 pt-4 pb-20 text-center sm:block sm:p-0">
        {/* Backdrop */}
        <div
          className="fixed inset-0 bg-gray-500 bg-opacity-75 transition-opacity"
          onClick={onClose}
        />
        
        {/* Modal */}
        <div className={`
          inline-block align-bottom bg-white rounded-lg text-left overflow-hidden shadow-xl transform transition-all sm:my-8 sm:align-middle sm:w-full ${sizeClasses[size]}
        `}>
          {/* Header */}
          <div className="bg-gray-50 px-4 py-3 sm:px-6">
            <div className="flex items-center justify-between">
              <h3 className="text-lg leading-6 font-medium text-gray-900">
                {title}
              </h3>
              <button
                onClick={onClose}
                className="text-gray-400 hover:text-gray-600 focus:outline-none focus:ring-2 focus:ring-offset-2 focus:ring-gray-500"
              >
                <X className="w-5 h-5" />
              </button>
            </div>
          </div>
          
          {/* Content */}
          <div className="px-4 py-5 sm:p-6">
            {children}
          </div>
        </div>
      </div>
    </div>
  );
};
```

## 📱 **Responsive Design**

### **Breakpoint System**
```css
/* Tailwind CSS Breakpoints */
/* sm: 640px and up */
/* md: 768px and up */
/* lg: 1024px and up */
/* xl: 1280px and up */
/* 2xl: 1536px and up */
```

### **Mobile-First Approach**
```tsx
// Example: Responsive Grid Layout
<div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 xl:grid-cols-4 gap-4">
  {/* Cards will stack on mobile, 2 columns on tablet, 3 on desktop, 4 on large screens */}
</div>

// Example: Responsive Navigation
<nav className="hidden md:flex space-x-4">
  {/* Desktop navigation */}
</nav>
<button className="md:hidden">
  {/* Mobile menu button */}
</button>
```

## ♿ **Accessibility Features**

### **Keyboard Navigation**
```tsx
// Focus management
const focusTrap = useFocusTrap();

// Keyboard shortcuts
useEffect(() => {
  const handleKeyDown = (e: KeyboardEvent) => {
    if (e.key === 'Escape') {
      onClose();
    }
  };
  
  document.addEventListener('keydown', handleKeyDown);
  return () => document.removeEventListener('keydown', handleKeyDown);
}, [onClose]);
```

### **Screen Reader Support**
```tsx
// ARIA labels and descriptions
<button
  aria-label="Emergency stop"
  aria-describedby="emergency-stop-description"
  onClick={onEmergencyStop}
>
  STOP
</button>
<div id="emergency-stop-description" className="sr-only">
  Press this button to immediately stop all robot movement
</div>
```

## 🧪 **Testing Strategy**

### **Unit Tests**
```tsx
// Example: Button Component Test
describe('PrimaryButton', () => {
  it('renders with correct text', () => {
    render(<PrimaryButton onClick={() => {}}>Click me</PrimaryButton>);
    expect(screen.getByText('Click me')).toBeInTheDocument();
  });

  it('calls onClick when clicked', () => {
    const handleClick = jest.fn();
    render(<PrimaryButton onClick={handleClick}>Click me</PrimaryButton>);
    fireEvent.click(screen.getByText('Click me'));
    expect(handleClick).toHaveBeenCalledTimes(1);
  });

  it('is disabled when disabled prop is true', () => {
    render(<PrimaryButton onClick={() => {}} disabled>Click me</PrimaryButton>);
    expect(screen.getByText('Click me')).toBeDisabled();
  });
});
```

### **Integration Tests**
```tsx
// Example: Control Panel Integration Test
describe('ControlPanel', () => {
  it('sends move command when directional button is clicked', async () => {
    const mockApi = jest.fn();
    render(<ControlPanel onMove={mockApi} />);
    
    fireEvent.click(screen.getByLabelText('Move forward'));
    
    expect(mockApi).toHaveBeenCalledWith('forward');
  });
});
```

## 📦 **Component Export**

### **Index File**
```tsx
// components/index.ts
export { PrimaryButton } from './buttons/PrimaryButton';
export { EmergencyStopButton } from './buttons/EmergencyStopButton';
export { DirectionalControl } from './controls/DirectionalControl';
export { SpeedControl } from './controls/SpeedControl';
export { StatusCard } from './status/StatusCard';
export { AlertBanner } from './status/AlertBanner';
export { TelemetryChart } from './data/TelemetryChart';
export { DataTable } from './data/DataTable';
export { Spinner } from './utils/Spinner';
export { Modal } from './utils/Modal';
```

## 🔄 **Changelog**

**v1.0 (2024-12-19)**
- ✅ Created core component library
- ✅ Implemented design system integration
- ✅ Added safety-first components
- ✅ Implemented responsive design
- ✅ Added accessibility features
- ✅ Created testing strategy

---

**Next Steps:**
1. Implement remaining components
2. Add animation system
3. Create component documentation
4. Set up Storybook for component development
5. Add performance optimization
