# OHT-50 Backend Makefile
# Các lệnh development và CI/CD

.PHONY: help install dev test lint format clean build docker-build docker-run docker-stop

# Default target
help:
	@echo "OHT-50 Backend - Available commands:"
	@echo ""
	@echo "Development:"
	@echo "  install     - Install dependencies"
	@echo "  dev         - Run development server"
	@echo "  test        - Run tests with coverage"
	@echo "  lint        - Run linting checks"
	@echo "  format      - Format code with black and isort"
	@echo "  clean       - Clean cache and temporary files"
	@echo ""
	@echo "Docker:"
	@echo "  docker-build - Build Docker image"
	@echo "  docker-run   - Run Docker container"
	@echo "  docker-stop  - Stop Docker container"
	@echo ""
	@echo "CI/CD:"
	@echo "  ci-lint     - Run CI linting checks"
	@echo "  ci-test     - Run CI tests"
	@echo "  ci-security - Run security scans"
	@echo "  ci-all      - Run all CI checks"

# Development commands
install:
	pip install -r requirements.txt
	pip install -e .[dev]

dev:
	uvicorn app.main:app --reload --host 127.0.0.1 --port 8000

test:
	pytest --cov=app --cov-report=html --cov-report=term-missing -v

test-fast:
	pytest --cov=app --cov-report=term-missing -v

lint:
	flake8 app tests
	black --check --diff app tests
	isort --check-only --diff app tests
	bandit -r app

format:
	black app tests
	isort app tests

clean:
	find . -type f -name "*.pyc" -delete
	find . -type d -name "__pycache__" -delete
	find . -type d -name "*.egg-info" -exec rm -rf {} +
	rm -rf .pytest_cache
	rm -rf htmlcov
	rm -rf .coverage
	rm -rf dist
	rm -rf build

# Docker commands
docker-build:
	docker build -t oht-50-backend:latest .

docker-run:
	docker run -d --name oht-50-backend -p 8000:8000 oht-50-backend:latest

docker-stop:
	docker stop oht-50-backend || true
	docker rm oht-50-backend || true

# CI/CD commands
ci-lint:
	flake8 app --count --select=E9,F63,F7,F82 --show-source --statistics
	flake8 app --count --exit-zero --max-complexity=10 --max-line-length=88 --statistics
	black --check --diff app
	isort --check-only --diff app

ci-test:
	pytest --cov=app --cov-report=xml --cov-report=html --cov-report=term-missing --junitxml=test-results.xml

ci-security:
	bandit -r app -f json -o bandit-report.json || true
	bandit -r app
	safety check --json --output safety-report.json || true
	safety check

ci-all: ci-lint ci-test ci-security

# Database commands (for future use)
db-migrate:
	@echo "Database migration commands will be added here"

db-seed:
	@echo "Database seeding commands will be added here"

# Documentation commands
docs-serve:
	@echo "Starting documentation server..."
	@echo "Visit http://127.0.0.1:8000/docs for API documentation"

# Monitoring commands
monitor-logs:
	@echo "Monitoring application logs..."
	@echo "Press Ctrl+C to stop"

# Deployment commands
deploy-dev:
	@echo "Deploying to development environment..."
	@echo "This will be implemented with actual deployment scripts"

deploy-prod:
	@echo "Deploying to production environment..."
	@echo "This will be implemented with actual deployment scripts"
