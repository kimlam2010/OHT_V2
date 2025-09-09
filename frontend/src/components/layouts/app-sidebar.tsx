import { Bot } from 'lucide-react'
import { Sidebar, SidebarContent, SidebarHeader, SidebarMenu, SidebarMenuButton, SidebarMenuItem } from '@/components/ui/sidebar'
import { menus } from './menu'
import { Navigation } from './navigation'

const defaultProps = { collapsible: 'icon' as const }

export default function AppSidebar({ props = defaultProps }: { props?: React.ComponentProps<typeof Sidebar> }) {
  return (
    <Sidebar {...props} className="bg-popover">
      <SidebarHeader className="py-2">
        <SidebarMenu>
          <SidebarMenuItem>
            <SidebarMenuButton size="lg" asChild>
              <div className="flex items-center gap-3 py-2 w-full">
                <div className="flex items-center justify-center bg-blue-600 rounded-lg aspect-square size-8 text-sidebar-primary-foreground">
                  <Bot className="size-4" />
                </div>
                <div className="flex flex-col gap-0.5">
                  <span className="text-sm font-semibold leading-none whitespace-nowrap">OHT-50</span>
                </div>
              </div>
            </SidebarMenuButton>
          </SidebarMenuItem>
        </SidebarMenu>
      </SidebarHeader>
      <SidebarContent>
        <Navigation items={menus} />
      </SidebarContent>
    </Sidebar>
  )
}
