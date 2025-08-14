from fastapi.testclient import TestClient


def test_user_settings_flow(client: TestClient, tmp_path):
	# Create new settings
	payload = {
		"username": "operator",
		"display_name": "Operator",
		"email": "op@example.com",
		"theme": "dark",
	}
	r = client.put('/api/v1/user/settings/operator', json=payload)
	assert r.status_code == 200

	# Read back
	r2 = client.get('/api/v1/user/settings/operator')
	assert r2.status_code == 200
	body = r2.json()
	assert body['email'] == 'op@example.com'


