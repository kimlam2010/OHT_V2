#!/usr/bin/env python3
"""
Test script for admin privileges
"""

import requests
import json

def test_admin_privileges():
    print('🔐 TESTING WITH ADMIN PRIVILEGES')
    print('=' * 60)

    base_url = 'http://localhost:8000'

    # 1. Test login với admin account
    print('\n1. Admin Login:')
    login_data = {
        'username': 'admin',
        'password': 'admin123'
    }

    try:
        response = requests.post(f'{base_url}/api/v1/auth/login', json=login_data)
        print(f'Login Response: {response.status_code}')
        
        if response.status_code == 200:
            token_data = response.json()
            token = token_data.get('access_token')
            user_info = token_data.get('user', {})
            print(f'✅ Login successful!')
            print(f'   User: {user_info.get("username", "Unknown")}')
            print(f'   Role: {user_info.get("role", "Unknown")}')
            print(f'   Token: {token[:30]}...')
            
            # 2. Test tất cả protected endpoints với admin token
            print('\n2. Testing ALL Protected Endpoints with Admin Token:')
            headers = {'Authorization': f'Bearer {token}'}
            
            # Test endpoints theo từng API
            test_cases = [
                # Robot API
                ('GET', '/api/v1/robot/status', 'Robot Status'),
                ('GET', '/api/v1/robot/position', 'Robot Position'),
                ('GET', '/api/v1/robot/configuration', 'Robot Configuration'),
                
                # Telemetry API
                ('GET', '/api/v1/telemetry/current', 'Current Telemetry'),
                ('GET', '/api/v1/telemetry/history', 'Telemetry History'),
                
                # Map API
                ('GET', '/api/v1/map/status', 'Map Status'),
                ('GET', '/api/v1/map/current', 'Current Map'),
                ('GET', '/api/v1/map/list', 'Map List'),
                
                # Sensors API
                ('GET', '/api/v1/sensors/status', 'Sensors Status'),
                ('GET', '/api/v1/sensors/list', 'Sensors List'),
                
                # Localization API
                ('GET', '/api/v1/localization/position', 'Localization Position'),
                ('GET', '/api/v1/localization/history', 'Position History'),
                
                # Safety API
                ('GET', '/api/v1/safety/status', 'Safety Status'),
                
                # Monitoring API
                ('GET', '/api/v1/monitoring/metrics', 'Monitoring Metrics'),
                ('GET', '/api/v1/monitoring/status', 'Monitoring Status'),
                
                # Config API
                ('GET', '/api/v1/config/current', 'Current Config'),
                ('GET', '/api/v1/config/system', 'System Config'),
                
                # Speed Control API
                ('GET', '/api/v1/speed-control/current', 'Current Speed'),
                ('GET', '/api/v1/speed-control/limits', 'Speed Limits'),
                
                # Auth API
                ('GET', '/api/v1/auth/me', 'Current User Info'),
            ]
            
            success_count = 0
            forbidden_count = 0
            unauthorized_count = 0
            error_count = 0
            total_count = len(test_cases)
            
            for method, endpoint, description in test_cases:
                try:
                    if method == 'GET':
                        resp = requests.get(f'{base_url}{endpoint}', headers=headers, timeout=5)
                    elif method == 'POST':
                        resp = requests.post(f'{base_url}{endpoint}', headers=headers, timeout=5)
                    
                    if resp.status_code == 200:
                        status = '✅'
                        success_count += 1
                    elif resp.status_code == 403:
                        status = '🔒'
                        forbidden_count += 1
                    elif resp.status_code == 401:
                        status = '⚠️'
                        unauthorized_count += 1
                    elif resp.status_code == 404:
                        status = '❌'
                        error_count += 1
                    else:
                        status = '⚠️'
                        error_count += 1
                    
                    print(f'{status} {method} {endpoint:<35} | {resp.status_code} | {description}')
                    
                    # Show response data for successful requests
                    if resp.status_code == 200 and endpoint in ['/api/v1/robot/status', '/api/v1/telemetry/current']:
                        try:
                            data = resp.json()
                            if 'data' in data:
                                print(f'    📊 Data: {str(data["data"])[:50]}...')
                        except:
                            pass
                            
                except Exception as e:
                    print(f'❌ {method} {endpoint:<35} | ERROR | {description} - {str(e)[:30]}')
                    error_count += 1
            
            print(f'\n📊 SUMMARY:')
            print(f'   ✅ Successful (200): {success_count}/{total_count}')
            print(f'   🔒 Forbidden (403): {forbidden_count}')
            print(f'   ⚠️  Unauthorized (401): {unauthorized_count}')
            print(f'   ❌ Errors (404/500): {error_count}')
            
            # 3. Test POST endpoints
            print('\n3. Testing POST Endpoints:')
            post_endpoints = [
                ('/api/v1/robot/command', {'command_type': 'move', 'parameters': {'direction': 'forward'}}),
                ('/api/v1/safety/emergency', {}),
            ]
            
            for endpoint, data in post_endpoints:
                try:
                    resp = requests.post(f'{base_url}{endpoint}', headers=headers, json=data, timeout=5)
                    status = '✅' if resp.status_code in [200, 201] else '⚠️'
                    print(f'{status} POST {endpoint:<35} | {resp.status_code}')
                except Exception as e:
                    print(f'❌ POST {endpoint:<35} | ERROR - {str(e)[:30]}')
                    
        else:
            print(f'❌ Login failed: {response.status_code}')
            print(f'Response: {response.text[:200]}')
            
    except Exception as e:
        print(f'❌ Login error: {str(e)[:100]}')

    print('\n' + '=' * 60)
    print('✅ ADMIN PRIVILEGE TESTING COMPLETED')

if __name__ == '__main__':
    test_admin_privileges()
