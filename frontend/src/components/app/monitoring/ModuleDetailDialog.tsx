import type { ReactNode } from 'react'
import type { ModuleData, TelemetryRegister } from '@/types'
import { Pen } from 'lucide-react'
import { useState } from 'react'
import { ErrorMessage } from '@/components/shared/ErrorMessage'
import { Button } from '@/components/ui/button'
import { Dialog, DialogContent, DialogDescription, DialogHeader, DialogTitle } from '@/components/ui/dialog'
import {
  Table,
  TableBody,
  TableCell,
  TableHead,
  TableHeader,
  TableRow,
} from '@/components/ui/table'
import { Tabs, TabsContent, TabsList, TabsTrigger } from '@/components/ui/tabs'
import { useTelemetryQuery } from '@/hooks/module'
import { matchQuery } from '@/lib/match-query'
import EditValueRegisterDialog from './EditValueRegisterDialog'
import { ModuleBadge } from './ModulesCard'

interface Props {
  open: boolean
  setOpen: (open: boolean) => void
  module: ModuleData
}

interface ModuleDetail {
  name: string
  value: string | number | ReactNode
  classNameValue?: string
}

function TelemetryTable({ module }: { module: ModuleData }) {
  const [openEditValueRegisterDialog, setOpenEditValueRegisterDialog] = useState(false)
  const [selectedRegister, setSelectedRegister] = useState<TelemetryRegister | null>(null)
  const telemetryQuery = useTelemetryQuery(module.address)

  const handleEditValueRegister = (register: TelemetryRegister) => {
    setSelectedRegister(register)
    setOpenEditValueRegisterDialog(true)
  }

  return (
    <Table className="text-base border">
      <TableHeader>
        <TableRow className="divide-x">
          <TableHead className="font-medium text-center w-1/7 bg-muted">Addr</TableHead>
          <TableHead className="w-1/5 font-medium text-center bg-muted">Name</TableHead>
          <TableHead className="w-1/5 font-medium text-center bg-muted">Value</TableHead>
          <TableHead className="font-medium text-center w-1/10 bg-muted">Unit</TableHead>
          <TableHead className="font-medium text-center w-1/10 bg-muted">Actions</TableHead>
        </TableRow>
      </TableHeader>
      <TableBody>
        {matchQuery(telemetryQuery, {
          error: error => (
            <TableRow>
              <TableCell colSpan={5} className="py-4 text-center">
                <ErrorMessage error={error} />
              </TableCell>
            </TableRow>
          ),
          loading: () => (
            <TableRow>
              <TableCell colSpan={5} className="py-4 text-center">Loading...</TableCell>
            </TableRow>
          ),
          empty: () => {
            return (
              <TableRow>
                <TableCell colSpan={5} className="py-4 text-center">No data</TableCell>
              </TableRow>
            )
          },
          success: (data) => {
            return data.data.registers.map(register => (
              <TableRow key={register.address} className="text-center divide-x">
                <TableCell className="font-mono font-medium text-muted-foreground">{register.address}</TableCell>
                <TableCell className="font-medium">{register.name}</TableCell>
                <TableCell>{register.value}</TableCell>
                <TableCell>{register.unit || 'N/A'}</TableCell>
                <TableCell className="flex justify-center items-center">
                  {register.mode === 'RW' || register.mode === 'W'
                    ? (
                        <Button
                          variant="ghost"
                          size="icon"
                          onClick={() => handleEditValueRegister(register)}
                        >
                          <Pen className="size-4" />
                        </Button>
                      )
                    : <></>}
                </TableCell>
              </TableRow>
            ))
          },
        })}
      </TableBody>
      {selectedRegister && <EditValueRegisterDialog open={openEditValueRegisterDialog} setOpen={setOpenEditValueRegisterDialog} register={selectedRegister} moduleAddress={module.address} />}
    </Table>
  )
}

export default function ModuleDetailDialog({ open, setOpen, module }: Props) {
  const moduleDetails: ModuleDetail[] = [
    {
      name: 'Module Name',
      value: module.name,
      classNameValue: 'font-medium uppercase text-primary',
    },
    {
      name: 'Address',
      value: module.addr,
      classNameValue: 'text-muted-foreground font-mono font-medium',
    },
    {
      name: 'Status',
      value: <ModuleBadge status={module.status} />,
    },
    {
      name: 'Type',
      value: module.type,
    },
    {
      name: 'Error Rate',
      value: `${(module.error_rate).toFixed(2)}%`,
    },
    {
      name: 'Last Seen',
      value: module.last_seen,
    },
    {
      name: 'Version',
      value: module.fw_version,
    },
  ]
  return (
    <Dialog open={open} onOpenChange={setOpen}>
      <DialogContent className="sm:min-w-[800px] min-w-3/4 max-h-3/4 min-h-1/2 content-start items-start">
        <DialogHeader>
          <DialogTitle>
            Module Details
          </DialogTitle>
          <DialogDescription className="sr-only"></DialogDescription>
        </DialogHeader>
        <div className="px-2">
          <Tabs defaultValue="overview">
            <TabsList className="mb-2">
              <TabsTrigger value="overview">Overview</TabsTrigger>
              <TabsTrigger value="telemetry">Telemetry</TabsTrigger>
            </TabsList>
            <TabsContent value="overview">
              <Table className="border">
                <TableBody>
                  {moduleDetails.map(item => (
                    <TableRow key={item.name} className="text-base">
                      <TableCell className="font-medium bg-muted sm:w-[200px] w-1/2">{item.name}</TableCell>
                      <TableCell className={item.classNameValue}>{item.value}</TableCell>
                    </TableRow>
                  ))}
                </TableBody>
              </Table>
            </TabsContent>
            <TabsContent value="telemetry">
              <TelemetryTable module={module} />
            </TabsContent>
          </Tabs>
        </div>
      </DialogContent>
    </Dialog>
  )
}
