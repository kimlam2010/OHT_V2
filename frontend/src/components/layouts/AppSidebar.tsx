import { Activity, LogOut } from 'lucide-react'
import { Button } from '@/components/ui/button'
import { Sidebar, SidebarContent, SidebarFooter, SidebarHeader, SidebarMenu, SidebarMenuButton, SidebarMenuItem } from '@/components/ui/sidebar'
import { menus } from './menu'
import { Navigation } from './Navigation'

const defaultProps = { collapsible: 'icon' as const }

export default function AppSidebar({ props = defaultProps }: { props?: React.ComponentProps<typeof Sidebar> }) {
  return (
    <Sidebar {...props} className="bg-popover">
      <SidebarHeader className="py-2">
        <SidebarMenu>
          <SidebarMenuItem>
            <SidebarMenuButton size="lg" asChild>
              <div className="flex items-center gap-3 py-2 w-full !text-primary bg-primary/10">
                <div className="flex items-center justify-center rounded-lg aspect-square size-8">
                  <Activity className="size-6" />
                </div>
                <div className="flex flex-col gap-0.5">
                  <span className="font-semibold leading-none whitespace-nowrap text-lg">OHT-50</span>
                </div>
              </div>
            </SidebarMenuButton>
          </SidebarMenuItem>
        </SidebarMenu>
      </SidebarHeader>
      <SidebarContent>
        <Navigation items={menus} />
      </SidebarContent>
      <SidebarFooter>
        <Button variant="outline" className="w-full !text-destructive bg-destructive/10 hover:bg-destructive/20 border-destructive/20">
          <LogOut className="size-4" />
          <span>Logout</span>
        </Button>
      </SidebarFooter>
    </Sidebar>
  )
}
