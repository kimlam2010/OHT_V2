import { Loader2 } from 'lucide-react'

export default function PendingComponent() {
  return (
    <div className="flex items-center justify-center ">
      <div className="flex items-center justify-center gap-2 bg-background size-full">
        <Loader2 className="size-6 animate-spin text-primary" />
        <p className="text-sm text-muted-foreground">Loading...</p>
      </div>
    </div>
  )
}
