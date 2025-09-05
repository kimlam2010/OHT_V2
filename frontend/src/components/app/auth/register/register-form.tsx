import { zodResolver } from '@hookform/resolvers/zod'
import { Link, useNavigate } from '@tanstack/react-router'
import { Lock, Mail, ShieldCheck, User } from 'lucide-react'
import { useForm } from 'react-hook-form'
import toast from 'react-hot-toast'
import { z } from 'zod'
import { Button } from '@/components/ui/button'
import {
  Card,
  CardContent,
  CardDescription,
  CardFooter,
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
import { useRegister } from '@/hooks/auth'
import { HTTPError } from '@/types/error'

const formSchema = z.object({
  email: z.string().nonempty('Email is required').email('Invalid email address'),
  username: z.string().nonempty('Username is required'),
  password: z.string().nonempty('Password is required'),
  confirmPassword: z.string().nonempty('Confirm password is required'),
}).refine((data) => {
  return data.password === data.confirmPassword
}, {
  message: 'Passwords do not match',
  path: ['confirmPassword'],
})

export function RegisterForm() {
  const { mutate: register } = useRegister()
  const navigate = useNavigate()
  const form = useForm<z.infer<typeof formSchema>>({
    resolver: zodResolver(formSchema),
    defaultValues: {
      email: '',
      username: '',
      password: '',
      confirmPassword: '',
    },
  })

  const onSubmit = (values: z.infer<typeof formSchema>) => {
    register(values, {
      onSuccess: () => {
        toast.success('Register successful')
        navigate({ to: '/login' })
      },
      onError: (error) => {
        if (error instanceof HTTPError) {
          toast.error(error.message)
        }
        else {
          toast.error('Register failed')
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
          Register to OHT-50 system
        </CardDescription>
      </CardHeader>
      <CardContent>
        <Form {...form}>
          <form onSubmit={form.handleSubmit(onSubmit)} className="space-y-6">
            <FormField
              control={form.control}
              name="email"
              render={({ field }) => (
                <FormItem>
                  <FormLabel>Email</FormLabel>
                  <FormControl>
                    <div className="relative">
                      <Input
                        type="email"
                        placeholder="Email"
                        {...field}
                        className="pl-9 rounded-full shadow-sm"
                      />
                      <Mail className="size-4 absolute left-2.5 top-1/2 -translate-y-1/2 text-gray-500" />
                    </div>
                  </FormControl>
                  <FormMessage />
                </FormItem>
              )}
            />
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
            <FormField
              control={form.control}
              name="confirmPassword"
              render={({ field }) => (
                <FormItem>
                  <FormLabel>Confirm Password</FormLabel>
                  <FormControl>
                    <div className="relative">
                      <Input
                        type="password"
                        placeholder="Confirm Password"
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
            <Button type="submit" className="w-full shadow-sm">
              Register
            </Button>
          </form>
        </Form>
      </CardContent>
      <CardFooter className="flex items-center justify-center">
        <span className="text-sm text-gray-500 flex items-center gap-2">
          Already have an account?
          <Link className="text-primary font-bold" to="/login">Login</Link>
        </span>
      </CardFooter>
    </Card>
  )
}
