import type { ReactNode } from 'react'
import { cn } from '@/lib/utils'

interface Props {
  /**
   * Whether to add vertical spacing between children
   * @default true
   */
  spacing?: boolean
  class?: string
  children: ReactNode
}

export default function PageContainer({ spacing = true, class: className, children }: Props) {
  return (
    <div className="p-4 w-full h-full">
      <div className={cn(spacing && 'space-y-4', className)}>
        {children}
      </div>
    </div>
  )
}
