import type { Item } from './NavigationComponent'
import { ChartBarBig } from 'lucide-react'

export const menus: Item[] = [
  {
    title: 'Dashboard',
    path: '/dashboard',
    icon: ChartBarBig,
  },
]

export { menus as routes }
