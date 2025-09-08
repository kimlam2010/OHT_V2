#!/usr/bin/env python3
"""
Fix Robot Mode Default Value Migration
Fixes inconsistency between database default and service default
"""

import asyncio
import logging
from sqlalchemy import text
from app.core.database import engine

logger = logging.getLogger(__name__)

async def fix_robot_mode_default():
    """Fix robot mode default value in database"""
    try:
        async with engine.begin() as conn:
            # Update existing robots with 'auto' mode to 'manual'
            result = await conn.execute(text("""
                UPDATE robots 
                SET mode = 'manual' 
                WHERE mode = 'auto'
            """))
            
            updated_count = result.rowcount
            logger.info(f"Updated {updated_count} robots from 'auto' to 'manual' mode")
            
            # Verify the change
            result = await conn.execute(text("""
                SELECT robot_id, mode, status 
                FROM robots 
                WHERE mode = 'auto'
            """))
            
            remaining_auto = result.fetchall()
            if remaining_auto:
                logger.warning(f"Still found {len(remaining_auto)} robots with 'auto' mode")
                for robot in remaining_auto:
                    logger.warning(f"Robot {robot[0]} still has mode='{robot[1]}'")
            else:
                logger.info("✅ All robots now have correct mode values")
                
    except Exception as e:
        logger.error(f"❌ Failed to fix robot mode default: {e}")
        raise

async def verify_robot_mode_consistency():
    """Verify robot mode consistency"""
    try:
        async with engine.begin() as conn:
            # Check current robot modes
            result = await conn.execute(text("""
                SELECT robot_id, mode, status, created_at 
                FROM robots 
                ORDER BY created_at DESC
            """))
            
            robots = result.fetchall()
            logger.info(f"Found {len(robots)} robots in database:")
            
            for robot in robots:
                robot_id, mode, status, created_at = robot
                logger.info(f"  Robot {robot_id}: mode='{mode}', status='{status}', created={created_at}")
                
            # Check for any inconsistencies
            invalid_modes = ['auto']  # Should not exist anymore
            for invalid_mode in invalid_modes:
                result = await conn.execute(text(f"""
                    SELECT COUNT(*) FROM robots WHERE mode = '{invalid_mode}'
                """))
                count = result.scalar()
                if count > 0:
                    logger.error(f"❌ Found {count} robots with invalid mode '{invalid_mode}'")
                else:
                    logger.info(f"✅ No robots with invalid mode '{invalid_mode}'")
                    
    except Exception as e:
        logger.error(f"❌ Failed to verify robot mode consistency: {e}")
        raise

if __name__ == "__main__":
    async def main():
        logging.basicConfig(level=logging.INFO)
        logger.info("🔧 Starting robot mode default fix...")
        
        await fix_robot_mode_default()
        await verify_robot_mode_consistency()
        
        logger.info("✅ Robot mode default fix completed")
    
    asyncio.run(main())
