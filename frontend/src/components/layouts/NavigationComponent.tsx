import type { LucideIcon } from 'lucide-react'
import type { FileRouteTypes } from '@/routeTree.gen'
import { Link } from '@tanstack/react-router'
import { SidebarGroup, SidebarMenu, SidebarMenuButton, SidebarMenuItem } from '@/components/ui/sidebar'

type Paths = FileRouteTypes['to']

export interface Item {
  title: string
  path: Paths
  icon: LucideIcon
  isActive?: boolean
}

interface Props {
  items: Item[]
}

export function Navigation({ items }: Props) {
  return (
    <SidebarGroup>
      <SidebarMenu>
        {items.map(item => (
          <SidebarMenuItem key={item.title}>
            <SidebarMenuButton asChild isActive={item.isActive}>
              <Link to={item.path} className="flex items-center gap-2">
                {item.icon && <item.icon className="size-4" />}
                <span className="text-base">{item.title}</span>
              </Link>
            </SidebarMenuButton>
          </SidebarMenuItem>
        ))}
      </SidebarMenu>
    </SidebarGroup>
  )
}
