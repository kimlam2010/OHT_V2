# PowerShell script to run tests with proper environment variables
# This ensures tests run with TESTING=true for mock services

Write-Host "🚀 Starting Backend Tests with TESTING=true..." -ForegroundColor Green

# Set environment variables
$env:TESTING = "true"
$env:PYTHONPATH = "."

Write-Host "✅ Environment variables set:" -ForegroundColor Yellow
Write-Host "   TESTING=$env:TESTING" -ForegroundColor Cyan
Write-Host "   PYTHONPATH=$env:PYTHONPATH" -ForegroundColor Cyan

# Run all tests
Write-Host "`n🧪 Running all tests..." -ForegroundColor Green
python -m pytest tests/ -v --tb=short

# Check exit code
if ($LASTEXITCODE -eq 0) {
    Write-Host "`n✅ All tests passed!" -ForegroundColor Green
} else {
    Write-Host "`n❌ Some tests failed!" -ForegroundColor Red
}

# Clean up environment
Remove-Item Env:TESTING -ErrorAction SilentlyContinue
Remove-Item Env:PYTHONPATH -ErrorAction SilentlyContinue

Write-Host "`n🏁 Test run completed." -ForegroundColor Green
