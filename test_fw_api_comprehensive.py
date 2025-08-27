#!/usr/bin/env python3
"""
Comprehensive OHT-50 Firmware API Testing Tool
Test tất cả API endpoints và tổng hợp kết quả
"""

import asyncio
import aiohttp
import json
import time
import sys
from typing import Dict, List, Any
from dataclasses import dataclass
from datetime import datetime

@dataclass
class APITestResult:
    """Kết quả test API endpoint"""
    endpoint: str
    method: str
    status_code: int
    response_time_ms: float
    success: bool
    error_message: str = ""
    response_data: Dict[str, Any] = None

class OHT50APITester:
    """OHT-50 API Testing Tool"""
    
    def __init__(self, base_url: str = "http://localhost:8080"):
        self.base_url = base_url
        self.session = None
        self.results: List[APITestResult] = []
        
        # Định nghĩa tất cả API endpoints cần test
        self.api_endpoints = [
            # System APIs
            {"path": "/api/v1/system/status", "method": "GET", "name": "System Status"},
            {"path": "/api/v1/system/health", "method": "GET", "name": "System Health"},
            
            # Network APIs
            {"path": "/api/v1/network/status", "method": "GET", "name": "Network Status"},
            {"path": "/api/v1/network/config", "method": "GET", "name": "Network Config GET"},
            {"path": "/api/v1/network/config", "method": "POST", "name": "Network Config POST", 
             "data": {"ethernet_port": 8080, "websocket_port": 8081}},
            
            # Communication APIs
            {"path": "/api/v1/communication/status", "method": "GET", "name": "Communication Status"},
            
            # Safety APIs
            {"path": "/api/v1/safety/status", "method": "GET", "name": "Safety Status"},
            
            # Configuration APIs
            {"path": "/api/v1/config", "method": "GET", "name": "Config GET"},
            {"path": "/api/v1/config", "method": "POST", "name": "Config POST", 
             "data": {"test_config": "value", "timestamp": int(time.time())}},
            
            # Module APIs
            {"path": "/api/v1/modules", "method": "GET", "name": "Modules List"},
            {"path": "/api/v1/modules/1", "method": "GET", "name": "Module 1 Info"},
            {"path": "/api/v1/modules/2", "method": "GET", "name": "Module 2 Info"},
            {"path": "/api/v1/modules/3", "method": "GET", "name": "Module 3 Info"},
            {"path": "/api/v1/modules/1/command", "method": "POST", "name": "Module 1 Command", 
             "data": {"command": "status", "parameters": {}}},
            {"path": "/api/v1/modules/2/command", "method": "POST", "name": "Module 2 Command", 
             "data": {"command": "status", "parameters": {}}},
            {"path": "/api/v1/modules/3/command", "method": "POST", "name": "Module 3 Command", 
             "data": {"command": "status", "parameters": {}}},
            
            # Module-specific APIs
            {"path": "/api/v1/modules/power/status", "method": "GET", "name": "Power Module Status"},
            {"path": "/api/v1/modules/power/control", "method": "POST", "name": "Power Module Control", 
             "data": {"action": "status"}},
            {"path": "/api/v1/modules/safety/sensors", "method": "GET", "name": "Safety Sensors"},
            {"path": "/api/v1/modules/safety/relays", "method": "GET", "name": "Safety Relays"},
            {"path": "/api/v1/modules/motor/status", "method": "GET", "name": "Motor Status"},
            {"path": "/api/v1/modules/motor/control", "method": "POST", "name": "Motor Control", 
             "data": {"action": "status"}},
            {"path": "/api/v1/modules/dock/status", "method": "GET", "name": "Dock Status"},
            {"path": "/api/v1/modules/dock/control", "method": "POST", "name": "Dock Control", 
             "data": {"action": "status"}},
            
            # Diagnostics APIs
            {"path": "/api/v1/diagnostics", "method": "GET", "name": "Diagnostics"},
            
            # Safety Control APIs
            {"path": "/api/v1/safety/estop", "method": "POST", "name": "E-Stop Control", 
             "data": {"action": "status"}},
        ]
    
    async def __aenter__(self):
        """Async context manager entry"""
        self.session = aiohttp.ClientSession(
            timeout=aiohttp.ClientTimeout(total=10),
            headers={'Content-Type': 'application/json'}
        )
        return self
    
    async def __aexit__(self, exc_type, exc_val, exc_tb):
        """Async context manager exit"""
        if self.session:
            await self.session.close()
    
    async def test_endpoint(self, endpoint: Dict[str, Any]) -> APITestResult:
        """Test một API endpoint"""
        url = f"{self.base_url}{endpoint['path']}"
        method = endpoint['method']
        name = endpoint['name']
        
        start_time = time.time()
        
        try:
            if method == "GET":
                async with self.session.get(url) as response:
                    response_time = (time.time() - start_time) * 1000
                    response_data = await response.json() if response.status == 200 else None
                    
                    return APITestResult(
                        endpoint=f"{method} {endpoint['path']}",
                        method=method,
                        status_code=response.status,
                        response_time_ms=response_time,
                        success=response.status == 200,
                        error_message="" if response.status == 200 else f"HTTP {response.status}",
                        response_data=response_data
                    )
            
            elif method == "POST":
                data = endpoint.get('data', {})
                async with self.session.post(url, json=data) as response:
                    response_time = (time.time() - start_time) * 1000
                    response_data = await response.json() if response.status == 200 else None
                    
                    return APITestResult(
                        endpoint=f"{method} {endpoint['path']}",
                        method=method,
                        status_code=response.status,
                        response_time_ms=response_time,
                        success=response.status == 200,
                        error_message="" if response.status == 200 else f"HTTP {response.status}",
                        response_data=response_data
                    )
        
        except asyncio.TimeoutError:
            return APITestResult(
                endpoint=f"{method} {endpoint['path']}",
                method=method,
                status_code=0,
                response_time_ms=(time.time() - start_time) * 1000,
                success=False,
                error_message="Timeout"
            )
        
        except Exception as e:
            return APITestResult(
                endpoint=f"{method} {endpoint['path']}",
                method=method,
                status_code=0,
                response_time_ms=(time.time() - start_time) * 1000,
                success=False,
                error_message=str(e)
            )
    
    async def test_all_endpoints(self) -> List[APITestResult]:
        """Test tất cả API endpoints"""
        print(f"🔍 Testing OHT-50 Firmware API at {self.base_url}")
        print(f"📅 Test started at: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
        print(f"📊 Total endpoints to test: {len(self.api_endpoints)}")
        print("-" * 80)
        
        # Test tất cả endpoints
        tasks = [self.test_endpoint(endpoint) for endpoint in self.api_endpoints]
        results = await asyncio.gather(*tasks, return_exceptions=True)
        
        # Convert exceptions to error results
        self.results = []
        for i, result in enumerate(results):
            if isinstance(result, Exception):
                self.results.append(APITestResult(
                    endpoint=f"{self.api_endpoints[i]['method']} {self.api_endpoints[i]['path']}",
                    method=self.api_endpoints[i]['method'],
                    status_code=0,
                    response_time_ms=0,
                    success=False,
                    error_message=str(result)
                ))
            else:
                self.results.append(result)
        
        return self.results
    
    def generate_summary(self) -> Dict[str, Any]:
        """Tạo báo cáo tổng hợp"""
        total_tests = len(self.results)
        successful_tests = sum(1 for r in self.results if r.success)
        failed_tests = total_tests - successful_tests
        
        # Phân loại theo endpoint groups
        endpoint_groups = {
            "System": [],
            "Network": [],
            "Communication": [],
            "Safety": [],
            "Configuration": [],
            "Modules": [],
            "Diagnostics": []
        }
        
        for result in self.results:
            if "system" in result.endpoint.lower():
                endpoint_groups["System"].append(result)
            elif "network" in result.endpoint.lower():
                endpoint_groups["Network"].append(result)
            elif "communication" in result.endpoint.lower():
                endpoint_groups["Communication"].append(result)
            elif "safety" in result.endpoint.lower():
                endpoint_groups["Safety"].append(result)
            elif "config" in result.endpoint.lower():
                endpoint_groups["Configuration"].append(result)
            elif "diagnostics" in result.endpoint.lower():
                endpoint_groups["Diagnostics"].append(result)
            else:
                endpoint_groups["Modules"].append(result)
        
        # Tính thống kê response time
        response_times = [r.response_time_ms for r in self.results if r.success]
        avg_response_time = sum(response_times) / len(response_times) if response_times else 0
        max_response_time = max(response_times) if response_times else 0
        min_response_time = min(response_times) if response_times else 0
        
        return {
            "test_info": {
                "timestamp": datetime.now().isoformat(),
                "base_url": self.base_url,
                "total_endpoints": total_tests
            },
            "summary": {
                "total_tests": total_tests,
                "successful_tests": successful_tests,
                "failed_tests": failed_tests,
                "success_rate": (successful_tests / total_tests * 100) if total_tests > 0 else 0
            },
            "performance": {
                "avg_response_time_ms": round(avg_response_time, 2),
                "max_response_time_ms": round(max_response_time, 2),
                "min_response_time_ms": round(min_response_time, 2)
            },
            "endpoint_groups": endpoint_groups,
            "detailed_results": self.results
        }
    
    def print_summary(self, summary: Dict[str, Any]):
        """In báo cáo tổng hợp"""
        print("\n" + "="*80)
        print("📋 OHT-50 FIRMWARE API TEST SUMMARY")
        print("="*80)
        
        # Test Info
        print(f"🕒 Test Time: {summary['test_info']['timestamp']}")
        print(f"🌐 Base URL: {summary['test_info']['base_url']}")
        print(f"📊 Total Endpoints: {summary['test_info']['total_endpoints']}")
        
        # Overall Results
        print(f"\n📈 OVERALL RESULTS:")
        print(f"   ✅ Successful: {summary['summary']['successful_tests']}")
        print(f"   ❌ Failed: {summary['summary']['failed_tests']}")
        print(f"   📊 Success Rate: {summary['summary']['success_rate']:.1f}%")
        
        # Performance
        print(f"\n⚡ PERFORMANCE:")
        print(f"   🕐 Average Response Time: {summary['performance']['avg_response_time_ms']}ms")
        print(f"   🚀 Fastest Response: {summary['performance']['min_response_time_ms']}ms")
        print(f"   🐌 Slowest Response: {summary['performance']['max_response_time_ms']}ms")
        
        # Results by Group
        print(f"\n📂 RESULTS BY ENDPOINT GROUP:")
        for group_name, results in summary['endpoint_groups'].items():
            if results:
                successful = sum(1 for r in results if r.success)
                total = len(results)
                success_rate = (successful / total * 100) if total > 0 else 0
                status = "✅" if success_rate == 100 else "⚠️" if success_rate > 50 else "❌"
                print(f"   {status} {group_name}: {successful}/{total} ({success_rate:.1f}%)")
        
        # Failed Tests
        failed_tests = [r for r in self.results if not r.success]
        if failed_tests:
            print(f"\n❌ FAILED TESTS:")
            for result in failed_tests:
                print(f"   • {result.endpoint}: {result.error_message}")
        
        # Successful Tests
        successful_tests = [r for r in self.results if r.success]
        if successful_tests:
            print(f"\n✅ SUCCESSFUL TESTS ({len(successful_tests)}):")
            for result in successful_tests:
                print(f"   • {result.endpoint} ({result.response_time_ms:.1f}ms)")
    
    def save_report(self, summary: Dict[str, Any], filename: str = "fw_api_test_report.json"):
        """Lưu báo cáo vào file JSON"""
        # Convert APITestResult objects to dicts for JSON serialization
        report_data = summary.copy()
        report_data['detailed_results'] = []
        for result in summary['detailed_results']:
            report_data['detailed_results'].append({
                'endpoint': result.endpoint,
                'method': result.method,
                'status_code': result.status_code,
                'response_time_ms': result.response_time_ms,
                'success': result.success,
                'error_message': result.error_message,
                'response_data': result.response_data
            })
        
        with open(filename, 'w', encoding='utf-8') as f:
            json.dump(report_data, f, indent=2, ensure_ascii=False)
        print(f"\n💾 Report saved to: {filename}")

async def main():
    """Main function"""
    base_url = "http://localhost:8080"
    
    # Check command line arguments
    if len(sys.argv) > 1:
        base_url = sys.argv[1]
    
    print("🚀 OHT-50 Firmware API Comprehensive Testing Tool")
    print(f"🎯 Testing firmware at: {base_url}")
    
    async with OHT50APITester(base_url) as tester:
        # Test tất cả endpoints
        results = await tester.test_all_endpoints()
        
        # Tạo báo cáo
        summary = tester.generate_summary()
        
        # In kết quả
        tester.print_summary(summary)
        
        # Lưu báo cáo
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        filename = f"fw_api_test_report_{timestamp}.json"
        tester.save_report(summary, filename)
        
        # Return code
        success_rate = summary['summary']['success_rate']
        if success_rate >= 90:
            print(f"\n🎉 EXCELLENT! Success rate: {success_rate:.1f}%")
            return 0
        elif success_rate >= 70:
            print(f"\n⚠️  GOOD! Success rate: {success_rate:.1f}% - Some issues found")
            return 1
        else:
            print(f"\n❌ POOR! Success rate: {success_rate:.1f}% - Many issues found")
            return 2

if __name__ == "__main__":
    try:
        exit_code = asyncio.run(main())
        sys.exit(exit_code)
    except KeyboardInterrupt:
        print("\n⏹️  Test interrupted by user")
        sys.exit(1)
    except Exception as e:
        print(f"\n💥 Test failed with error: {e}")
        sys.exit(1)
