import { apiFetchJson } from './http'

export type UserSettings = {
	username: string
	display_name?: string
	email: string
	theme: 'light'|'dark'
}

export async function getUserSettings(username: string): Promise<UserSettings|null>{
	const r = await apiFetchJson<UserSettings>(`/api/v1/user/settings/${encodeURIComponent(username)}`)
	return r.ok ? r.data : null
}

export async function updateUserSettings(username: string, settings: UserSettings): Promise<UserSettings|null>{
	const r = await apiFetchJson<UserSettings>(`/api/v1/user/settings/${encodeURIComponent(username)}` ,{
		method: 'PUT',
		body: JSON.stringify(settings)
	})
	return r.ok ? r.data : null
}


