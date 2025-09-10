import { zodResolver } from '@hookform/resolvers/zod'
import { useNavigate } from '@tanstack/react-router'
import { Lock, LogIn, ShieldCheck, User } from 'lucide-react'
import { useForm } from 'react-hook-form'
import toast from 'react-hot-toast'
import { z } from 'zod'
import { Button } from '@/components/ui/button'
import {
  Card,
  CardContent,
  CardDescription,
  CardHeader,
  CardTitle,
} from '@/components/ui/card'
import {
  Form,
  FormControl,
  FormField,
  FormItem,
  FormLabel,
  FormMessage,
} from '@/components/ui/form'
import { Input } from '@/components/ui/input'
import { ACCESS_TOKEN } from '@/constants/string'
import { useLogin } from '@/hooks/auth'
import { HTTPError } from '@/types/error'

const formSchema = z.object({
  username: z.string().min(1, { message: 'Username is required' }),
  password: z.string().min(1, { message: 'Password is required' }),
})

export function LoginForm() {
  const { mutate: login } = useLogin()
  const navigate = useNavigate()
  const form = useForm<z.infer<typeof formSchema>>({
    resolver: zodResolver(formSchema),
    defaultValues: {
      username: '',
      password: '',
    },
  })

  const onSubmit = (values: z.infer<typeof formSchema>) => {
    login(values, {
      onSuccess: (response) => {
        toast.success('Login successful')
        localStorage.setItem(ACCESS_TOKEN, response.access_token)
        navigate({ to: '/dashboard' })
      },
      onError: (error) => {
        if (error instanceof HTTPError) {
          toast.error(error.message)
        }
        else {
          toast.error('Login failed')
        }
      },
    })
  }

  return (
    <Card className="w-full max-w-sm pt-0 overflow-hidden shadow-2xl">
      <CardHeader className="flex flex-col gap-2 items-center justify-center bg-primary py-6">
        <CardTitle className="text-2xl text-white dark:text-black flex items-center gap-2">
          <ShieldCheck className="size-6" />
          OHT-50
        </CardTitle>
        <CardDescription className="text-white dark:text-black">
          Login into OHT-50 system
        </CardDescription>
      </CardHeader>
      <CardContent>
        <Form {...form}>
          <form onSubmit={form.handleSubmit(onSubmit)} className="space-y-6">
            <FormField
              control={form.control}
              name="username"
              render={({ field }) => (
                <FormItem>
                  <FormLabel>Username</FormLabel>
                  <FormControl>
                    <div className="relative">
                      <Input
                        placeholder="Username"
                        {...field}
                        className="pl-9 rounded-full shadow-sm"
                      />
                      <User className="size-4 absolute left-2.5 top-1/2 -translate-y-1/2 text-gray-500" />
                    </div>
                  </FormControl>
                  <FormMessage />
                </FormItem>
              )}
            />
            <FormField
              control={form.control}
              name="password"
              render={({ field }) => (
                <FormItem>
                  <FormLabel>Password</FormLabel>
                  <FormControl>
                    <div className="relative">
                      <Input
                        type="password"
                        placeholder="Password"
                        {...field}
                        className="pl-9 rounded-full shadow-sm"
                      />
                      <Lock className="size-4 absolute left-2.5 top-1/2 -translate-y-1/2 text-gray-500" />
                    </div>
                  </FormControl>
                  <FormMessage />
                </FormItem>
              )}
            />
            <Button type="submit" className="w-full flex items-center gap-2 shadow-sm">
              <LogIn />
              Login to system
            </Button>
          </form>
        </Form>
      </CardContent>
    </Card>
  )
}
