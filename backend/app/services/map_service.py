"""
Map Service - Handles map creation, management, and localization
"""

import asyncio
import uuid
import numpy as np
from datetime import datetime, timezone
from typing import Dict, List, Optional, Any
from sqlalchemy import select
from sqlalchemy.ext.asyncio import AsyncSession

from app.models.map import Map, MapSession, RobotPosition
from app.core.database import get_db_context
from app.services.hybrid_localization_engine import HybridLocalizationEngine


class MapService:
    """Service for managing maps and localization"""
    
    def __init__(self):
        self.is_mapping = False
        self.current_session: Optional[MapSession] = None
        self.current_map: Optional[Map] = None
        self.mapping_lock = asyncio.Lock()
        self.localization_engine = HybridLocalizationEngine()
        
    async def start_mapping(self, map_name: str, resolution: float = 0.05, 
                           width: int = 1000, height: int = 1000) -> Dict[str, Any]:
        """
        Start a new mapping session
        
        Args:
            map_name: Name of the map
            resolution: Resolution in mm per pixel
            width: Map width in pixels
            height: Map height in pixels
            
        Returns:
            Dict with mapping session info
        """
        async with self.mapping_lock:
            if self.is_mapping:
                raise ValueError("Mapping already in progress")
                
            try:
                # Create new map
                map_id = f"map_{uuid.uuid4().hex[:8]}"
                session_id = f"session_{uuid.uuid4().hex[:8]}"
                
                # Initialize empty occupancy grid
                occupancy_grid = np.full((height, width), -1, dtype=np.int8).tolist()
                
                # Create map record
                async with get_db_context() as db:
                    new_map = Map(
                        map_id=map_id,
                        name=map_name,
                        resolution=resolution,
                        width=width,
                        height=height,
                        occupancy_grid=occupancy_grid,
                        robot_trajectory=[],
                        rfid_positions=[],
                        map_metadata={
                            "created_by": "system",
                            "version": "1.0",
                            "coordinate_system": "cartesian"
                        }
                    )
                    
                    db.add(new_map)
                    await db.commit()
                    await db.refresh(new_map)
                    
                    # Create mapping session
                    session = MapSession(
                        session_id=session_id,
                        map_id=map_id,
                        start_time=datetime.now(timezone.utc),
                        is_active=True
                    )
                    
                    db.add(session)
                    await db.commit()
                    await db.refresh(session)
                    
                    # Update service state
                    self.is_mapping = True
                    self.current_session = session
                    self.current_map = new_map
                    
                    return {
                        "success": True,
                        "session_id": session_id,
                        "map_id": map_id,
                        "message": "Mapping session started successfully"
                    }
                    
            except Exception as e:
                self.is_mapping = False
                self.current_session = None
                self.current_map = None
                raise Exception(f"Failed to start mapping: {str(e)}")
    
    async def stop_mapping(self) -> Dict[str, Any]:
        """Stop the current mapping session"""
        async with self.mapping_lock:
            if not self.is_mapping or not self.current_session:
                raise ValueError("No active mapping session")
                
            try:
                async with get_db_context() as db:
                    # Update session
                    self.current_session.end_time = datetime.now(timezone.utc)
                    self.current_session.is_active = False
                    
                    # Calculate mapping quality
                    if self.current_map:
                        coverage = self._calculate_map_coverage(self.current_map.occupancy_grid)
                        self.current_session.mapping_quality = coverage
                    
                    await db.commit()
                    
                    # Reset service state
                    self.is_mapping = False
                    self.current_session = None
                    self.current_map = None
                    
                    return {
                        "success": True,
                        "message": "Mapping session stopped successfully"
                    }
                    
            except Exception as e:
                raise Exception(f"Failed to stop mapping: {str(e)}")
    
    async def update_occupancy_grid(self, lidar_data: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Update the occupancy grid with new LiDAR data"""
        if not self.is_mapping or not self.current_map:
            raise ValueError("No active mapping session")
            
        try:
            # Process LiDAR data
            updated_grid = await self._process_lidar_data(lidar_data)
            
            # Update map
            async with get_db_context() as db:
                self.current_map.occupancy_grid = updated_grid.tolist()
                
                # Update session statistics
                if self.current_session:
                    self.current_session.total_scans += 1
                
                await db.commit()
                
                return {
                    "success": True,
                    "message": "Occupancy grid updated successfully"
                }
                
        except Exception as e:
            raise Exception(f"Failed to update occupancy grid: {str(e)}")
    
    async def load_map(self, map_id: str) -> Dict[str, Any]:
        """Load a map by ID"""
        try:
            async with get_db_context() as db:
                result = await db.execute(select(Map).filter(Map.map_id == map_id))
                map_record = result.scalar_one_or_none()
                
                if not map_record:
                    raise ValueError(f"Map {map_id} not found")
                
                # Initialize localization engine
                self.localization_engine.initialize(
                    map_record.occupancy_grid,
                    map_record.resolution
                )
                
                return {
                    "success": True,
                    "map": {
                        "map_id": map_record.map_id,
                        "name": map_record.name,
                        "resolution": map_record.resolution,
                        "width": map_record.width,
                        "height": map_record.height,
                        "created_at": map_record.created_at.isoformat(),
                        "updated_at": map_record.updated_at.isoformat(),
                        "metadata": map_record.map_metadata
                    }
                }
                
        except Exception as e:
            raise Exception(f"Failed to load map: {str(e)}")
    
    async def get_map_list(self) -> Dict[str, Any]:
        """Get list of all maps"""
        try:
            async with get_db_context() as db:
                result = await db.execute(select(Map))
                maps = result.scalars().all()
            
            map_list = []
            for map_record in maps:
                map_list.append({
                    "map_id": map_record.map_id,
                    "name": map_record.name,
                    "resolution": map_record.resolution,
                    "width": map_record.width,
                    "height": map_record.height,
                    "created_at": map_record.created_at.isoformat(),
                    "updated_at": map_record.updated_at.isoformat(),
                    "metadata": map_record.map_metadata
                })
                
            return {
                "success": True,
                "maps": map_list
            }
            
        except Exception as e:
            raise Exception(f"Failed to get map list: {str(e)}")
    
    async def delete_map(self, map_id: str) -> Dict[str, Any]:
        """Delete a map by ID"""
        try:
            async with get_db_context() as db:
                result = await db.execute(select(Map).filter(Map.map_id == map_id))
                map_record = result.scalar_one_or_none()
                
                if not map_record:
                    raise ValueError(f"Map {map_id} not found")
                
                await db.delete(map_record)
                await db.commit()
                
                return {
                    "success": True,
                    "message": f"Map {map_id} deleted successfully"
                }
                
        except Exception as e:
            raise Exception(f"Failed to delete map: {str(e)}")
    
    async def get_robot_position(self) -> Dict[str, Any]:
        """Get current robot position using localization"""
        if not self.current_map:
            return {"success": False, "error": "No active map for localization"}
            
        try:
            # Get sensor data (mock for now)
            sensor_data = {
                "rfid": [],
                "accelerometer": {"x": 0, "y": 0, "z": 0},
                "proximity": [],
                "lidar": []
            }
            
            # Use localization engine
            position = await self.localization_engine.estimate_position(sensor_data)
            
            # Store position
            await self._store_robot_position(position)
            
            return {
                "success": True,
                "position": {
                    "x": position["x"],
                    "y": position["y"],
                    "theta": position["theta"],
                    "confidence": position["confidence"],
                    "timestamp": datetime.now(timezone.utc).isoformat()
                }
            }
            
        except Exception as e:
            return {"success": False, "error": f"Localization failed: {str(e)}"}
    
    async def _store_robot_position(self, position: Dict[str, Any]) -> None:
        """Store robot position in database"""
        try:
            async with get_db_context() as db:
                robot_pos = RobotPosition(
                    map_id=self.current_map.map_id,
                    x=position["x"],
                    y=position["y"],
                    theta=position["theta"],
                    confidence=position["confidence"],
                    timestamp=datetime.now(timezone.utc)
                )
                
                db.add(robot_pos)
                await db.commit()
                
        except Exception as e:
            # Log error but don't fail the main operation
            print(f"Failed to store robot position: {e}")
    
    async def _process_lidar_data(self, lidar_data: List[Dict[str, Any]]) -> np.ndarray:
        """Process LiDAR data and update occupancy grid"""
        # Simple LiDAR processing - convert polar to cartesian
        if not self.current_map:
            raise ValueError("No active map")
            
        # Get current grid
        grid = np.array(self.current_map.occupancy_grid)
        
        # Process each LiDAR point
        for point in lidar_data:
            distance = point.get("distance", 0)
            angle = point.get("angle", 0)
            
            if distance > 0:
                # Convert to cartesian coordinates
                x = distance * np.cos(np.radians(angle))
                y = distance * np.sin(np.radians(angle))
                
                # Convert to grid coordinates
                grid_x = int(x / self.current_map.resolution + self.current_map.width / 2)
                grid_y = int(y / self.current_map.resolution + self.current_map.height / 2)
                
                # Update grid (simple approach)
                if 0 <= grid_x < self.current_map.width and 0 <= grid_y < self.current_map.height:
                    grid[grid_y, grid_x] = 1  # Occupied
        
        return grid
    
    def get_mapping_status(self) -> Dict[str, Any]:
        """Get current mapping status"""
        return {
            "is_mapping": self.is_mapping,
            "current_map_id": self.current_map.map_id if self.current_map else None,
            "current_session_id": self.current_session.session_id if self.current_session else None,
            "mapping_progress": 0.0,  # Placeholder
            "total_scans": self.current_session.total_scans if self.current_session else 0,
            "mapping_quality": self.current_session.mapping_quality if self.current_session else 0.0
        }
    
    def _calculate_map_coverage(self, occupancy_grid: List[List[int]]) -> float:
        """Calculate map coverage percentage"""
        if not occupancy_grid:
            return 0.0
            
        grid = np.array(occupancy_grid)
        total_cells = grid.size
        explored_cells = np.sum(grid != -1)
        
        return (explored_cells / total_cells) * 100.0