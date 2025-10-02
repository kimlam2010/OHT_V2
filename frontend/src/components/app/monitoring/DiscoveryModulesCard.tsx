import { CheckCircle2, CircleAlertIcon, Search } from 'lucide-react'
import { useMemo, useState } from 'react'
import { Button } from '@/components/ui/button'
import { Progress } from '@/components/ui/progress'
import { useDiscoveryResultQuery, useDiscoveryStatusQuery, useStartDiscoveryMutation } from '@/hooks/module'
import { matchQuery } from '@/lib/match-query'
import { cn } from '@/lib/utils'

export default function DiscoveryModulesCard() {
  const [discoveryEnabled, setDiscoveryEnabled] = useState(false)
  const discoveryResult = useDiscoveryResultQuery({ doNotShowLoading: true })
  const { mutate: startDiscovery } = useStartDiscoveryMutation()
  const discoveryStatus = useDiscoveryStatusQuery(discoveryEnabled, 500, { doNotShowLoading: true })

  const progress = useMemo(() => {
    if (discoveryStatus.data && discoveryStatus.data.data.progress) {
      if (discoveryStatus.data.data.progress === 100) {
        setDiscoveryEnabled(false)
        discoveryResult.refetch()
      }
      return discoveryStatus.data.data.progress
    }
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [discoveryStatus.data])

  const handleStartDiscovery = () => {
    startDiscovery({ doNotShowLoading: true }, {
      onSuccess: () => {
        setDiscoveryEnabled(true)
      },
    })
  }
  return (
    <div className="p-4 space-y-4 rounded-md border">
      <div className="font-semibold leading-none">
        Module Discovery
      </div>
      <Button size="sm" className="flex gap-2 items-center text-xs" onClick={() => handleStartDiscovery()}>
        <Search />
        Scan now
      </Button>
      <div>
        {
          discoveryStatus.data && progress !== null
          && (
            <>
              <Progress value={progress} className="h-1" />
              <span className="text-xs text-muted-foreground">
                {discoveryStatus.data.data.status_message}
              </span>
            </>
          )
        }
      </div>
      {
        progress === 100 && (
          matchQuery(discoveryResult, {
            success: (data) => {
              return (
                <div className="grid grid-cols-2 gap-2 text-xs font-normal text-white">
                  {data.data.map(item => (
                    <div key={item.addr} className={cn('p-2 rounded-md flex items-center gap-2', item.conflict ? 'bg-destructive/80' : 'bg-success')}>
                      {!item.conflict ? <CheckCircle2 className="size-4" /> : <CircleAlertIcon className="size-4" />}
                      <span className="font-normal">
                        {item.addr}
                      </span>
                      -
                      <span className="font-medium">
                        {item.name}
                      </span>
                    </div>
                  ))}
                </div>
              )
            },
          })
        )
      }
    </div>
  )
}
