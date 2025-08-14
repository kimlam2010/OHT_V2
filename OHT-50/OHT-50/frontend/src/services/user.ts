export type UserSettings = {
	username: string
	display_name?: string
	email: string
	theme: 'light'|'dark'
}

export async function getUserSettings(username: string): Promise<UserSettings|null>{
	const res = await fetch(`/api/v1/user/settings/${encodeURIComponent(username)}`)
	if (!res.ok) return null
	return res.json()
}

export async function updateUserSettings(username: string, settings: UserSettings): Promise<UserSettings|null>{
	const res = await fetch(`/api/v1/user/settings/${encodeURIComponent(username)}` ,{
		method: 'PUT',
		headers: { 'Content-Type': 'application/json' },
		body: JSON.stringify(settings)
	})
	if (!res.ok) return null
	return res.json()
}


