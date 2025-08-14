import { describe, it, expect, beforeEach, vi } from 'vitest'
import { render, screen, fireEvent, waitFor } from '@testing-library/react'
import React from 'react'
import UserSettingsPage from '../UserSettingsPage'

vi.mock('../../../services/user', ()=>({
	getUserSettings: vi.fn(async ()=>({ username: 'operator', display_name: 'Operator', email: 'op@example.com', theme: 'light' })),
	updateUserSettings: vi.fn(async (_u: string, s: any)=> s),
}))

vi.mock('../../auth/auth', ()=>({ getCurrentUser: ()=> 'operator' }))

describe('UserSettingsPage', () =>{
	it('loads and saves settings', async () =>{
		render(<UserSettingsPage />)
		await waitFor(()=> screen.getByDisplayValue('Operator'))
		const email = screen.getByDisplayValue('op@example.com') as HTMLInputElement
		fireEvent.change(email, { target: { value: 'new@example.com' }})
		const saveBtn = screen.getByRole('button', { name: /lưu/i })
		fireEvent.click(saveBtn)
		await waitFor(()=> expect(email.value).toBe('new@example.com'))
	})
})


