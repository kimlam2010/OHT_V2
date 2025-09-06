#!/usr/bin/env python3
"""
Debug 500 errors
"""

import requests
import json

def debug_500_errors():
    print('🔧 DEBUGGING 500 ERRORS')
    print('=' * 40)

    base_url = 'http://localhost:8000'

    # Login first
    login_data = {'username': 'admin', 'password': 'admin123'}
    response = requests.post(f'{base_url}/api/v1/auth/login', json=login_data)
    token = response.json().get('access_token')
    headers = {'Authorization': f'Bearer {token}'}

    # Test problematic endpoints
    problematic_endpoints = [
        '/api/v1/robot/position',
        '/api/v1/robot/configuration',
        '/api/v1/speed-control/current',
        '/api/v1/speed-control/limits'
    ]

    for endpoint in problematic_endpoints:
        try:
            resp = requests.get(f'{base_url}{endpoint}', headers=headers, timeout=5)
            print(f'\n{endpoint}: {resp.status_code}')
            if resp.status_code == 500:
                try:
                    error_data = resp.json()
                    print(f'   Error: {error_data.get("detail", "Unknown error")}')
                except:
                    print(f'   Error: {resp.text[:100]}')
        except Exception as e:
            print(f'\n{endpoint}: ERROR - {str(e)[:50]}')

if __name__ == '__main__':
    debug_500_errors()
