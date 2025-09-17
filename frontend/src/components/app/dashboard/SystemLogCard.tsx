import type { LogItem } from '@/types'
import { AlertCircle } from 'lucide-react'
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
            <TableCell className="font-medium px-2">
              <Skeleton className="h-[calc(var(--spacing)*5.5)] w-44" />
            </TableCell>
            <TableCell className="w-fit justify-start px-2">
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

function ErrorMessage({ error }: { error: Error }) {
  return (
    <div className=" flex gap-2 items-center text-destructive">
      <AlertCircle className="size-6" />
      <p>Error: </p>
      {error.message}
    </div>
  )
}

function LevelBadge({ level }: { level: string }) {
  if (level === 'info') {
    return <Badge variant="outline" className="bg-blue-500 text-white">Info</Badge>
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
              <TableCell className="font-medium text-muted-foreground text-sm font-mono">{formatDate(log.timestamp)}</TableCell>
              <TableCell className=" uppercase"><LevelBadge level={log.level} /></TableCell>
              <TableCell className="text-sm font-mono">{log.message}</TableCell>
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
    <Card>
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
          error: error => <ErrorMessage error={error} />,
          success: data => <SystemLogTable logs={data.logs} />,
        })}
      </CardContent>

      <CardFooter>
        <div className="flex justify-end items-center w-full gap-2">
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
