import { useLocalStorage } from '@reactuses/core'
import clsx from 'clsx'
import ScrollToTopButton from '@/components/shared/ScrollToTopButton'
import { Separator } from '@/components/ui/separator'
import { SidebarInset, SidebarProvider, SidebarTrigger } from '@/components/ui/sidebar'
import AppSidebar from './AppSidebar'

export default function MainLayout({ children }: { children: React.ReactNode }) {
  const [open, setOpen] = useLocalStorage<boolean | undefined>('sidebar', undefined)

  return (
    <div>
      <SidebarProvider open={open ?? true}>
        <AppSidebar />
        <SidebarInset>
          <header
            className={clsx(
              'fixed top-0 right-0 z-10 transition-[left,right,width] duration-0 md:!duration-200 ease-linear bg-background shadow left-0',
              open ? 'md:left-64' : 'md:left-12',
            )}
          >
            <div className="flex items-center justify-between h-12 border-b shrink-0">
              <div className="flex items-center gap-2 px-4">
                <SidebarTrigger className="-ml-1" onClick={() => setOpen(prev => !prev)} />
                <Separator orientation="vertical" className="h-4 mr-2" />
              </div>
              {/* <HeaderActions className="mr-4" /> */}
            </div>
          </header>
          <main className="flex flex-col flex-1 mt-12">
            {children}
          </main>
        </SidebarInset>
        <ScrollToTopButton />
      </SidebarProvider>
    </div>
  )
}
