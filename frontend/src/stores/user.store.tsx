import type { User } from '@/types'
import { create } from 'zustand'
import { persist } from 'zustand/middleware'
import { USER_STORE } from '@/constants/string'

interface UserState {
  user: User | null
  setUser: (user: User) => void
  getUser: () => User | null
  clearUser: () => void
}

export const useUserStore = create<UserState>()(
  persist(
    (set, get) => ({
      user: null,
      setUser: (user: User) => set({ user }),
      getUser: () => get().user,
      clearUser: () => set({ user: null }),
    }),
    {
      name: USER_STORE,
    },
  ),
)
