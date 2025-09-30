import type { Item } from './NavigationComponent'
import { ChartBarBig, Monitor } from 'lucide-react'

export const menus: Item[] = [
  {
    title: 'Dashboard',
    path: '/dashboard',
    icon: ChartBarBig,
  },
  {
    title: 'Monitoring',
    path: '/monitoring',
    icon: Monitor,
  },
]

export { menus as routes }
