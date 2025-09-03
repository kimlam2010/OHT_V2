import type { ReactNode } from 'react'
import { QueryCache, QueryClient, QueryClientProvider } from '@tanstack/react-query'
import { ReactQueryDevtools } from '@tanstack/react-query-devtools'
import { HttpStatusCode } from 'axios'
import { HTTPError } from '@/types/error'

const RETRY_LIMIT = 3
const STALE_TIME = 1000 * 60 * 5 // 5 minutes
const TIMEOUT_STATUS_CODES = [
  HttpStatusCode.RequestTimeout,
  HttpStatusCode.GatewayTimeout,
]

function handleNumRetries(failureCount: number, error: Error) {
  if (error instanceof HTTPError) {
    const isUnderRetryLimit = failureCount < RETRY_LIMIT
    const isTimeout = TIMEOUT_STATUS_CODES.includes(error.status)
    return isUnderRetryLimit && isTimeout
  }

  return false
}

function ReactQuery({ children }: { children: ReactNode }) {
  const queryClient = new QueryClient({
    queryCache: new QueryCache({}),
    defaultOptions: {
      queries: {
        retry: handleNumRetries,
        refetchOnWindowFocus: false,
        staleTime: STALE_TIME,
      },
      mutations: {
        retry: handleNumRetries,
      },
    },
  })

  return (
    <QueryClientProvider client={queryClient}>
      {children}
      <ReactQueryDevtools initialIsOpen={false} />
    </QueryClientProvider>
  )
}

export default ReactQuery
