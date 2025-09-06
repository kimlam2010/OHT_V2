"""
LiDAR Data Processor for OHT-50
Real-time LiDAR data processing with obstacle detection and mapping
"""

import asyncio
import logging
import math
import time
from typing import Dict, Any, List, Optional, Tuple
from datetime import datetime, timezone
from dataclasses import dataclass, field
from enum import Enum
import statistics

logger = logging.getLogger(__name__)


class ObstacleType(Enum):
    """Obstacle types detected by LiDAR"""
    STATIC = "static"
    DYNAMIC = "dynamic"
    PERSON = "person"
    VEHICLE = "vehicle"
    WALL = "wall"
    UNKNOWN = "unknown"


class ObstacleSeverity(Enum):
    """Obstacle severity levels"""
    LOW = "low"
    MEDIUM = "medium"
    HIGH = "high"
    CRITICAL = "critical"


@dataclass
class LiDARPoint:
    """LiDAR point data"""
    distance: float  # meters
    angle: float     # degrees
    intensity: float # 0.0 to 1.0
    timestamp: datetime = field(default_factory=lambda: datetime.now(timezone.utc))


@dataclass
class Obstacle:
    """Detected obstacle information"""
    id: str
    type: ObstacleType
    severity: ObstacleSeverity
    center_x: float
    center_y: float
    distance: float
    angle: float
    width: float
    height: float
    confidence: float  # 0.0 to 1.0
    points: List[LiDARPoint] = field(default_factory=list)
    timestamp: datetime = field(default_factory=lambda: datetime.now(timezone.utc))


@dataclass
class LiDARScan:
    """Complete LiDAR scan data"""
    points: List[LiDARPoint]
    obstacles: List[Obstacle]
    scan_quality: float  # 0.0 to 1.0
    processing_time_ms: float
    timestamp: datetime = field(default_factory=lambda: datetime.now(timezone.utc))


class LiDARDataProcessor:
    """
    High-performance LiDAR data processor
    Target: < 20ms processing time for 360° scan
    """
    
    def __init__(self):
        # Processing parameters
        self.max_range = 30.0  # meters
        self.min_range = 0.1   # meters
        self.angle_resolution = 1.0  # degrees
        self.obstacle_threshold = 0.5  # meters
        
        # Obstacle detection parameters
        self.cluster_distance_threshold = 0.3  # meters
        self.min_cluster_size = 3
        self.max_cluster_size = 100
        
        # Performance tracking
        self.processing_times: List[float] = []
        self.scan_count = 0
        self.obstacle_count = 0
        
        # Obstacle tracking
        self.obstacle_history: Dict[str, List[Obstacle]] = {}
        self.next_obstacle_id = 1
        
        logger.info("LiDARDataProcessor initialized")
    
    async def process_lidar_data(self, raw_data: Dict[str, Any]) -> LiDARScan:
        """
        Process raw LiDAR data into structured format
        Target: < 20ms processing time
        """
        start_time = time.perf_counter()
        
        try:
            # Extract points from raw data
            points = self._extract_points(raw_data)
            
            # Filter and validate points
            filtered_points = self._filter_points(points)
            
            # Detect obstacles
            obstacles = await self._detect_obstacles(filtered_points)
            
            # Calculate scan quality
            scan_quality = self._calculate_scan_quality(filtered_points, obstacles)
            
            # Create LiDAR scan
            processing_time = (time.perf_counter() - start_time) * 1000
            scan = LiDARScan(
                points=filtered_points,
                obstacles=obstacles,
                scan_quality=scan_quality,
                processing_time_ms=processing_time
            )
            
            # Update statistics
            self._update_stats(scan)
            
            logger.debug("Processed LiDAR scan: %d points, %d obstacles, %.2fms", 
                        len(filtered_points), len(obstacles), processing_time)
            
            return scan
            
        except Exception as e:
            logger.error("Error processing LiDAR data: %s", e)
            # Return empty scan on error
            return LiDARScan(
                points=[],
                obstacles=[],
                scan_quality=0.0,
                processing_time_ms=(time.perf_counter() - start_time) * 1000
            )
    
    def _extract_points(self, raw_data: Dict[str, Any]) -> List[LiDARPoint]:
        """Extract LiDAR points from raw data"""
        points = []
        
        try:
            # Handle different data formats
            if "points" in raw_data:
                # Structured format
                for point_data in raw_data["points"]:
                    point = LiDARPoint(
                        distance=point_data.get("distance", 0.0),
                        angle=point_data.get("angle", 0.0),
                        intensity=point_data.get("intensity", 0.0)
                    )
                    points.append(point)
            
            elif "distances" in raw_data and "angles" in raw_data:
                # Array format
                distances = raw_data["distances"]
                angles = raw_data["angles"]
                intensities = raw_data.get("intensities", [0.0] * len(distances))
                
                for i in range(min(len(distances), len(angles))):
                    point = LiDARPoint(
                        distance=distances[i],
                        angle=angles[i],
                        intensity=intensities[i] if i < len(intensities) else 0.0
                    )
                    points.append(point)
            
            else:
                # Simulate 360° scan if no data
                logger.warning("No valid LiDAR data format found, generating simulated data")
                points = self._generate_simulated_scan()
            
        except Exception as e:
            logger.error("Error extracting LiDAR points: %s", e)
            points = self._generate_simulated_scan()
        
        return points
    
    def _generate_simulated_scan(self) -> List[LiDARPoint]:
        """Generate simulated LiDAR scan for testing"""
        points = []
        
        for angle in range(0, 360, int(self.angle_resolution)):
            # Simulate some obstacles
            distance = self.max_range
            
            # Add some simulated obstacles
            if 45 <= angle <= 55:  # Obstacle at 50 degrees
                distance = 2.0
            elif 180 <= angle <= 190:  # Obstacle at 185 degrees
                distance = 1.5
            elif 270 <= angle <= 280:  # Obstacle at 275 degrees
                distance = 3.0
            
            # Add some noise
            distance += (time.time() % 1.0 - 0.5) * 0.1
            
            point = LiDARPoint(
                distance=max(self.min_range, min(distance, self.max_range)),
                angle=float(angle),
                intensity=0.8 if distance < 5.0 else 0.3
            )
            points.append(point)
        
        return points
    
    def _filter_points(self, points: List[LiDARPoint]) -> List[LiDARPoint]:
        """Filter and validate LiDAR points"""
        filtered_points = []
        
        for point in points:
            # Range validation
            if not (self.min_range <= point.distance <= self.max_range):
                continue
            
            # Angle validation
            if not (0 <= point.angle < 360):
                continue
            
            # Intensity validation
            if not (0.0 <= point.intensity <= 1.0):
                point.intensity = max(0.0, min(1.0, point.intensity))
            
            filtered_points.append(point)
        
        return filtered_points
    
    async def _detect_obstacles(self, points: List[LiDARPoint]) -> List[Obstacle]:
        """Detect obstacles from LiDAR points"""
        obstacles = []
        
        try:
            # Cluster nearby points
            clusters = self._cluster_points(points)
            
            # Process each cluster
            for cluster in clusters:
                if len(cluster) >= self.min_cluster_size:
                    obstacle = self._create_obstacle_from_cluster(cluster)
                    if obstacle:
                        obstacles.append(obstacle)
            
            # Track obstacles over time
            obstacles = await self._track_obstacles(obstacles)
            
        except Exception as e:
            logger.error("Error detecting obstacles: %s", e)
        
        return obstacles
    
    def _cluster_points(self, points: List[LiDARPoint]) -> List[List[LiDARPoint]]:
        """Cluster nearby LiDAR points"""
        clusters = []
        used_points = set()
        
        for i, point in enumerate(points):
            if i in used_points:
                continue
            
            cluster = [point]
            used_points.add(i)
            
            # Find nearby points
            for j, other_point in enumerate(points):
                if j in used_points:
                    continue
                
                # Calculate distance between points
                distance = self._calculate_point_distance(point, other_point)
                
                if distance <= self.cluster_distance_threshold:
                    cluster.append(other_point)
                    used_points.add(j)
            
            if len(cluster) >= self.min_cluster_size:
                clusters.append(cluster)
        
        return clusters
    
    def _calculate_point_distance(self, point1: LiDARPoint, point2: LiDARPoint) -> float:
        """Calculate distance between two LiDAR points"""
        # Convert to Cartesian coordinates
        x1 = point1.distance * math.cos(math.radians(point1.angle))
        y1 = point1.distance * math.sin(math.radians(point1.angle))
        x2 = point2.distance * math.cos(math.radians(point2.angle))
        y2 = point2.distance * math.sin(math.radians(point2.angle))
        
        return math.sqrt((x2 - x1)**2 + (y2 - y1)**2)
    
    def _create_obstacle_from_cluster(self, cluster: List[LiDARPoint]) -> Optional[Obstacle]:
        """Create obstacle from point cluster"""
        try:
            if len(cluster) < self.min_cluster_size:
                return None
            
            # Calculate cluster center
            center_x = sum(p.distance * math.cos(math.radians(p.angle)) for p in cluster) / len(cluster)
            center_y = sum(p.distance * math.sin(math.radians(p.angle)) for p in cluster) / len(cluster)
            
            # Calculate distance and angle to center
            distance = math.sqrt(center_x**2 + center_y**2)
            angle = math.degrees(math.atan2(center_y, center_x))
            if angle < 0:
                angle += 360
            
            # Calculate obstacle dimensions
            min_x = min(p.distance * math.cos(math.radians(p.angle)) for p in cluster)
            max_x = max(p.distance * math.cos(math.radians(p.angle)) for p in cluster)
            min_y = min(p.distance * math.sin(math.radians(p.angle)) for p in cluster)
            max_y = max(p.distance * math.sin(math.radians(p.angle)) for p in cluster)
            
            width = max_x - min_x
            height = max_y - min_y
            
            # Determine obstacle type and severity
            obstacle_type, severity = self._classify_obstacle(cluster, distance, width, height)
            
            # Calculate confidence
            confidence = self._calculate_obstacle_confidence(cluster, distance)
            
            obstacle = Obstacle(
                id=f"obs_{self.next_obstacle_id}",
                type=obstacle_type,
                severity=severity,
                center_x=center_x,
                center_y=center_y,
                distance=distance,
                angle=angle,
                width=width,
                height=height,
                confidence=confidence,
                points=cluster
            )
            
            self.next_obstacle_id += 1
            return obstacle
            
        except Exception as e:
            logger.error("Error creating obstacle from cluster: %s", e)
            return None
    
    def _classify_obstacle(self, cluster: List[LiDARPoint], distance: float, 
                          width: float, height: float) -> Tuple[ObstacleType, ObstacleSeverity]:
        """Classify obstacle type and severity"""
        # Determine type based on size and distance
        if distance < 1.0:
            if width < 0.5 and height < 0.5:
                return ObstacleType.PERSON, ObstacleSeverity.CRITICAL
            elif width < 2.0 and height < 2.0:
                return ObstacleType.VEHICLE, ObstacleSeverity.HIGH
            else:
                return ObstacleType.WALL, ObstacleSeverity.HIGH
        elif distance < 3.0:
            if width < 1.0 and height < 1.0:
                return ObstacleType.PERSON, ObstacleSeverity.MEDIUM
            elif width < 3.0 and height < 3.0:
                return ObstacleType.VEHICLE, ObstacleSeverity.MEDIUM
            else:
                return ObstacleType.STATIC, ObstacleSeverity.MEDIUM
        else:
            return ObstacleType.STATIC, ObstacleSeverity.LOW
    
    def _calculate_obstacle_confidence(self, cluster: List[LiDARPoint], distance: float) -> float:
        """Calculate obstacle detection confidence"""
        # Base confidence on cluster size and distance
        size_factor = min(len(cluster) / 10.0, 1.0)  # More points = higher confidence
        distance_factor = max(0.0, 1.0 - distance / 10.0)  # Closer = higher confidence
        
        return (size_factor + distance_factor) / 2.0
    
    async def _track_obstacles(self, obstacles: List[Obstacle]) -> List[Obstacle]:
        """Track obstacles over time for better classification"""
        current_time = datetime.now(timezone.utc)
        
        for obstacle in obstacles:
            # Update obstacle history
            if obstacle.id not in self.obstacle_history:
                self.obstacle_history[obstacle.id] = []
            
            self.obstacle_history[obstacle.id].append(obstacle)
            
            # Keep only recent history (last 10 scans)
            if len(self.obstacle_history[obstacle.id]) > 10:
                self.obstacle_history[obstacle.id].pop(0)
            
            # Improve classification based on history
            if len(self.obstacle_history[obstacle.id]) > 3:
                obstacle = self._improve_obstacle_classification(obstacle)
        
        return obstacles
    
    def _improve_obstacle_classification(self, obstacle: Obstacle) -> Obstacle:
        """Improve obstacle classification based on history"""
        history = self.obstacle_history.get(obstacle.id, [])
        
        if len(history) < 3:
            return obstacle
        
        # Check if obstacle is moving (dynamic vs static)
        recent_positions = [(obs.center_x, obs.center_y) for obs in history[-3:]]
        movement = 0.0
        
        for i in range(1, len(recent_positions)):
            dx = recent_positions[i][0] - recent_positions[i-1][0]
            dy = recent_positions[i][1] - recent_positions[i-1][1]
            movement += math.sqrt(dx**2 + dy**2)
        
        # If moving significantly, classify as dynamic
        if movement > 0.5:  # 50cm movement threshold
            if obstacle.type == ObstacleType.STATIC:
                obstacle.type = ObstacleType.DYNAMIC
                obstacle.severity = ObstacleSeverity.HIGH
        
        return obstacle
    
    def _calculate_scan_quality(self, points: List[LiDARPoint], obstacles: List[Obstacle]) -> float:
        """Calculate overall scan quality"""
        if not points:
            return 0.0
        
        # Factors affecting quality
        point_density = len(points) / 360.0  # points per degree
        coverage = self._calculate_coverage(points)
        consistency = self._calculate_consistency(points)
        
        # Weighted quality score
        quality = (
            min(point_density / 2.0, 1.0) * 0.4 +  # Point density (40%)
            coverage * 0.3 +                        # Coverage (30%)
            consistency * 0.3                       # Consistency (30%)
        )
        
        return max(0.0, min(1.0, quality))
    
    def _calculate_coverage(self, points: List[LiDARPoint]) -> float:
        """Calculate angular coverage of scan"""
        if not points:
            return 0.0
        
        angles = [p.angle for p in points]
        angles.sort()
        
        # Calculate gaps in coverage
        gaps = []
        for i in range(1, len(angles)):
            gap = angles[i] - angles[i-1]
            if gap > self.angle_resolution * 2:  # Significant gap
                gaps.append(gap)
        
        # Coverage is inverse of gap size
        total_gaps = sum(gaps)
        coverage = max(0.0, 1.0 - total_gaps / 360.0)
        
        return coverage
    
    def _calculate_consistency(self, points: List[LiDARPoint]) -> float:
        """Calculate data consistency"""
        if len(points) < 10:
            return 0.5
        
        # Check for reasonable distance distribution
        distances = [p.distance for p in points]
        mean_distance = statistics.mean(distances)
        std_distance = statistics.stdev(distances) if len(distances) > 1 else 0.0
        
        # Consistency based on standard deviation
        if mean_distance > 0:
            cv = std_distance / mean_distance  # Coefficient of variation
            consistency = max(0.0, 1.0 - cv)
        else:
            consistency = 0.0
        
        return consistency
    
    def _update_stats(self, scan: LiDARScan):
        """Update processing statistics"""
        self.scan_count += 1
        self.obstacle_count += len(scan.obstacles)
        self.processing_times.append(scan.processing_time_ms)
        
        # Keep only recent processing times
        if len(self.processing_times) > 100:
            self.processing_times.pop(0)
    
    def get_processing_stats(self) -> Dict[str, Any]:
        """Get processing statistics"""
        avg_processing_time = statistics.mean(self.processing_times) if self.processing_times else 0.0
        max_processing_time = max(self.processing_times) if self.processing_times else 0.0
        min_processing_time = min(self.processing_times) if self.processing_times else 0.0
        
        return {
            "scan_count": self.scan_count,
            "obstacle_count": self.obstacle_count,
            "avg_processing_time_ms": avg_processing_time,
            "max_processing_time_ms": max_processing_time,
            "min_processing_time_ms": min_processing_time,
            "avg_obstacles_per_scan": self.obstacle_count / max(self.scan_count, 1),
            "tracked_obstacles": len(self.obstacle_history)
        }
    
    def get_obstacle_history(self, obstacle_id: str) -> List[Obstacle]:
        """Get obstacle tracking history"""
        return self.obstacle_history.get(obstacle_id, [])
    
    def clear_obstacle_history(self):
        """Clear obstacle tracking history"""
        self.obstacle_history.clear()
        self.next_obstacle_id = 1
        logger.info("Cleared obstacle tracking history")


# Global LiDAR data processor instance
lidar_data_processor = LiDARDataProcessor()
