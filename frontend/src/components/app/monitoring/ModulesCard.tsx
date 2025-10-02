/* eslint-disable react/no-array-index-key */
import type { ModuleData } from '@/types'
import { useMemo, useState } from 'react'
import { Badge } from '@/components/ui/badge'
import {
  Card,
  CardContent,
  CardHeader,
  CardTitle,
} from '@/components/ui/card'
import { Skeleton } from '@/components/ui/skeleton'
import { useModulesQuery } from '@/hooks/module'
import { matchQuery } from '@/lib/match-query'
import ModuleDetailDialog from './ModuleDetailDialog'

function ModulesSkeleton() {
  return (
    <div className="p-2 space-y-2 w-full rounded-md border shadow-sm">
      <div className="flex gap-4 justify-between items-center">
        <div className="space-y-1.5 h-10">
          <Skeleton className="w-20 h-3.5" />
          <Skeleton className="w-10 h-3.5" />
        </div>
        <Skeleton className="w-16 h-[calc(var(--spacing)*5.5)]" />
      </div>
      <div className="grid grid-cols-2 gap-2">
        <div className="flex flex-col items-center">
          <p className="text-sm text-muted-foreground">Last seen</p>
          <div className="h-6 flex">
            <Skeleton className="my-auto w-7 h-3.5" />
          </div>
        </div>
        <div className="flex flex-col items-center">
          <p className="text-sm text-muted-foreground">FW Version</p>
          <div className="h-6 flex">
            <Skeleton className="my-auto w-7 h-3.5" />
          </div>
        </div>
        <div className="flex flex-col items-center">
          <p className="text-sm text-muted-foreground">Error Rate</p>
          <div className="h-6 flex">
            <Skeleton className="my-auto w-7 h-3.5" />
          </div>
        </div>
        <div className="flex flex-col items-center">
          <p className="text-sm text-muted-foreground">Response P95</p>
          <div className="h-6 flex">
            <Skeleton className="my-auto w-7 h-3.5" />
          </div>
        </div>
      </div>
    </div>
  )
}

export function ModuleBadge({ status }: { status: ModuleData['status'] }) {
  return (
    <Badge className="text-xs uppercase rounded-full" variant={status === 'healthy' ? 'success' : status === 'warning' ? 'warning' : status === 'error' ? 'destructive' : status === 'offline' ? 'secondary' : 'outline'}>
      {status}
    </Badge>
  )
}

function ModuleItem({ module, onClick }: { module: ModuleData, onClick: (moduleAddress: number) => void }) {
  return (
    <div className="p-2 space-y-2 w-full rounded-md border shadow-sm cursor-pointer" onClick={() => onClick(module.address)}>
      <div className="flex gap-4 justify-between items-center">
        <div>
          <p className="text-sm font-medium">{module.name}</p>
          <p className="font-mono text-sm font-medium text-muted-foreground">{module.addr}</p>
        </div>
        <ModuleBadge status={module.status} />
      </div>
      <div className="grid grid-cols-2 gap-2">
        <div className="flex flex-col items-center">
          <p className="text-sm text-muted-foreground">Last seen</p>
          <p className="font-medium">{module.last_seen}</p>
        </div>
        <div className="flex flex-col items-center">
          <p className="text-sm text-muted-foreground">FW Version</p>
          <p className="font-medium">{module.fw_version}</p>
        </div>
        <div className="flex flex-col items-center">
          <p className="text-sm text-muted-foreground">Error Rate</p>
          <p className="font-medium">
            {module.error_rate.toFixed(2)}
            %
          </p>
        </div>
        <div className="flex flex-col items-center">
          <p className="text-sm text-muted-foreground hidden xl:block">Response P95</p>
          <p className="text-sm text-muted-foreground block xl:hidden">Res P95</p>
          <p className="font-medium">
            {module.response_time}
            {' '}
            ms
          </p>
        </div>
      </div>
    </div>
  )
}

export default function ModulesCard() {
  const [open, setOpen] = useState(false)
  const [moduleAddress, setModuleAddress] = useState<number | null>(null)

  const modulesQuery = useModulesQuery()

  const handleOpenModuleDetailDialog = (moduleAddress: number) => {
    setModuleAddress(moduleAddress)
    setOpen(true)
  }

  const module = useMemo(() => {
    return modulesQuery.data?.data.find(module => module.address === moduleAddress)
  }, [modulesQuery.data, moduleAddress])

  return (
    <>
      <Card className="gap-3 h-full shadow">
        <CardHeader>
          <CardTitle>RS485 Modules</CardTitle>
        </CardHeader>
        <CardContent>
          {matchQuery(modulesQuery, {
            loading: () => (
              <div className="grid grid-cols-2 gap-4  xl:grid-cols-4">
                {Array.from({ length: 7 }).map((_, index) => (
                  <ModulesSkeleton key={index} />
                ))}
              </div>
            ),
            success: data => (
              <div className="grid grid-cols-2 gap-4 xl:grid-cols-4">
                {data.data.map(module => (
                  <ModuleItem
                    key={module.addr}
                    module={module}
                    onClick={handleOpenModuleDetailDialog}
                  />
                ))}
              </div>
            ),
          })}
        </CardContent>
      </Card>
      {moduleAddress !== null && module && (
        <ModuleDetailDialog open={open} setOpen={setOpen} module={module} />
      )}
    </>
  )
}
