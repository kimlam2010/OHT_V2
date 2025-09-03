import type { ReactNode } from 'react'
import ReactQuery from './react-query'
import TanstackDevtoolsCustom from './tanstack-devtools'
import { ToastProvider } from './toast-provider'

function Plugins({ children }: { children: ReactNode }) {
  return (
    <>
      <ReactQuery>
        {children}
      </ReactQuery>
      <TanstackDevtoolsCustom />
      <ToastProvider />
    </>
  )
}

export default Plugins
