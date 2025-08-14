from fastapi.testclient import TestClient
from unittest.mock import patch, MagicMock

from app.models.user import UserSettings


def test_get_user_settings_not_found(client: TestClient):
	with patch('app.api.v1.user.service') as mock_service:
		mock_service.get_user_settings.return_value = None
		r = client.get('/api/v1/user/settings/alice')
		assert r.status_code == 404
		assert 'not found' in r.json()['message'].lower()


def test_get_user_settings_ok(client: TestClient):
	with patch('app.api.v1.user.service') as mock_service:
		mock_service.get_user_settings.return_value = UserSettings(
			username='alice', display_name='Alice', email='alice@example.com', theme='dark'
		)
		r = client.get('/api/v1/user/settings/alice')
		assert r.status_code == 200
		body = r.json()
		assert body['username'] == 'alice'
		assert body['theme'] == 'dark'


def test_put_user_settings_username_mismatch(client: TestClient):
	payload = {
		"username": "bob", "display_name": "Bob", "email": "bob@example.com", "theme": "light"
	}
	r = client.put('/api/v1/user/settings/alice', json=payload)
	assert r.status_code == 400
	assert 'mismatch' in r.json()['message'].lower()


def test_put_user_settings_ok(client: TestClient):
	with patch('app.api.v1.user.service') as mock_service:
		mock_service.update_user_settings.return_value = UserSettings(
			username='alice', display_name='Alice', email='alice@example.com', theme='light'
		)
		payload = {
			"username": "alice", "display_name": "Alice", "email": "alice@example.com", "theme": "light"
		}
		r = client.put('/api/v1/user/settings/alice', json=payload)
		assert r.status_code == 200
		assert r.json()['username'] == 'alice'


