import type { LogItem } from '@/types'
import { useState } from 'react'
import { Badge } from '@/components/ui/badge'
import { Button } from '@/components/ui/button'
import {
  Card,
  CardAction,
  CardContent,
  CardFooter,
  CardHeader,
  CardTitle,
} from '@/components/ui/card'
import {
  Select,
  SelectContent,
  SelectItem,
  SelectTrigger,
  SelectValue,
} from '@/components/ui/select'
import { Skeleton } from '@/components/ui/skeleton'
import {
  Table,
  TableBody,
  TableCell,
  TableRow,
} from '@/components/ui/table'
import { useSystemLogQuery } from '@/hooks'
import { formatDate } from '@/lib/date'
import { matchQuery } from '@/lib/match-query'

function Spinner() {
  return (
    <Table>
      <TableBody>
        {Array.from({ length: 8 }).map((_, index) => (
          // eslint-disable-next-line react/no-array-index-key
          <TableRow key={`skeleton-row-${index}`}>
            <TableCell className="px-2 font-medium">
              <Skeleton className="h-[calc(var(--spacing)*5.5)] w-44" />
            </TableCell>
            <TableCell className="justify-start px-2 w-fit">
              <Skeleton className="h-[calc(var(--spacing)*5.5)] w-24" />
            </TableCell>
            <TableCell>
              <Skeleton className="h-[calc(var(--spacing)*5.5)] w-44" />
            </TableCell>
          </TableRow>
        ))}
      </TableBody>
    </Table>
  )
}

function LevelBadge({ level }: { level: string }) {
  if (level === 'info') {
    return <Badge variant="outline" className="text-white bg-blue-500">Info</Badge>
  }
  if (level === 'warning') {
    return <Badge variant="warning">Warning</Badge>
  }
  if (level === 'error') {
    return <Badge variant="destructive">Error</Badge>
  }
  if (level === 'debug') {
    return <Badge variant="success">Debug</Badge>
  }
  return <Badge variant="outline">{level}</Badge>
}

function SystemLogTable({ logs }: { logs: LogItem[] }) {
  return (
    <div className="max-h-[calc(var(--spacing)*77)] overflow-y-auto">
      <Table>
        <TableBody>
          {logs.map(log => (
            <TableRow key={log.id}>
              <TableCell className="font-mono text-sm font-medium text-muted-foreground">{formatDate(log.timestamp)}</TableCell>
              <TableCell className="uppercase"><LevelBadge level={log.level} /></TableCell>
              <TableCell className="font-mono text-sm">{log.message}</TableCell>
            </TableRow>
          ))}
        </TableBody>
      </Table>
    </div>
  )
}

export default function SystemLogCard() {
  const [limit, setLimit] = useState(10)
  const [level, setLevel] = useState('all')
  const [page, setPage] = useState(1)
  const systemLogQuery = useSystemLogQuery(limit, level, page, { staleTime: 0 })

  const handleLimitChange = (value: string) => {
    setLimit(Number(value))
    setPage(1)
  }

  return (
    <Card className="gap-3 shadow">
      <CardHeader>
        <CardTitle>System Log</CardTitle>
        <CardAction className="flex gap-3">
          <Button className="!rounded-none p-1.5 h-fit text-sm !border" variant={level === 'all' ? 'default' : 'outline'} onClick={() => setLevel('all')}>
            All
          </Button>
          <Button className="!rounded-none p-1.5 h-fit text-sm !border" variant={level === 'info' ? 'default' : 'outline'} onClick={() => setLevel('info')}>
            Info
          </Button>
          <Button className="!rounded-none p-1.5 h-fit text-sm !border" variant={level === 'warning' ? 'default' : 'outline'} onClick={() => setLevel('warning')}>
            Warning
          </Button>
          <Button className="!rounded-none p-1.5 h-fit text-sm !border" variant={level === 'error' ? 'default' : 'outline'} onClick={() => setLevel('error')}>
            Error
          </Button>
          <Button className="!rounded-none p-1.5 h-fit text-sm !border" variant={level === 'debug' ? 'default' : 'outline'} onClick={() => setLevel('debug')}>
            Debug
          </Button>
        </CardAction>
      </CardHeader>
      <CardContent>
        {matchQuery(systemLogQuery, {
          loading: () => <Spinner />,
          success: data => <SystemLogTable logs={data.logs} />,
        })}
      </CardContent>

      <CardFooter>
        <div className="flex gap-2 justify-end items-center w-full">
          <div className="text-sm text-muted-foreground">
            Total:
            {' '}
            {systemLogQuery.data?.total_count || 0}
            {' '}
            logs
          </div>
          <Select value={limit.toString()} onValueChange={handleLimitChange}>
            <SelectTrigger className="w-fit">
              <SelectValue placeholder="Limit" />
            </SelectTrigger>
            <SelectContent>
              <SelectItem value="10">10</SelectItem>
              <SelectItem value="20">20</SelectItem>
              <SelectItem value="50">50</SelectItem>
            </SelectContent>
          </Select>
          <Button variant="outline" size="sm" onClick={() => setPage(page - 1)} disabled={page === 1}>
            Prev
          </Button>
          <Button variant="outline" size="sm" onClick={() => setPage(page + 1)} disabled={!systemLogQuery.data?.has_next}>
            Next
          </Button>
        </div>
      </CardFooter>
    </Card>
  )
}
