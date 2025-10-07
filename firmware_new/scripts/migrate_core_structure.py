#!/usr/bin/env python3
"""
Migration Script: Restructure src/app/core/
Option 1: Domain-Driven Structure
"""

import os
import shutil
from pathlib import Path

# Base paths
CORE_DIR = Path("src/app/core")
BACKUP_DIR = CORE_DIR / "_backup"

# Migration mapping
MIGRATIONS = {
    # State Management
    "state_management": [
        "system_state_machine.c",
        "system_state_machine.h",
        "system_controller.c",
        "system_controller.h"
    ],
    
    # Safety System
    "safety": [
        "safety_monitor.c",
        "safety_monitor.h",
        "safety_integration/critical_module_detector.c",
        "safety_integration/critical_module_detector.h",
        "safety_integration/graduated_response_system.c",
        "safety_integration/graduated_response_system.h",
        "safety_integration/safety_rs485_integration.c",
        "safety_integration/safety_rs485_integration.h"
    ],
    
    # Control System
    "control": [
        "control_loop.c",
        "control_loop.h",
        "estimator_1d.c",
        "estimator_1d.h"
    ],
    
    # Backup files
    "_backup": [
        "safety_monitor.c.phase2.2.backup.20250919_161056",
        "safety_monitor.c.pre-phase2.20250919_160344"
    ]
}

def create_folders():
    """Create new folder structure"""
    print("📁 Creating folder structure...")
    for folder in MIGRATIONS.keys():
        folder_path = CORE_DIR / folder
        folder_path.mkdir(parents=True, exist_ok=True)
        print(f"  ✅ Created: {folder_path}")

def move_files():
    """Move files to new structure"""
    print("\n📦 Moving files...")
    
    for dest_folder, files in MIGRATIONS.items():
        dest_path = CORE_DIR / dest_folder
        
        for file in files:
            src_file = CORE_DIR / file
            
            # Get just the filename (remove safety_integration/ prefix if exists)
            filename = Path(file).name
            dest_file = dest_path / filename
            
            if src_file.exists():
                print(f"  📄 {src_file} → {dest_file}")
                shutil.move(str(src_file), str(dest_file))
            else:
                print(f"  ⚠️  Not found: {src_file}")

def cleanup_old_folders():
    """Clean up old safety_integration folder if empty"""
    print("\n🧹 Cleaning up old folders...")
    old_safety_integration = CORE_DIR / "safety_integration"
    
    if old_safety_integration.exists():
        # Check if empty (only .gitkeep allowed)
        files = list(old_safety_integration.glob("*"))
        gitkeep_files = [f for f in files if f.name == ".gitkeep"]
        
        if len(files) == 0 or (len(files) == 1 and len(gitkeep_files) == 1):
            print(f"  🗑️  Removing empty folder: {old_safety_integration}")
            shutil.rmtree(old_safety_integration)
        else:
            print(f"  ⚠️  Folder not empty, keeping: {old_safety_integration}")

def verify_migration():
    """Verify migration completed successfully"""
    print("\n✅ Verifying migration...")
    
    all_ok = True
    for dest_folder, files in MIGRATIONS.items():
        dest_path = CORE_DIR / dest_folder
        
        for file in files:
            filename = Path(file).name
            dest_file = dest_path / filename
            
            if not dest_file.exists():
                print(f"  ❌ Missing: {dest_file}")
                all_ok = False
    
    if all_ok:
        print("  ✅ All files migrated successfully!")
    else:
        print("  ❌ Some files missing!")
    
    return all_ok

def main():
    print("=" * 60)
    print("🚀 CORE STRUCTURE MIGRATION - OPTION 1")
    print("=" * 60)
    
    # Step 1: Create folders
    create_folders()
    
    # Step 2: Move files
    move_files()
    
    # Step 3: Cleanup
    cleanup_old_folders()
    
    # Step 4: Verify
    success = verify_migration()
    
    print("\n" + "=" * 60)
    if success:
        print("✅ Migration completed successfully!")
    else:
        print("❌ Migration completed with errors!")
    print("=" * 60)
    
    return 0 if success else 1

if __name__ == "__main__":
    exit(main())

