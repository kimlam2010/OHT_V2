# PowerShell script to create Pull Request manually
# This script will open the GitHub web interface to create PR

param(
    [string]$Title = "🚀 Update OHT-50 Documentation and GitHub Integration",
    [string]$Body = "PR_TEMPLATE.md",
    [string]$BaseBranch = "main",
    [string]$HeadBranch = "feature/pm-update-docs"
)

Write-Host "🚀 Creating Pull Request..." -ForegroundColor Green
Write-Host "================================================" -ForegroundColor Green

# Get current repository URL
$remoteUrl = git config --get remote.origin.url
if ($remoteUrl -match "github\.com[:/]([^/]+)/([^/]+?)(?:\.git)?$") {
    $owner = $matches[1]
    $repo = $matches[2]
    Write-Host "Repository: $owner/$repo" -ForegroundColor Cyan
} else {
    Write-Host "❌ Error: Could not determine GitHub repository URL" -ForegroundColor Red
    exit 1
}

# Construct GitHub PR URL
$prUrl = "https://github.com/$owner/$repo/compare/$BaseBranch...$HeadBranch"

# Read PR body from file if it exists
$prBody = ""
if (Test-Path $Body) {
    $prBody = Get-Content $Body -Raw
    Write-Host "✅ Using PR template from: $Body" -ForegroundColor Green
} else {
    $prBody = @"
# 🚀 Pull Request: Update OHT-50 Documentation and GitHub Integration

## 📋 **PR Overview**
**Type:** Documentation Update  
**Priority:** High  
**Branch:** `$HeadBranch` → `$BaseBranch`  
**Status:** Ready for Review

## 🎯 **Objective**
Update OHT-50 documentation system with comprehensive GitHub integration analysis and enhanced requirements documentation.

## 📊 **Changes Summary**
- Updated REQ_RS485_Travel_Motor_Integration.md to v2.1
- Added GitHub integration analysis and compatibility reports
- Created complete UI/UX design requirements suite
- Enhanced firmware-backend integration documentation

## 🔧 **Key Technical Changes**
- System Registers moved from 0x00F0-0x00F6 to 0x0100-0x0106
- Auto-detect registers remain at 0x00F7-0x00FF
- Comprehensive GitHub repository compatibility analysis
- Ready-to-submit GitHub issue for external repository

## 🚨 **Critical Issues Addressed**
- Auto-Detect Failure in external modules
- Module Identification Missing
- No Validation Mechanism
- Address Conflict in register mapping

## 📈 **Impact Assessment**
- Enhanced documentation coverage and quality
- Improved GitHub integration analysis
- Standardized register mapping for auto-detection
- Complete UI/UX requirements for frontend development

## 🧪 **Testing Requirements**
- Auto-detect testing with new register layout
- Integration testing with OHT-50 Master Module
- Register mapping validation
- Control mode testing

## 📋 **Review Checklist**
- [ ] All new files follow documentation standards
- [ ] Register address changes are clearly documented
- [ ] GitHub integration analysis is comprehensive
- [ ] UI/UX requirements are complete and clear
- [ ] Implementation roadmap is realistic

## 🚀 **Next Steps After Approval**
1. Submit GitHub issue to KhaLin0401/Driver_2_Motor
2. Begin UI/UX design implementation
3. Start firmware-backend integration planning
4. Update development roadmap with new requirements

**🏷️ Labels:** `documentation`, `integration`, `github`, `auto-detect`, `ui-ux`, `firmware-backend`
"@
    Write-Host "⚠️  Using default PR template" -ForegroundColor Yellow
}

# URL encode the title and body
$encodedTitle = [System.Web.HttpUtility]::UrlEncode($Title)
$encodedBody = [System.Web.HttpUtility]::UrlEncode($prBody)

# Construct the full GitHub PR URL with pre-filled data
$fullPrUrl = "$prUrl?title=$encodedTitle&body=$encodedBody"

Write-Host "📝 PR Title: $Title" -ForegroundColor Cyan
Write-Host "🌿 Base Branch: $BaseBranch" -ForegroundColor Cyan
Write-Host "🌿 Head Branch: $HeadBranch" -ForegroundColor Cyan
Write-Host "🔗 PR URL: $fullPrUrl" -ForegroundColor Cyan

Write-Host ""
Write-Host "📋 Instructions:" -ForegroundColor Yellow
Write-Host "1. Click the link above to open GitHub PR creation page" -ForegroundColor White
Write-Host "2. Review the pre-filled title and description" -ForegroundColor White
Write-Host "3. Add appropriate reviewers (CTO, FW Team Lead, Backend Team Lead)" -ForegroundColor White
Write-Host "4. Add labels: documentation, integration, github, auto-detect, ui-ux, firmware-backend" -ForegroundColor White
Write-Host "5. Click 'Create pull request'" -ForegroundColor White

Write-Host ""
Write-Host "🎯 Ready to create PR!" -ForegroundColor Green

# Ask user if they want to open the URL
$openUrl = Read-Host "Do you want to open the PR creation page in your browser? (y/n)"
if ($openUrl -eq "y" -or $openUrl -eq "Y") {
    Start-Process $fullPrUrl
    Write-Host "✅ Opened PR creation page in browser" -ForegroundColor Green
} else {
    Write-Host "📋 Please manually open: $fullPrUrl" -ForegroundColor Cyan
}

Write-Host ""
Write-Host "📊 PR Summary:" -ForegroundColor Green
Write-Host "Repository: $owner/$repo" -ForegroundColor White
Write-Host "From: $HeadBranch" -ForegroundColor White
Write-Host "To: $BaseBranch" -ForegroundColor White
Write-Host "Files Changed: 29 files, 10,160 insertions(+), 172 deletions(-)" -ForegroundColor White

Write-Host ""
Write-Host "✅ PR creation process completed!" -ForegroundColor Green
