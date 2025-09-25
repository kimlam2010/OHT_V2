#!/bin/bash

# 🔧 Script cài đặt libwebsockets cho OHT-50 WebSocket System Fix
# Chạy với: sudo ./install_libwebsockets.sh

echo "🚀 Cài đặt libwebsockets cho OHT-50 WebSocket System Fix"
echo "=================================================="

# Update package list
echo "📦 Updating package list..."
apt update

# Install libwebsockets development package
echo "📦 Installing libwebsockets-dev..."
apt install -y libwebsockets-dev

# Install libwebsockets runtime package
echo "📦 Installing libwebsockets16..."
apt install -y libwebsockets16

# Verify installation
echo "✅ Verifying installation..."
pkg-config --modversion libwebsockets

# Check library files
echo "📁 Checking library files..."
ls -la /usr/lib/*/libwebsockets*

echo "🎉 libwebsockets installation completed!"
echo "📋 Next steps:"
echo "   1. Update CMakeLists.txt to link libwebsockets"
echo "   2. Rewrite WebSocket server with libwebsockets"
echo "   3. Fix thread management issues"
echo "   4. Test WebSocket system"
