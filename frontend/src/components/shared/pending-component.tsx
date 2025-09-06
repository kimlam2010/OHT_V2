import { Loader2 } from 'lucide-react'

export default function PendingComponent() {
  return (
    <div className="bg-background size-full flex items-center justify-center gap-2">
      <Loader2 className="size-10 animate-spin text-primary" />
      <p className="text-sm text-muted-foreground">Loading...</p>
    </div>
  )
}
