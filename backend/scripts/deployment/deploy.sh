#!/bin/bash

# OHT-50 Backend Deployment Script

set -e

echo "🚀 Starting OHT-50 Backend deployment..."

# Configuration
APP_NAME="oht50-backend"
DOCKER_COMPOSE_FILE="docker/docker-compose.yml"
ENV_FILE=".env"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check if .env file exists
if [ ! -f "$ENV_FILE" ]; then
    print_warning "Environment file not found. Creating from template..."
    cp env.example .env
    print_warning "Please edit .env file with your configuration before continuing."
    exit 1
fi

# Check if Docker is running
if ! docker info > /dev/null 2>&1; then
    print_error "Docker is not running. Please start Docker and try again."
    exit 1
fi

# Stop existing containers
print_status "Stopping existing containers..."
docker-compose -f $DOCKER_COMPOSE_FILE down || true

# Build and start services
print_status "Building and starting services..."
docker-compose -f $DOCKER_COMPOSE_FILE up --build -d

# Wait for services to be ready
print_status "Waiting for services to be ready..."
sleep 10

# Check if services are running
print_status "Checking service status..."
if docker-compose -f $DOCKER_COMPOSE_FILE ps | grep -q "Up"; then
    print_status "Services are running successfully!"
else
    print_error "Some services failed to start. Check logs with: docker-compose -f $DOCKER_COMPOSE_FILE logs"
    exit 1
fi

# Health check
print_status "Performing health check..."
if curl -f http://localhost:8000/health > /dev/null 2>&1; then
    print_status "Health check passed! Backend is running at http://localhost:8000"
else
    print_warning "Health check failed. Service might still be starting up."
fi

# Show service information
print_status "Service Information:"
echo "  - Backend API: http://localhost:8000"
echo "  - API Documentation: http://localhost:8000/docs"
echo "  - Health Check: http://localhost:8000/health"
echo "  - Metrics: http://localhost:8000/metrics"
echo "  - Redis: localhost:6379"

print_status "Deployment completed successfully! 🎉"
