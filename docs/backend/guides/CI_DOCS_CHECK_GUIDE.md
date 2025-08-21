# CI Docs-Check Guide - OHT-50 Backend

## Mục tiêu
Tự động hóa việc kiểm tra tính nhất quán giữa code, OpenAPI docs và tài liệu.

## CI Pipeline Components

### 1. Link Checker
```yaml
# .github/workflows/docs-check.yml
name: Documentation Check
on: [push, pull_request]
jobs:
  link-check:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      - name: Check Markdown Links
        run: |
          pip install markdown-link-check
          find docs/ -name "*.md" -exec markdown-link-check {} \;
```

### 2. OpenAPI Diff Check
```yaml
  openapi-diff:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      - name: Generate OpenAPI Schema
        run: |
          cd backend
          python -c "
          from app.main import app
          import json
          schema = app.openapi()
          with open('openapi-current.json', 'w') as f:
              json.dump(schema, f, indent=2)
          "
      - name: Compare with Baseline
        run: |
          if [ -f openapi-baseline.json ]; then
            diff openapi-baseline.json openapi-current.json || {
              echo "OpenAPI schema changed. Update baseline or docs."
              exit 1
            }
          fi
```

### 3. Markdown Lint
```yaml
  markdown-lint:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      - name: Lint Markdown
        run: |
          pip install markdownlint-cli
          markdownlint docs/**/*.md
```

### 4. Documentation Coverage
```yaml
  doc-coverage:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      - name: Check API Documentation
        run: |
          python scripts/check_api_docs.py
```

## Local Development Tools

### 1. Pre-commit Hooks
```yaml
# .pre-commit-config.yaml
repos:
  - repo: https://github.com/igorshubovych/markdownlint-cli
    rev: v0.33.0
    hooks:
      - id: markdownlint
        args: [--fix]
  - repo: local
    hooks:
      - id: openapi-check
        name: OpenAPI Schema Check
        entry: python scripts/check_openapi.py
        language: python
        files: ^backend/app/
```

### 2. Documentation Scripts
```python
# scripts/check_api_docs.py
import os
import re
from pathlib import Path

def check_api_endpoints():
    """Check if all API endpoints are documented"""
    api_files = Path("backend/app/api").rglob("*.py")
    documented_endpoints = set()
    
    # Extract documented endpoints from docs
    docs_files = Path("docs/backend").rglob("*.md")
    for doc_file in docs_files:
        with open(doc_file) as f:
            content = f.read()
            endpoints = re.findall(r'`([A-Z]+ /[^`]+)`', content)
            documented_endpoints.update(endpoints)
    
    # Extract actual endpoints from code
    actual_endpoints = set()
    for api_file in api_files:
        with open(api_file) as f:
            content = f.read()
            routes = re.findall(r'@.*\.(get|post|put|delete)\(["\']([^"\']+)["\']', content)
            actual_endpoints.update([f"{method.upper()} {route}" for method, route in routes])
    
    missing = actual_endpoints - documented_endpoints
    if missing:
        print(f"Missing documentation for: {missing}")
        return False
    return True

if __name__ == "__main__":
    if not check_api_endpoints():
        exit(1)
```

## Validation Rules

### 1. Link Validation
- Tất cả internal links phải hợp lệ
- External links phải accessible
- Cross-references giữa docs phải chính xác

### 2. OpenAPI Validation
- Schema phải valid JSON
- Examples phải match model structure
- Response codes phải documented
- Error responses phải defined

### 3. Content Validation
- Markdown syntax phải valid
- Headers phải có proper hierarchy
- Code blocks phải có language specifiers
- Tables phải có proper formatting

## Failure Handling

### 1. Breaking Changes
- OpenAPI schema changes require docs update
- New endpoints require documentation
- Model changes require example updates

### 2. Documentation Drift
- Automated alerts for documentation gaps
- Weekly reports on documentation health
- Integration with PR review process

## Monitoring & Reporting

### 1. Metrics
- Documentation coverage percentage
- Link health status
- OpenAPI schema consistency
- Documentation update frequency

### 2. Alerts
- Failed link checks
- Missing API documentation
- Schema validation errors
- Documentation staleness

## Integration with Development Workflow

### 1. PR Requirements
- Documentation updates for API changes
- Link validation passing
- OpenAPI schema consistency
- Example completeness

### 2. Release Process
- Documentation review checklist
- API documentation finalization
- Example validation
- Link health verification
