#!/usr/bin/env python3
"""
Script để fix lỗi database và khởi động backend
"""

import os
import sys
import subprocess
import asyncio
from pathlib import Path

def fix_env_file():
    """Tạo file .env với cấu hình đúng"""
    print("Fixing .env file...")
    
    # Xóa file .env cũ
    env_file = Path(".env")
    if env_file.exists():
        env_file.unlink()
    
    # Tạo file .env mới
    env_content = """DATABASE_URL=sqlite+aiosqlite:///./oht50.db
JWT_SECRET=your-secret-key-here
USE_MOCK_FIRMWARE=true
DEBUG=false
HOST=0.0.0.0
PORT=8000
"""
    
    with open(".env", "w", encoding="utf-8") as f:
        f.write(env_content)
    
    print(".env file created successfully")

def uninstall_psycopg2():
    """Uninstall psycopg2 packages"""
    print("Uninstalling psycopg2 packages...")
    
    try:
        subprocess.run([sys.executable, "-m", "pip", "uninstall", "psycopg2", "psycopg2-binary", "-y"], 
                      check=True, capture_output=True)
        print("psycopg2 packages uninstalled")
    except subprocess.CalledProcessError as e:
        print(f"Warning: {e}")

def test_database_connection():
    """Test database connection"""
    print("Testing database connection...")
    
    try:
        # Test import
        from app.core.database import engine
        print("Database engine imported successfully")
        
        # Test connection
        async def test_conn():
            async with engine.begin() as conn:
                from sqlalchemy import text
                await conn.run_sync(lambda sync_conn: sync_conn.execute(text("SELECT 1")))
            print("Database connection test successful")
        
        asyncio.run(test_conn())
        return True
        
    except Exception as e:
        print(f"Database connection failed: {e}")
        return False

def run_migration():
    """Run database migration"""
    print("Running database migration...")
    
    try:
        subprocess.run([sys.executable, "-m", "alembic", "upgrade", "head"], 
                      check=True, capture_output=True)
        print("Migration completed successfully")
        return True
    except subprocess.CalledProcessError as e:
        print(f"Migration failed: {e}")
        return False

def init_database():
    """Initialize database directly"""
    print("Initializing database directly...")
    
    try:
        from app.core.database import init_db
        asyncio.run(init_db())
        print("Database initialized successfully")
        return True
    except Exception as e:
        print(f"Database initialization failed: {e}")
        return False

def main():
    """Main function"""
    print("Starting OHT-50 Backend Database Fix...")
    
    # Step 1: Fix .env file
    fix_env_file()
    
    # Step 2: Uninstall psycopg2
    uninstall_psycopg2()
    
    # Step 3: Test database connection
    if test_database_connection():
        print("Database connection is working!")
    else:
        print("Database connection failed. Trying alternative approach...")
        
        # Step 4: Try migration
        if run_migration():
            print("Migration successful!")
        else:
            # Step 5: Try direct initialization
            if init_database():
                print("Database initialized directly!")
            else:
                print("All database initialization methods failed")
                return False
    
    print("Database fix completed successfully!")
    print("You can now run: python -m uvicorn app.main:app --host 0.0.0.0 --port 8000 --reload")
    return True

if __name__ == "__main__":
    success = main()
    sys.exit(0 if success else 1)
