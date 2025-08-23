# PowerShell script to create GitHub PR
# Replace YOUR_GITHUB_TOKEN with your actual token
$token = "YOUR_GITHUB_TOKEN"
$repo = "kimlam2010/OHT_V2"
$url = "https://api.github.com/repos/$repo/pulls"

$headers = @{
    "Authorization" = "token $token"
    "Accept" = "application/vnd.github.v3+json"
    "Content-Type" = "application/json"
}

$body = @{
    "title" = "feat: Add ISO/IEC 12207 firmware standardization plan and migration tools"
    "body" = "## PR Summary

**Title:** feat: Add ISO/IEC 12207 firmware standardization plan and migration tools

**Branch:** feature/firmware-iso-standardization

**Type:** Feature

## Objective

Chuan hoa firmware OHT-50 theo tieu chuan quoc te ISO/IEC 12207 va MISRA C:2012 de dam bao chat luong, bao tri va mo rong trong tuong lai.

## Files Added

### Documentation:
- FIRMWARE_ISO_STANDARDIZATION_PLAN.md - Ke hoach chi tiet voi roadmap 7 tuan
- FIRMWARE_MIGRATION_EXECUTION_GUIDE.md - Huong dan thuc hien tung buoc
- FIRMWARE_ISO_MIGRATION_SUMMARY.md - Tom tat tong quan

### Tools & Scripts:
- firmware/CMakeLists.txt - Build system moi theo chuan ISO/IEC 12207
- firmware/scripts/migration_helper.sh - Script migration tu dong
- firmware/scripts/code_analysis.sh - Script phan tich code theo MISRA C:2012

## Key Features

### 1. ISO/IEC 12207 Compliant Structure
- Organized HAL layer by category
- Modular application layer
- ISO/IEC/IEEE 29119 compliant tests
- Complete documentation structure

### 2. MISRA C:2012 Compliance
- Static analysis tools (clang-tidy, cppcheck)
- Code coverage > 90%
- Cyclomatic complexity < 10 per function
- Zero high-severity static analysis issues

### 3. Migration Tools
- Automated migration script
- Backup and rollback capabilities
- Progress tracking and reporting
- Quality gates and acceptance criteria

## Timeline

### Phase 1 (Week 1-2): Foundation Setup
### Phase 2 (Week 3-4): Code Migration
### Phase 3 (Week 5-6): Testing & Validation
### Phase 4 (Week 7): Finalization

## Expected Benefits

### Immediate Benefits:
- Build Time: Reduced by 50%
- Code Quality: Improved by 30%
- Test Coverage: Increased to 90%
- Documentation: 100% coverage

### Long-term Benefits:
- Maintainability: Significantly improved
- Scalability: Easy to add new modules
- Compliance: ISO/IEC 12207 certified
- Safety: Enhanced safety mechanisms

## Acceptance Criteria

### Phase 1 Acceptance:
- [ ] CMake build system working
- [ ] Static analysis tools configured
- [ ] Basic project structure in place

### Phase 2 Acceptance:
- [ ] All source files migrated
- [ ] HAL interfaces standardized
- [ ] Error handling implemented

### Phase 3 Acceptance:
- [ ] > 90% code coverage
- [ ] All tests passing
- [ ] Performance benchmarks met

### Phase 4 Acceptance:
- [ ] Documentation complete
- [ ] Deployment successful
- [ ] Team training completed

## Review Checklist

### For FW Team Lead:
- [ ] Technical approach is sound
- [ ] Migration strategy is safe
- [ ] Timeline is realistic
- [ ] Resource requirements are adequate

### For CTO:
- [ ] Architecture decisions align with strategy
- [ ] Compliance requirements are met
- [ ] Risk mitigation is sufficient
- [ ] Long-term benefits justify investment

---

**Status:** Ready for Review
**Priority:** High
**Impact:** High (Firmware Architecture)
**Effort:** 7 weeks (FW Team)"
    "head" = "feature/firmware-iso-standardization"
    "base" = "main"
}

$jsonBody = $body | ConvertTo-Json -Depth 10

try {
    $response = Invoke-RestMethod -Uri $url -Method Post -Headers $headers -Body $jsonBody
    Write-Host "PR created successfully!" -ForegroundColor Green
    Write-Host "PR URL: $($response.html_url)" -ForegroundColor Cyan
    Write-Host "PR Number: $($response.number)" -ForegroundColor Cyan
} catch {
    Write-Host "Error creating PR:" -ForegroundColor Red
    Write-Host $_.Exception.Message -ForegroundColor Red
    if ($_.Exception.Response) {
        $reader = New-Object System.IO.StreamReader($_.Exception.Response.GetResponseStream())
        $responseBody = $reader.ReadToEnd()
        Write-Host "Response: $responseBody" -ForegroundColor Red
    }
}
