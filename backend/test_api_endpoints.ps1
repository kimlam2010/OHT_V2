# OHT-50 Backend API Testing Script
# Tests all API endpoints systematically

Write-Host "🚀 OHT-50 Backend API Testing Script" -ForegroundColor Green
Write-Host "=====================================" -ForegroundColor Green

# Base URL
$baseUrl = "http://127.0.0.1:8000"

# Test results storage
$testResults = @()

# Function to test endpoint
function Test-Endpoint {
    param(
        [string]$Method,
        [string]$Url,
        [hashtable]$Headers = @{},
        [string]$Body = $null,
        [string]$ContentType = "application/json",
        [string]$TestName
    )
    
    try {
        $startTime = Get-Date
        $response = Invoke-WebRequest -Uri $Url -Method $Method -Headers $Headers -Body $Body -ContentType $ContentType -ErrorAction Stop
        $endTime = Get-Date
        $responseTime = ($endTime - $startTime).TotalMilliseconds
        
        $result = @{
            TestName = $TestName
            Method = $Method
            Url = $Url
            StatusCode = $response.StatusCode
            ResponseTime = [math]::Round($responseTime, 2)
            Success = $true
            Error = $null
            Content = $response.Content.Substring(0, [Math]::Min(200, $response.Content.Length))
        }
        
        Write-Host "✅ $TestName - Status: $($response.StatusCode) - Time: ${responseTime}ms" -ForegroundColor Green
    }
    catch {
        $result = @{
            TestName = $TestName
            Method = $Method
            Url = $Url
            StatusCode = $_.Exception.Response.StatusCode.value__
            ResponseTime = 0
            Success = $false
            Error = $_.Exception.Message
            Content = $null
        }
        
        Write-Host "❌ $TestName - Status: $($_.Exception.Response.StatusCode.value__) - Error: $($_.Exception.Message)" -ForegroundColor Red
    }
    
    $script:testResults += $result
    return $result
}

# Phase 1: Basic Connectivity Tests
Write-Host "`n📡 Phase 1: Basic Connectivity Tests" -ForegroundColor Yellow
Test-Endpoint -Method "GET" -Url "$baseUrl/" -TestName "Root Endpoint"
Test-Endpoint -Method "GET" -Url "$baseUrl/health" -TestName "Health Check"
Test-Endpoint -Method "GET" -Url "$baseUrl/docs" -TestName "API Documentation"

# Phase 2: Authentication Tests
Write-Host "`n🔐 Phase 2: Authentication Tests" -ForegroundColor Yellow
$loginResponse = Test-Endpoint -Method "POST" -Url "$baseUrl/api/v1/auth/login" -Body '{"username":"admin","password":"admin123"}' -TestName "User Login"

if ($loginResponse.Success) {
    $token = ($loginResponse.Content | ConvertFrom-Json).access_token
    $authHeaders = @{"Authorization" = "Bearer $token"}
    Write-Host "✅ Authentication successful, token obtained" -ForegroundColor Green
} else {
    Write-Host "❌ Authentication failed, skipping authenticated tests" -ForegroundColor Red
    $authHeaders = @{}
}

# Phase 3: Robot Control API Tests
Write-Host "`n🤖 Phase 3: Robot Control API Tests" -ForegroundColor Yellow
Test-Endpoint -Method "GET" -Url "$baseUrl/api/v1/robot/status" -Headers $authHeaders -TestName "Robot Status"
Test-Endpoint -Method "GET" -Url "$baseUrl/api/v1/robot/position" -Headers $authHeaders -TestName "Robot Position"
Test-Endpoint -Method "POST" -Url "$baseUrl/api/v1/robot/move" -Headers $authHeaders -Body '{"x":100,"y":200}' -TestName "Robot Move"
Test-Endpoint -Method "POST" -Url "$baseUrl/api/v1/robot/stop" -Headers $authHeaders -TestName "Robot Stop"
Test-Endpoint -Method "POST" -Url "$baseUrl/api/v1/robot/emergency-stop" -Headers $authHeaders -TestName "Emergency Stop"

# Phase 4: Telemetry API Tests
Write-Host "`n📊 Phase 4: Telemetry API Tests" -ForegroundColor Yellow
Test-Endpoint -Method "GET" -Url "$baseUrl/api/v1/telemetry/current" -Headers $authHeaders -TestName "Current Telemetry"
Test-Endpoint -Method "GET" -Url "$baseUrl/api/v1/telemetry/lidar" -Headers $authHeaders -TestName "LiDAR Data"
Test-Endpoint -Method "GET" -Url "$baseUrl/api/v1/telemetry/sensors" -Headers $authHeaders -TestName "Sensor Data"
Test-Endpoint -Method "GET" -Url "$baseUrl/api/v1/telemetry/history" -Headers $authHeaders -TestName "Telemetry History"

# Phase 5: Safety API Tests
Write-Host "`n🚨 Phase 5: Safety API Tests" -ForegroundColor Yellow
Test-Endpoint -Method "GET" -Url "$baseUrl/api/v1/safety/status" -Headers $authHeaders -TestName "Safety Status"
Test-Endpoint -Method "GET" -Url "$baseUrl/api/v1/safety/alerts" -Headers $authHeaders -TestName "Safety Alerts"
Test-Endpoint -Method "POST" -Url "$baseUrl/api/v1/safety/emergency-stop" -Headers $authHeaders -TestName "Safety Emergency Stop"

# Phase 6: Speed Control API Tests
Write-Host "`n⚡ Phase 6: Speed Control API Tests" -ForegroundColor Yellow
Test-Endpoint -Method "GET" -Url "$baseUrl/api/v1/speed-control/status" -Headers $authHeaders -TestName "Speed Control Status"
Test-Endpoint -Method "POST" -Url "$baseUrl/api/v1/speed-control/set-speed" -Headers $authHeaders -Body '{"target_speed":1.5,"mode":"manual"}' -TestName "Set Speed"
Test-Endpoint -Method "GET" -Url "$baseUrl/api/v1/speed-control/performance" -Headers $authHeaders -TestName "Speed Performance"

# Phase 7: Configuration API Tests
Write-Host "`n⚙️ Phase 7: Configuration API Tests" -ForegroundColor Yellow
Test-Endpoint -Method "GET" -Url "$baseUrl/api/v1/config/system" -Headers $authHeaders -TestName "System Config"
Test-Endpoint -Method "GET" -Url "$baseUrl/api/v1/config/robot" -Headers $authHeaders -TestName "Robot Config"
Test-Endpoint -Method "POST" -Url "$baseUrl/api/v1/config/update" -Headers $authHeaders -Body '{"key":"test","value":"test"}' -TestName "Update Config"

# Phase 8: Monitoring API Tests
Write-Host "`n📈 Phase 8: Monitoring API Tests" -ForegroundColor Yellow
Test-Endpoint -Method "GET" -Url "$baseUrl/api/v1/monitoring/health" -Headers $authHeaders -TestName "Monitoring Health"
Test-Endpoint -Method "GET" -Url "$baseUrl/api/v1/monitoring/metrics" -Headers $authHeaders -TestName "System Metrics"
Test-Endpoint -Method "GET" -Url "$baseUrl/api/v1/monitoring/logs" -Headers $authHeaders -TestName "System Logs"

# Generate Summary Report
Write-Host "`n📋 API Testing Summary Report" -ForegroundColor Cyan
Write-Host "=============================" -ForegroundColor Cyan

$totalTests = $testResults.Count
$successfulTests = ($testResults | Where-Object { $_.Success -eq $true }).Count
$failedTests = $totalTests - $successfulTests
$successRate = [math]::Round(($successfulTests / $totalTests) * 100, 2)

Write-Host "Total Tests: $totalTests" -ForegroundColor White
Write-Host "Successful: $successfulTests" -ForegroundColor Green
Write-Host "Failed: $failedTests" -ForegroundColor Red
Write-Host "Success Rate: $successRate%" -ForegroundColor $(if ($successRate -ge 90) { "Green" } elseif ($successRate -ge 70) { "Yellow" } else { "Red" })

# Performance Analysis
$avgResponseTime = [math]::Round(($testResults | Where-Object { $_.Success -eq $true } | Measure-Object -Property ResponseTime -Average).Average, 2)
$maxResponseTime = ($testResults | Where-Object { $_.Success -eq $true } | Measure-Object -Property ResponseTime -Maximum).Maximum
$minResponseTime = ($testResults | Where-Object { $_.Success -eq $true } | Measure-Object -Property ResponseTime -Minimum).Minimum

Write-Host "`n⚡ Performance Analysis" -ForegroundColor Cyan
Write-Host "Average Response Time: ${avgResponseTime}ms" -ForegroundColor White
Write-Host "Max Response Time: ${maxResponseTime}ms" -ForegroundColor White
Write-Host "Min Response Time: ${minResponseTime}ms" -ForegroundColor White

# Failed Tests Details
if ($failedTests -gt 0) {
    Write-Host "`n❌ Failed Tests Details" -ForegroundColor Red
    $testResults | Where-Object { $_.Success -eq $false } | ForEach-Object {
        Write-Host "  - $($_.TestName): $($_.Error)" -ForegroundColor Red
    }
}

# Performance Issues
$slowTests = $testResults | Where-Object { $_.Success -eq $true -and $_.ResponseTime -gt 100 }
if ($slowTests.Count -gt 0) {
    Write-Host "`n🐌 Slow Tests (>100ms)" -ForegroundColor Yellow
    $slowTests | ForEach-Object {
        Write-Host "  - $($_.TestName): $($_.ResponseTime)ms" -ForegroundColor Yellow
    }
}

Write-Host "`nAPI Testing Complete!" -ForegroundColor Green
