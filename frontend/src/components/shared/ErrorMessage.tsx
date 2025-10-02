import { AlertCircle } from 'lucide-react'

export function ErrorMessage({ error }: { error: Error }) {
  return (
    <div className="flex flex-col gap-2 justify-center items-center p-2 w-full text-destructive">
      <AlertCircle className="size-6" />
      <p>
        {error.message}
      </p>
    </div>
  )
}
