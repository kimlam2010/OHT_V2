#!/usr/bin/env python3
"""
Generate a secure JWT secret for OHT-50 Backend
"""

import secrets
import sys
import os

def generate_jwt_secret():
    """Generate a secure JWT secret"""
    # Generate 32 bytes (256 bits) of random data
    secret = secrets.token_hex(32)
    return secret

def main():
    """Main function"""
    print("🔐 OHT-50 Backend JWT Secret Generator")
    print("=" * 50)
    
    # Generate secret
    secret = generate_jwt_secret()
    
    print(f"Generated JWT Secret: {secret}")
    print()
    print("📝 Add this to your .env file:")
    print(f"JWT_SECRET={secret}")
    print()
    print("⚠️  IMPORTANT SECURITY NOTES:")
    print("1. Keep this secret secure and never commit it to version control")
    print("2. Use different secrets for different environments (dev/staging/prod)")
    print("3. Rotate secrets regularly in production")
    print("4. Store secrets in secure environment variable management systems")
    print()
    print("✅ JWT secret generated successfully!")

if __name__ == "__main__":
    main()
