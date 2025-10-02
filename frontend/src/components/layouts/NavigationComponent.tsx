import type { LucideIcon } from 'lucide-react'
import type { FileRouteTypes } from '@/routeTree.gen'
import { Link, useLocation } from '@tanstack/react-router'
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
  const { pathname } = useLocation()

  const isActive = (path: Paths) => pathname === path

  return (
    <SidebarGroup>
      <SidebarMenu>
        {items.map(item => (
          <SidebarMenuItem key={item.title}>
            <SidebarMenuButton asChild isActive={isActive(item.path)}>
              <Link to={item.path} className="flex items-center gap-2 data-[active=true]:!text-white data-[active=true]:!bg-primary">
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
