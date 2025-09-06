# PowerShell script to run backend in production mode
# This ensures production environment without TESTING=true

Write-Host "🚀 Starting Backend in Production Mode..." -ForegroundColor Green

# Set production environment variables
$env:TESTING = "false"
$env:PYTHONPATH = "."
$env:ENVIRONMENT = "production"

Write-Host "✅ Production environment variables set:" -ForegroundColor Yellow
Write-Host "   TESTING=$env:TESTING" -ForegroundColor Cyan
Write-Host "   ENVIRONMENT=$env:ENVIRONMENT" -ForegroundColor Cyan
Write-Host "   PYTHONPATH=$env:PYTHONPATH" -ForegroundColor Cyan

Write-Host "`n⚠️  WARNING: Production mode requires:" -ForegroundColor Yellow
Write-Host "   - Real Firmware HTTP API running" -ForegroundColor Red
Write-Host "   - Database properly configured" -ForegroundColor Red
Write-Host "   - Authentication system active" -ForegroundColor Red

# Start the server
Write-Host "`n🌐 Starting FastAPI server..." -ForegroundColor Green
python -m uvicorn app.main:app --host 127.0.0.1 --port 8000 --reload

# Clean up environment
Remove-Item Env:TESTING -ErrorAction SilentlyContinue
Remove-Item Env:PYTHONPATH -ErrorAction SilentlyContinue
Remove-Item Env:ENVIRONMENT -ErrorAction SilentlyContinue

Write-Host "`n🏁 Production server stopped." -ForegroundColor Green
