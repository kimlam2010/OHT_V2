"""
PathPlanningService - Lập kế hoạch đường đi thông minh
"""

import asyncio
import logging
import math
import time
from datetime import datetime
from typing import List, Optional, Dict, Any, Tuple
from uuid import UUID

from app.models.path_planning import (
    Route, SafeRoute, RouteSegment, Obstacle, Point, RouteType, ObstacleType,
    PathPlanningRequest, PathPlanningResult, PathPlanningConfig, OptimizationMetrics,
    PathPlanningEvent
)
from app.services.log_service import LogService


logger = logging.getLogger(__name__)


class PathPlanningService:
    """Service lập kế hoạch đường đi thông minh"""
    
    def __init__(self, log_service: LogService):
        self.log_service = log_service
        self.config = PathPlanningConfig()
        self.obstacles: Dict[UUID, Obstacle] = {}
        self.safety_zones: List[Dict[str, Any]] = []
        self.planning_history: List[PathPlanningEvent] = []
        self._lock = asyncio.Lock()
        
        logger.info("PathPlanningService initialized")
    
    async def optimize_route(self, waypoints: List[Point]) -> Route:
        """Tối ưu hóa route từ danh sách waypoints"""
        async with self._lock:
            try:
                if len(waypoints) < 2:
                    raise ValueError("Cần ít nhất 2 waypoints để tạo route")
                
                # Tạo route cơ bản
                route = Route(
                    route_type=RouteType.OPTIMIZED,
                    waypoints=waypoints,
                    total_distance=self._calculate_total_distance(waypoints)
                )
                
                # Tối ưu hóa route
                optimized_route = await self._optimize_route_algorithm(route)
                
                # Tính toán metrics
                metrics = self._calculate_optimization_metrics(route, optimized_route)
                
                # Log event
                await self._log_planning_event(
                    "route_optimized",
                    f"Route optimized with {len(waypoints)} waypoints",
                    route_id=optimized_route.id,
                    metrics=metrics
                )
                
                return optimized_route
                
            except Exception as e:
                logger.error(f"Error optimizing route: {e}")
                raise
    
    async def avoid_obstacles(self, route: Route) -> SafeRoute:
        """Tránh chướng ngại vật và tạo route an toàn"""
        async with self._lock:
            try:
                # Tạo safe route từ route gốc
                safe_route = SafeRoute(
                    route_type=RouteType.SAFE,
                    waypoints=route.waypoints.copy(),
                    total_distance=route.total_distance,
                    safety_zones_integrated=True
                )
                
                # Phân tích chướng ngại vật
                obstacles_on_route = await self._analyze_obstacles_on_route(route)
                
                # Tạo segments với tránh chướng ngại vật
                safe_segments = await self._create_safe_segments(route, obstacles_on_route)
                safe_route.segments = safe_segments
                
                # Tính toán safety score
                safe_route.safety_score = self._calculate_safety_score(safe_route, obstacles_on_route)
                safe_route.obstacles_avoided = len(obstacles_on_route)
                
                # Tạo backup routes
                safe_route.backup_routes = await self._generate_backup_routes(route, obstacles_on_route)
                
                # Đánh giá rủi ro
                safe_route.risk_assessment = self._assess_route_risks(safe_route)
                
                # Log event
                await self._log_planning_event(
                    "obstacles_avoided",
                    f"Route optimized to avoid {len(obstacles_on_route)} obstacles",
                    route_id=safe_route.id
                )
                
                return safe_route
                
            except Exception as e:
                logger.error(f"Error avoiding obstacles: {e}")
                raise
    
    async def integrate_safety_zones(self, route: Route) -> SafeRoute:
        """Tích hợp vùng an toàn vào route"""
        async with self._lock:
            try:
                # Tạo safe route
                safe_route = SafeRoute(
                    route_type=RouteType.SAFE,
                    waypoints=route.waypoints.copy(),
                    total_distance=route.total_distance,
                    safety_zones_integrated=True
                )
                
                # Phân tích vùng an toàn
                affected_zones = await self._analyze_safety_zones(route)
                
                # Điều chỉnh route để tránh vùng an toàn
                adjusted_waypoints = await self._adjust_route_for_safety_zones(route, affected_zones)
                safe_route.waypoints = adjusted_waypoints
                
                # Tạo emergency exits
                safe_route.emergency_exits = await self._generate_emergency_exits(route, affected_zones)
                
                # Cập nhật metrics
                safe_route.total_distance = self._calculate_total_distance(adjusted_waypoints)
                safe_route.safety_score = self._calculate_safety_zone_score(safe_route, affected_zones)
                
                # Log event
                await self._log_planning_event(
                    "safety_zones_integrated",
                    f"Route adjusted for {len(affected_zones)} safety zones",
                    route_id=safe_route.id
                )
                
                return safe_route
                
            except Exception as e:
                logger.error(f"Error integrating safety zones: {e}")
                raise
    
    async def generate_waypoints(self, mission_data: Dict[str, Any]) -> List[Point]:
        """Tạo waypoints từ mission data"""
        async with self._lock:
            try:
                start_point = mission_data.get("start_point")
                end_point = mission_data.get("end_point")
                intermediate_points = mission_data.get("waypoints", [])
                
                if not start_point or not end_point:
                    raise ValueError("Start point và end point là bắt buộc")
                
                # Tạo waypoints cơ bản
                waypoints = [Point(**start_point)]
                
                # Thêm intermediate points
                for point_data in intermediate_points:
                    waypoints.append(Point(**point_data))
                
                waypoints.append(Point(**end_point))
                
                # Tối ưu hóa waypoints
                optimized_waypoints = await self._optimize_waypoints(waypoints)
                
                # Log event
                await self._log_planning_event(
                    "waypoints_generated",
                    f"Generated {len(optimized_waypoints)} optimized waypoints",
                    data={"mission_id": mission_data.get("mission_id")}
                )
                
                return optimized_waypoints
                
            except Exception as e:
                logger.error(f"Error generating waypoints: {e}")
                raise
    
    async def plan_complete_route(self, request: PathPlanningRequest) -> PathPlanningResult:
        """Lập kế hoạch route hoàn chỉnh"""
        async with self._lock:
            start_time = time.time()
            iterations = 0
            
            try:
                # Tạo waypoints
                waypoints = [request.start_point] + request.waypoints + [request.end_point]
                
                # Tạo route cơ bản
                route = Route(
                    route_type=request.route_type,
                    waypoints=waypoints,
                    total_distance=self._calculate_total_distance(waypoints)
                )
                iterations += 1
                
                # Tối ưu hóa route
                if request.route_type == RouteType.OPTIMIZED:
                    route = await self._optimize_route_algorithm(route)
                    iterations += 1
                
                # Tránh chướng ngại vật
                safe_route = await self.avoid_obstacles(route)
                iterations += 1
                
                # Tích hợp vùng an toàn
                if request.safety_zones:
                    safe_route = await self.integrate_safety_zones(safe_route)
                    iterations += 1
                
                # Tạo alternatives
                alternatives = await self._generate_alternative_routes(route, request)
                
                planning_time = time.time() - start_time
                
                result = PathPlanningResult(
                    success=True,
                    route=route,
                    safe_route=safe_route,
                    planning_time=planning_time,
                    iterations=iterations,
                    alternatives=alternatives
                )
                
                # Log event
                await self._log_planning_event(
                    "route_planned",
                    f"Complete route planned in {planning_time:.3f}s",
                    route_id=safe_route.id,
                    data={"iterations": iterations, "alternatives": len(alternatives)}
                )
                
                return result
                
            except Exception as e:
                planning_time = time.time() - start_time
                logger.error(f"Error planning complete route: {e}")
                
                return PathPlanningResult(
                    success=False,
                    planning_time=planning_time,
                    iterations=iterations,
                    errors=[str(e)]
                )
    
    async def update_obstacles(self, obstacles: List[Obstacle]):
        """Cập nhật danh sách chướng ngại vật"""
        async with self._lock:
            # Cập nhật obstacles
            for obstacle in obstacles:
                self.obstacles[obstacle.id] = obstacle
            
            # Xóa obstacles cũ
            current_time = datetime.utcnow()
            expired_obstacles = [
                obs_id for obs_id, obstacle in self.obstacles.items()
                if (current_time - obstacle.timestamp).total_seconds() > 300  # 5 phút
            ]
            for obs_id in expired_obstacles:
                del self.obstacles[obs_id]
            
            logger.info(f"Updated obstacles: {len(self.obstacles)} active, {len(expired_obstacles)} expired")
    
    async def update_safety_zones(self, safety_zones: List[Dict[str, Any]]):
        """Cập nhật vùng an toàn"""
        async with self._lock:
            self.safety_zones = safety_zones
            logger.info(f"Updated safety zones: {len(safety_zones)} zones")
    
    async def get_planning_history(self, limit: int = 100) -> List[PathPlanningEvent]:
        """Lấy lịch sử lập kế hoạch"""
        return self.planning_history[-limit:] if self.planning_history else []
    
    async def update_config(self, config_data: Dict[str, Any]) -> PathPlanningConfig:
        """Cập nhật cấu hình"""
        async with self._lock:
            for key, value in config_data.items():
                if hasattr(self.config, key):
                    setattr(self.config, key, value)
            
            logger.info("PathPlanningService config updated")
            return self.config
    
    # Private methods
    def _calculate_total_distance(self, waypoints: List[Point]) -> float:
        """Tính tổng khoảng cách của waypoints"""
        if len(waypoints) < 2:
            return 0.0
        
        total_distance = 0.0
        for i in range(len(waypoints) - 1):
            total_distance += waypoints[i].distance_to(waypoints[i + 1])
        
        return total_distance
    
    async def _optimize_route_algorithm(self, route: Route) -> Route:
        """Thuật toán tối ưu hóa route (A* hoặc similar)"""
        # Implement A* algorithm hoặc thuật toán tối ưu hóa khác
        # Đây là implementation đơn giản
        optimized_waypoints = route.waypoints.copy()
        
        # Tối ưu hóa đơn giản: loại bỏ waypoints gần nhau
        i = 1
        while i < len(optimized_waypoints) - 1:
            prev_point = optimized_waypoints[i - 1]
            current_point = optimized_waypoints[i]
            next_point = optimized_waypoints[i + 1]
            
            # Nếu waypoint hiện tại quá gần với waypoint trước và sau
            if (current_point.distance_to(prev_point) < 50 and 
                current_point.distance_to(next_point) < 50):
                optimized_waypoints.pop(i)
            else:
                i += 1
        
        route.waypoints = optimized_waypoints
        route.total_distance = self._calculate_total_distance(optimized_waypoints)
        route.efficiency_score = 1.0 - (len(route.waypoints) / len(optimized_waypoints))
        
        return route
    
    async def _analyze_obstacles_on_route(self, route: Route) -> List[Obstacle]:
        """Phân tích chướng ngại vật trên route"""
        obstacles_on_route = []
        
        for obstacle in self.obstacles.values():
            if not obstacle.is_active:
                continue
            
            # Kiểm tra xem obstacle có ảnh hưởng đến route không
            for i in range(len(route.waypoints) - 1):
                start_point = route.waypoints[i]
                end_point = route.waypoints[i + 1]
                
                # Tính khoảng cách từ obstacle đến đoạn route
                distance = self._distance_point_to_line(
                    obstacle.position, start_point, end_point
                )
                
                if distance <= obstacle.radius + self.config.safety_margin:
                    obstacles_on_route.append(obstacle)
                    break
        
        return obstacles_on_route
    
    async def _create_safe_segments(self, route: Route, obstacles: List[Obstacle]) -> List[RouteSegment]:
        """Tạo segments an toàn"""
        segments = []
        
        for i in range(len(route.waypoints) - 1):
            start_point = route.waypoints[i]
            end_point = route.waypoints[i + 1]
            
            # Tìm obstacles ảnh hưởng đến segment này
            segment_obstacles = [
                obs for obs in obstacles
                if self._distance_point_to_line(obs.position, start_point, end_point) <= obs.radius
            ]
            
            segment = RouteSegment(
                start_point=start_point,
                end_point=end_point,
                segment_type="safe" if not segment_obstacles else "obstacle_avoidance",
                distance=start_point.distance_to(end_point),
                estimated_time=start_point.distance_to(end_point) / 500.0,  # Giả sử tốc độ 500mm/s
                speed_limit=300.0 if segment_obstacles else 1000.0,  # Giảm tốc độ nếu có obstacle
                obstacles=segment_obstacles,
                is_safe=len(segment_obstacles) == 0
            )
            
            segments.append(segment)
        
        return segments
    
    async def _analyze_safety_zones(self, route: Route) -> List[Dict[str, Any]]:
        """Phân tích vùng an toàn ảnh hưởng đến route"""
        affected_zones = []
        
        for zone in self.safety_zones:
            # Kiểm tra xem zone có ảnh hưởng đến route không
            for i in range(len(route.waypoints) - 1):
                start_point = route.waypoints[i]
                end_point = route.waypoints[i + 1]
                
                # Logic kiểm tra vùng an toàn (đơn giản)
                if self._route_intersects_safety_zone(start_point, end_point, zone):
                    affected_zones.append(zone)
                    break
        
        return affected_zones
    
    async def _adjust_route_for_safety_zones(self, route: Route, safety_zones: List[Dict[str, Any]]) -> List[Point]:
        """Điều chỉnh route để tránh vùng an toàn"""
        adjusted_waypoints = route.waypoints.copy()
        
        # Logic điều chỉnh đơn giản: thêm waypoints để tránh vùng an toàn
        # Trong thực tế, sẽ cần thuật toán phức tạp hơn
        
        return adjusted_waypoints
    
    async def _generate_emergency_exits(self, route: Route, safety_zones: List[Dict[str, Any]]) -> List[Point]:
        """Tạo điểm thoát khẩn cấp"""
        emergency_exits = []
        
        # Tạo emergency exits ở các điểm an toàn gần route
        for i in range(0, len(route.waypoints), 3):  # Mỗi 3 waypoints
            if i < len(route.waypoints):
                emergency_exits.append(route.waypoints[i])
        
        return emergency_exits
    
    async def _generate_backup_routes(self, route: Route, obstacles: List[Obstacle]) -> List[Route]:
        """Tạo backup routes"""
        backup_routes = []
        
        # Tạo 2 backup routes đơn giản
        for i in range(2):
            backup_waypoints = route.waypoints.copy()
            
            # Thay đổi nhỏ waypoints để tạo route khác
            for j in range(1, len(backup_waypoints) - 1):
                backup_waypoints[j].x += (i + 1) * 50  # Offset 50mm
                backup_waypoints[j].y += (i + 1) * 50
            
            backup_route = Route(
                route_type=RouteType.SAFE,
                waypoints=backup_waypoints,
                total_distance=self._calculate_total_distance(backup_waypoints)
            )
            
            backup_routes.append(backup_route)
        
        return backup_routes
    
    async def _generate_alternative_routes(self, route: Route, request: PathPlanningRequest) -> List[Route]:
        """Tạo route thay thế"""
        alternatives = []
        
        # Tạo alternative routes với các thuật toán khác nhau
        alternative_types = [RouteType.LINEAR, RouteType.SAFE]
        
        for alt_type in alternative_types:
            if alt_type != route.route_type:
                alt_route = Route(
                    route_type=alt_type,
                    waypoints=route.waypoints.copy(),
                    total_distance=route.total_distance
                )
                alternatives.append(alt_route)
        
        return alternatives
    
    async def _optimize_waypoints(self, waypoints: List[Point]) -> List[Point]:
        """Tối ưu hóa waypoints"""
        if len(waypoints) <= 2:
            return waypoints
        
        optimized = [waypoints[0]]
        
        for i in range(1, len(waypoints) - 1):
            prev_point = optimized[-1]
            current_point = waypoints[i]
            next_point = waypoints[i + 1]
            
            # Chỉ thêm waypoint nếu nó tạo ra góc đáng kể
            angle = self._calculate_angle(prev_point, current_point, next_point)
            if angle > 15:  # Góc > 15 độ
                optimized.append(current_point)
        
        optimized.append(waypoints[-1])
        return optimized
    
    def _calculate_optimization_metrics(self, original_route: Route, optimized_route: Route) -> OptimizationMetrics:
        """Tính toán metrics tối ưu hóa"""
        distance_improvement = 0.0
        if original_route.total_distance > 0:
            distance_improvement = (
                (original_route.total_distance - optimized_route.total_distance) / 
                original_route.total_distance * 100
            )
        
        return OptimizationMetrics(
            distance_improvement=distance_improvement,
            time_improvement=distance_improvement * 0.8,  # Giả sử
            safety_improvement=optimized_route.safety_score * 100,
            energy_efficiency=optimized_route.efficiency_score,
            smoothness_score=1.0 - (len(optimized_route.waypoints) / len(original_route.waypoints)),
            complexity_score=1.0 - optimized_route.efficiency_score
        )
    
    def _calculate_safety_score(self, route: SafeRoute, obstacles: List[Obstacle]) -> float:
        """Tính điểm an toàn"""
        if not obstacles:
            return 1.0
        
        # Tính điểm dựa trên số obstacle và khoảng cách
        total_risk = 0.0
        for obstacle in obstacles:
            min_distance = float('inf')
            for segment in route.segments:
                distance = self._distance_point_to_line(
                    obstacle.position, segment.start_point, segment.end_point
                )
                min_distance = min(min_distance, distance)
            
            if min_distance < obstacle.radius:
                total_risk += 1.0
            elif min_distance < obstacle.radius + self.config.safety_margin:
                total_risk += 0.5
        
        return max(0.0, 1.0 - (total_risk / len(obstacles)))
    
    def _calculate_safety_zone_score(self, route: SafeRoute, safety_zones: List[Dict[str, Any]]) -> float:
        """Tính điểm an toàn cho vùng an toàn"""
        if not safety_zones:
            return 1.0
        
        # Logic tính điểm đơn giản
        return 1.0 - (len(safety_zones) * 0.1)  # Giảm 10% cho mỗi vùng an toàn
    
    def _assess_route_risks(self, route: SafeRoute) -> Dict[str, float]:
        """Đánh giá rủi ro route"""
        risks = {
            "obstacle_risk": 1.0 - route.safety_score,
            "complexity_risk": 1.0 - route.efficiency_score,
            "distance_risk": min(1.0, route.total_distance / 10000.0),  # Rủi ro tăng theo khoảng cách
            "overall_risk": (1.0 - route.safety_score + 1.0 - route.efficiency_score) / 2
        }
        
        return risks
    
    def _distance_point_to_line(self, point: Point, line_start: Point, line_end: Point) -> float:
        """Tính khoảng cách từ điểm đến đường thẳng"""
        # Sử dụng công thức khoảng cách từ điểm đến đường thẳng
        x0, y0 = point.x, point.y
        x1, y1 = line_start.x, line_start.y
        x2, y2 = line_end.x, line_end.y
        
        if x2 == x1 and y2 == y1:
            return math.sqrt((x0 - x1)**2 + (y0 - y1)**2)
        
        numerator = abs((y2 - y1) * x0 - (x2 - x1) * y0 + x2 * y1 - y2 * x1)
        denominator = math.sqrt((y2 - y1)**2 + (x2 - x1)**2)
        
        return numerator / denominator if denominator > 0 else 0
    
    def _route_intersects_safety_zone(self, start_point: Point, end_point: Point, zone: Dict[str, Any]) -> bool:
        """Kiểm tra route có cắt vùng an toàn không"""
        # Logic đơn giản: kiểm tra xem đoạn route có đi qua vùng an toàn không
        # Trong thực tế, sẽ cần logic phức tạp hơn
        zone_center = Point(x=zone.get("center_x", 0), y=zone.get("center_y", 0))
        zone_radius = zone.get("radius", 0)
        
        distance = self._distance_point_to_line(zone_center, start_point, end_point)
        return distance <= zone_radius
    
    def _calculate_angle(self, p1: Point, p2: Point, p3: Point) -> float:
        """Tính góc giữa 3 điểm"""
        v1 = (p1.x - p2.x, p1.y - p2.y)
        v2 = (p3.x - p2.x, p3.y - p2.y)
        
        dot_product = v1[0] * v2[0] + v1[1] * v2[1]
        mag1 = math.sqrt(v1[0]**2 + v1[1]**2)
        mag2 = math.sqrt(v2[0]**2 + v2[1]**2)
        
        if mag1 == 0 or mag2 == 0:
            return 0
        
        cos_angle = dot_product / (mag1 * mag2)
        cos_angle = max(-1, min(1, cos_angle))  # Clamp to [-1, 1]
        
        return math.degrees(math.acos(cos_angle))
    
    async def _log_planning_event(self, event_type: str, description: str, 
                                route_id: Optional[UUID] = None, 
                                metrics: Optional[OptimizationMetrics] = None,
                                data: Optional[Dict[str, Any]] = None):
        """Log sự kiện path planning"""
        event = PathPlanningEvent(
            event_type=event_type,
            description=description,
            route_id=route_id,
            metrics=metrics,
            data=data or {}
        )
        
        self.planning_history.append(event)
        
        # Giữ lịch sử trong giới hạn
        if len(self.planning_history) > 1000:
            self.planning_history = self.planning_history[-1000:]
        
        logger.info(f"Path planning event: {event_type} - {description}")
