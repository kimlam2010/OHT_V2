import dayjs from 'dayjs'
import { CartesianGrid, Line, LineChart, ResponsiveContainer, Tooltip, XAxis, YAxis } from 'recharts'
import { cn } from '@/lib/utils'

interface CPUData {
  timestamp: number
  cpu: number
}

const data: CPUData[] = [
  { timestamp: dayjs('2024-01-15T08:00:01').unix(), cpu: 32.1 },
  { timestamp: dayjs('2024-01-15T08:00:02').unix(), cpu: 45.8 },
  { timestamp: dayjs('2024-01-15T08:00:05').unix(), cpu: 78.2 }, // Gap - missing 3,4
  { timestamp: dayjs('2024-01-15T08:00:06').unix(), cpu: 65.3 },
  { timestamp: dayjs('2024-01-15T08:00:07').unix(), cpu: 42.7 },
  { timestamp: dayjs('2024-01-15T08:00:10').unix(), cpu: 38.9 }, // Gap - missing 8,9
  { timestamp: dayjs('2024-01-15T08:00:11').unix(), cpu: 55.4 },
  { timestamp: dayjs('2024-01-15T08:00:15').unix(), cpu: 72.1 }, // Gap - missing 12,13,14
  { timestamp: dayjs('2024-01-15T08:00:16').unix(), cpu: 89.5 },
  { timestamp: dayjs('2024-01-15T08:00:17').unix(), cpu: 67.8 },
  { timestamp: dayjs('2024-01-15T08:00:18').unix(), cpu: 58.3 },
  { timestamp: dayjs('2024-01-15T08:00:19').unix(), cpu: 74.6 },
  { timestamp: dayjs('2024-01-15T08:00:20').unix(), cpu: 39.2 },
  { timestamp: dayjs('2024-01-15T08:00:21').unix(), cpu: 28.7 },
  { timestamp: dayjs('2024-01-15T08:00:22').unix(), cpu: 41.3 },
  { timestamp: dayjs('2024-01-15T08:00:23').unix(), cpu: 52.6 },
  { timestamp: dayjs('2024-01-15T08:00:24').unix(), cpu: 47.1 },
  { timestamp: dayjs('2024-01-15T08:00:25').unix(), cpu: 39.8 },
  { timestamp: dayjs('2024-01-15T08:00:26').unix(), cpu: 55.4 },
  { timestamp: dayjs('2024-01-15T08:00:27').unix(), cpu: 72.1 },
]
export default function CpuUsageChart() {
  const POINT_WIDTH = 30
  const totalWidth = data.length * POINT_WIDTH

  return (
    <div className="w-full h-56 overflow-x-auto">
      <div className={cn('h-full min-w-full', `w-[${totalWidth}px]`)}>
        <ResponsiveContainer>
          <LineChart
            data={data}
            margin={{ top: 5, right: 10, left: 10, bottom: 0 }}
          >
            <CartesianGrid strokeDasharray="1 0" stroke="#e5e7eb" />
            <XAxis
              dataKey="timestamp"
              type="number"
              domain={['dataMin', 'dataMax']}
              interval={0}
              tickCount={15}
              includeHidden={true}
              tick={true}
              tickLine={false}
              tickFormatter={() => ''}
              height={5}
            />
            <YAxis
              className="static"
              tickCount={6}
              ticks={[0, 20, 40, 60, 80, 100]}
              label={{
                value: 'CPU Usage (%)',
                angle: -90,
                position: 'insideLeft',
                offset: 0,
                style: {
                  textAnchor: 'middle',
                },
              }}
            />
            <Tooltip
              labelFormatter={value => dayjs.unix(value).format('HH:mm:ss')}
              formatter={value => [`${value}%`, 'CPU Usage']}
            />
            <Line
              isAnimationActive={false}
              type="linear"
              dataKey="cpu"
              stroke="#8884d8"
              strokeWidth={1}
              dot={true}
            />
          </LineChart>
        </ResponsiveContainer>
      </div>
    </div>
  )
}
