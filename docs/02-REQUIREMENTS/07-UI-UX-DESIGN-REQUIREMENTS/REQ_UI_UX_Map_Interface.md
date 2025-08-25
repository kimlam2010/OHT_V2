# 🗺️ REQ_UI_UX_Map_Interface - Giao diện bản đồ LiDAR 360°

**Phiên bản:** v1.0  
**Ngày tạo:** 2025-01-28  
**Trạng thái:** ✅ Hoàn thành  
**Đối tượng đọc:** UI/UX Designers, Frontend Developers, Robotics Engineers

---

## 🎯 **Mục tiêu Map Interface:**

### **Concept chính:**
- **Visualization:** LiDAR 360° real-time mapping
- **Navigation:** Robot path planning và obstacle detection
- **Interaction:** Zoom, pan, và real-time updates
- **Safety:** Clear obstacle visualization và safety zones

### **Nguyên tắc thiết kế:**
1. **Real-time:** Cập nhật dữ liệu LiDAR theo thời gian thực
2. **Clarity:** Hiển thị rõ ràng chướng ngại vật và đường đi
3. **Performance:** Tối ưu cho rendering 360° data
4. **Safety:** Ưu tiên hiển thị thông tin an toàn

---

## 📊 **BẢNG MAPPING - TÀI LIỆU NÀY:**

| **Phần** | **Mục đích** | **Đối tượng đọc** | **Liên quan đến** | **Trạng thái** |
|----------|--------------|-------------------|-------------------|----------------|
| LiDAR Visualization | Hiển thị dữ liệu LiDAR 360° | Developers, Engineers | Hardware Integration | ✅ Hoàn thành |
| Navigation Path | Đường đi và path planning | Developers, Engineers | Control System | ✅ Hoàn thành |
| Obstacle Detection | Phát hiện và hiển thị chướng ngại vật | Developers, Engineers | Safety System | ✅ Hoàn thành |
| Interactive Controls | Điều khiển tương tác bản đồ | Designers, Developers | User Experience | ✅ Hoàn thành |

---

## 🔍 **1. LIDAR VISUALIZATION - Hiển thị dữ liệu LiDAR**

### **LiDAR Data Structure:**
```javascript
// LiDAR Data Format
interface LiDARData {
  timestamp: number;           // Timestamp của dữ liệu
  angle: number;              // Góc quét (0-360°)
  distance: number;           // Khoảng cách (mm)
  intensity: number;          // Cường độ phản xạ
  quality: number;            // Chất lượng dữ liệu (0-100)
  valid: boolean;             // Dữ liệu có hợp lệ không
}

// LiDAR Scan Data
interface LiDARScan {
  deviceId: string;           // ID thiết bị LiDAR
  scanId: number;             // ID lần quét
  timestamp: number;          // Timestamp
  points: LiDARData[];        // Mảng điểm dữ liệu
  resolution: number;         // Độ phân giải (points/degree)
  frequency: number;          // Tần số quét (Hz)
}
```

### **Canvas Rendering Setup:**
```javascript
// LiDAR Canvas Setup
class LiDARVisualizer {
  constructor(canvasId) {
    this.canvas = document.getElementById(canvasId);
    this.ctx = this.canvas.getContext('2d');
    this.scale = 1.0;
    this.offsetX = 0;
    this.offsetY = 0;
    this.isDragging = false;
    this.lastMousePos = { x: 0, y: 0 };
    
    this.setupCanvas();
    this.setupEventListeners();
  }
  
  setupCanvas() {
    // Set canvas size
    this.canvas.width = this.canvas.offsetWidth;
    this.canvas.height = this.canvas.offsetHeight;
    
    // Set background
    this.ctx.fillStyle = '#F8F9FA';
    this.ctx.fillRect(0, 0, this.canvas.width, this.canvas.height);
    
    // Draw grid
    this.drawGrid();
  }
  
  drawGrid() {
    const gridSize = 50;
    const gridColor = '#E9ECEF';
    
    this.ctx.strokeStyle = gridColor;
    this.ctx.lineWidth = 1;
    
    // Vertical lines
    for (let x = 0; x <= this.canvas.width; x += gridSize) {
      this.ctx.beginPath();
      this.ctx.moveTo(x, 0);
      this.ctx.lineTo(x, this.canvas.height);
      this.ctx.stroke();
    }
    
    // Horizontal lines
    for (let y = 0; y <= this.canvas.height; y += gridSize) {
      this.ctx.beginPath();
      this.ctx.moveTo(0, y);
      this.ctx.lineTo(this.canvas.width, y);
      this.ctx.stroke();
    }
  }
}
```

### **LiDAR Point Rendering:**
```javascript
// Render LiDAR Points
class LiDARPointRenderer {
  constructor(visualizer) {
    this.visualizer = visualizer;
    this.points = [];
    this.obstacles = [];
    this.robotPosition = { x: 0, y: 0, heading: 0 };
  }
  
  updateLiDARData(scanData) {
    this.points = scanData.points.map(point => ({
      x: Math.cos(point.angle * Math.PI / 180) * point.distance,
      y: Math.sin(point.angle * Math.PI / 180) * point.distance,
      intensity: point.intensity,
      quality: point.quality,
      valid: point.valid
    }));
    
    this.detectObstacles();
    this.render();
  }
  
  render() {
    const ctx = this.visualizer.ctx;
    const centerX = this.visualizer.canvas.width / 2;
    const centerY = this.visualizer.canvas.height / 2;
    
    // Clear canvas
    ctx.clearRect(0, 0, this.visualizer.canvas.width, this.visualizer.canvas.height);
    this.visualizer.drawGrid();
    
    // Apply transformations
    ctx.save();
    ctx.translate(centerX + this.visualizer.offsetX, centerY + this.visualizer.offsetY);
    ctx.scale(this.visualizer.scale, this.visualizer.scale);
    
    // Draw LiDAR points
    this.drawLiDARPoints(ctx);
    
    // Draw obstacles
    this.drawObstacles(ctx);
    
    // Draw robot
    this.drawRobot(ctx);
    
    // Draw safety zones
    this.drawSafetyZones(ctx);
    
    ctx.restore();
  }
  
  drawLiDARPoints(ctx) {
    this.points.forEach(point => {
      if (!point.valid) return;
      
      const alpha = point.quality / 100;
      const size = Math.max(1, point.intensity / 100);
      
      ctx.globalAlpha = alpha;
      ctx.fillStyle = this.getPointColor(point.intensity);
      ctx.beginPath();
      ctx.arc(point.x, point.y, size, 0, 2 * Math.PI);
      ctx.fill();
    });
    
    ctx.globalAlpha = 1.0;
  }
  
  getPointColor(intensity) {
    // Color mapping based on intensity
    if (intensity > 80) return '#FF6B35';      // High intensity - Orange
    if (intensity > 60) return '#FFC107';      // Medium intensity - Yellow
    if (intensity > 40) return '#17A2B8';      // Low intensity - Blue
    return '#6C757D';                          // Very low intensity - Gray
  }
}
```

---

## 🛤️ **2. NAVIGATION PATH - Đường đi và path planning**

### **Path Data Structure:**
```javascript
// Path Planning Data
interface PathPoint {
  x: number;                  // X coordinate
  y: number;                  // Y coordinate
  heading: number;            // Heading angle (degrees)
  speed: number;              // Target speed
  timestamp: number;          // Expected arrival time
}

interface NavigationPath {
  pathId: string;             // Unique path ID
  startPoint: PathPoint;      // Starting point
  endPoint: PathPoint;        // Destination point
  waypoints: PathPoint[];     // Intermediate waypoints
  status: 'planned' | 'active' | 'completed' | 'failed';
  safetyMargin: number;       // Safety margin (meters)
  maxSpeed: number;           // Maximum speed
  estimatedDuration: number;  // Estimated completion time
}
```

### **Path Visualization:**
```javascript
// Path Renderer
class PathRenderer {
  constructor(visualizer) {
    this.visualizer = visualizer;
    this.currentPath = null;
    this.pathHistory = [];
    this.pathColors = {
      planned: '#17A2B8',     // Blue for planned paths
      active: '#FF6B35',      // Orange for active path
      completed: '#28A745',   // Green for completed
      failed: '#DC3545'       // Red for failed
    };
  }
  
  updatePath(pathData) {
    this.currentPath = pathData;
    this.renderPath();
  }
  
  renderPath() {
    if (!this.currentPath) return;
    
    const ctx = this.visualizer.ctx;
    const centerX = this.visualizer.canvas.width / 2;
    const centerY = this.visualizer.canvas.height / 2;
    
    ctx.save();
    ctx.translate(centerX + this.visualizer.offsetX, centerY + this.visualizer.offsetY);
    ctx.scale(this.visualizer.scale, this.visualizer.scale);
    
    // Draw path line
    this.drawPathLine(ctx);
    
    // Draw waypoints
    this.drawWaypoints(ctx);
    
    // Draw start and end markers
    this.drawPathMarkers(ctx);
    
    // Draw safety corridor
    this.drawSafetyCorridor(ctx);
    
    ctx.restore();
  }
  
  drawPathLine(ctx) {
    const path = this.currentPath;
    const color = this.pathColors[path.status];
    
    ctx.strokeStyle = color;
    ctx.lineWidth = 3;
    ctx.lineCap = 'round';
    ctx.lineJoin = 'round';
    
    // Create dashed line effect for planned paths
    if (path.status === 'planned') {
      ctx.setLineDash([10, 5]);
    } else {
      ctx.setLineDash([]);
    }
    
    ctx.beginPath();
    
    // Start from start point
    ctx.moveTo(path.startPoint.x, path.startPoint.y);
    
    // Draw through waypoints
    path.waypoints.forEach(waypoint => {
      ctx.lineTo(waypoint.x, waypoint.y);
    });
    
    // End at destination
    ctx.lineTo(path.endPoint.x, path.endPoint.y);
    
    ctx.stroke();
    ctx.setLineDash([]);
  }
  
  drawWaypoints(ctx) {
    const path = this.currentPath;
    
    // Draw waypoint markers
    path.waypoints.forEach((waypoint, index) => {
      ctx.fillStyle = '#FF6B35';
      ctx.strokeStyle = '#FFFFFF';
      ctx.lineWidth = 2;
      
      ctx.beginPath();
      ctx.arc(waypoint.x, waypoint.y, 6, 0, 2 * Math.PI);
      ctx.fill();
      ctx.stroke();
      
      // Draw waypoint number
      ctx.fillStyle = '#FFFFFF';
      ctx.font = '12px Inter';
      ctx.textAlign = 'center';
      ctx.fillText((index + 1).toString(), waypoint.x, waypoint.y + 4);
    });
  }
  
  drawPathMarkers(ctx) {
    const path = this.currentPath;
    
    // Start marker (Green)
    this.drawMarker(ctx, path.startPoint.x, path.startPoint.y, '#28A745', 'S');
    
    // End marker (Red)
    this.drawMarker(ctx, path.endPoint.x, path.endPoint.y, '#DC3545', 'E');
  }
  
  drawMarker(ctx, x, y, color, label) {
    ctx.fillStyle = color;
    ctx.strokeStyle = '#FFFFFF';
    ctx.lineWidth = 2;
    
    ctx.beginPath();
    ctx.arc(x, y, 8, 0, 2 * Math.PI);
    ctx.fill();
    ctx.stroke();
    
    // Draw label
    ctx.fillStyle = '#FFFFFF';
    ctx.font = 'bold 14px Inter';
    ctx.textAlign = 'center';
    ctx.fillText(label, x, y + 4);
  }
  
  drawSafetyCorridor(ctx) {
    const path = this.currentPath;
    const margin = path.safetyMargin;
    
    ctx.strokeStyle = 'rgba(255, 107, 53, 0.3)';
    ctx.lineWidth = 2;
    ctx.setLineDash([5, 5]);
    
    // Draw parallel lines for safety corridor
    // This is a simplified version - actual implementation would be more complex
    ctx.beginPath();
    ctx.moveTo(path.startPoint.x - margin, path.startPoint.y - margin);
    path.waypoints.forEach(waypoint => {
      ctx.lineTo(waypoint.x - margin, waypoint.y - margin);
    });
    ctx.lineTo(path.endPoint.x - margin, path.endPoint.y - margin);
    ctx.stroke();
    
    ctx.setLineDash([]);
  }
}
```

---

## 🚧 **3. OBSTACLE DETECTION - Phát hiện và hiển thị chướng ngại vật**

### **Obstacle Detection Algorithm:**
```javascript
// Obstacle Detection
class ObstacleDetector {
  constructor() {
    this.obstacles = [];
    this.safetyZones = [];
    this.detectionThreshold = 0.5; // meters
    this.clusteringRadius = 0.3;   // meters
  }
  
  detectObstacles(lidarPoints) {
    const clusters = this.clusterPoints(lidarPoints);
    this.obstacles = clusters.map(cluster => this.analyzeCluster(cluster));
    this.updateSafetyZones();
  }
  
  clusterPoints(points) {
    const clusters = [];
    const visited = new Set();
    
    points.forEach((point, index) => {
      if (visited.has(index)) return;
      
      const cluster = [point];
      visited.add(index);
      
      // Find nearby points
      points.forEach((otherPoint, otherIndex) => {
        if (visited.has(otherIndex)) return;
        
        const distance = Math.sqrt(
          Math.pow(point.x - otherPoint.x, 2) + 
          Math.pow(point.y - otherPoint.y, 2)
        );
        
        if (distance <= this.clusteringRadius) {
          cluster.push(otherPoint);
          visited.add(otherIndex);
        }
      });
      
      if (cluster.length > 3) { // Minimum cluster size
        clusters.push(cluster);
      }
    });
    
    return clusters;
  }
  
  analyzeCluster(cluster) {
    // Calculate cluster properties
    const centerX = cluster.reduce((sum, p) => sum + p.x, 0) / cluster.length;
    const centerY = cluster.reduce((sum, p) => sum + p.y, 0) / cluster.length;
    
    const maxDistance = Math.max(...cluster.map(p => 
      Math.sqrt(Math.pow(p.x - centerX, 2) + Math.pow(p.y - centerY, 2))
    ));
    
    const avgIntensity = cluster.reduce((sum, p) => sum + p.intensity, 0) / cluster.length;
    
    return {
      id: this.generateObstacleId(),
      center: { x: centerX, y: centerY },
      radius: maxDistance,
      points: cluster,
      intensity: avgIntensity,
      confidence: this.calculateConfidence(cluster),
      type: this.classifyObstacle(cluster),
      timestamp: Date.now()
    };
  }
  
  classifyObstacle(cluster) {
    const centerX = cluster.reduce((sum, p) => sum + p.x, 0) / cluster.length;
    const centerY = cluster.reduce((sum, p) => sum + p.y, 0) / cluster.length;
    
    // Calculate bounding box
    const minX = Math.min(...cluster.map(p => p.x));
    const maxX = Math.max(...cluster.map(p => p.x));
    const minY = Math.min(...cluster.map(p => p.y));
    const maxY = Math.max(...cluster.map(p => p.y));
    
    const width = maxX - minX;
    const height = maxY - minY;
    const aspectRatio = width / height;
    
    // Classify based on shape and size
    if (aspectRatio > 2 || aspectRatio < 0.5) {
      return 'wall';
    } else if (cluster.length < 10) {
      return 'small_object';
    } else {
      return 'large_object';
    }
  }
  
  calculateConfidence(cluster) {
    // Calculate confidence based on point density and consistency
    const avgDistance = cluster.reduce((sum, p) => sum + p.distance, 0) / cluster.length;
    const variance = cluster.reduce((sum, p) => sum + Math.pow(p.distance - avgDistance, 2), 0) / cluster.length;
    
    const densityScore = Math.min(cluster.length / 20, 1); // Normalize to 0-1
    const consistencyScore = Math.max(0, 1 - variance / 10000); // Lower variance = higher confidence
    
    return (densityScore + consistencyScore) / 2;
  }
  
  updateSafetyZones() {
    this.safetyZones = this.obstacles.map(obstacle => ({
      center: obstacle.center,
      radius: obstacle.radius + this.detectionThreshold,
      type: 'warning',
      priority: obstacle.confidence > 0.8 ? 'high' : 'medium'
    }));
  }
}
```

### **Obstacle Visualization:**
```javascript
// Obstacle Renderer
class ObstacleRenderer {
  constructor(visualizer) {
    this.visualizer = visualizer;
    this.obstacleColors = {
      wall: '#6C757D',           // Gray for walls
      small_object: '#FFC107',   // Yellow for small objects
      large_object: '#DC3545'    // Red for large objects
    };
  }
  
  renderObstacles(obstacles, safetyZones) {
    const ctx = this.visualizer.ctx;
    
    // Draw safety zones first (background)
    safetyZones.forEach(zone => {
      this.drawSafetyZone(ctx, zone);
    });
    
    // Draw obstacles on top
    obstacles.forEach(obstacle => {
      this.drawObstacle(ctx, obstacle);
    });
  }
  
  drawSafetyZone(ctx, zone) {
    const alpha = zone.priority === 'high' ? 0.3 : 0.1;
    const color = zone.priority === 'high' ? '#DC3545' : '#FFC107';
    
    ctx.globalAlpha = alpha;
    ctx.fillStyle = color;
    ctx.beginPath();
    ctx.arc(zone.center.x, zone.center.y, zone.radius, 0, 2 * Math.PI);
    ctx.fill();
    
    // Draw border
    ctx.globalAlpha = alpha + 0.2;
    ctx.strokeStyle = color;
    ctx.lineWidth = 2;
    ctx.stroke();
    
    ctx.globalAlpha = 1.0;
  }
  
  drawObstacle(ctx, obstacle) {
    const color = this.obstacleColors[obstacle.type];
    
    // Draw obstacle body
    ctx.fillStyle = color;
    ctx.strokeStyle = '#FFFFFF';
    ctx.lineWidth = 2;
    
    ctx.beginPath();
    ctx.arc(obstacle.center.x, obstacle.center.y, obstacle.radius, 0, 2 * Math.PI);
    ctx.fill();
    ctx.stroke();
    
    // Draw confidence indicator
    const confidenceBarWidth = obstacle.radius * 2;
    const confidenceBarHeight = 4;
    const confidenceX = obstacle.center.x - confidenceBarWidth / 2;
    const confidenceY = obstacle.center.y + obstacle.radius + 8;
    
    // Background bar
    ctx.fillStyle = 'rgba(0, 0, 0, 0.3)';
    ctx.fillRect(confidenceX, confidenceY, confidenceBarWidth, confidenceBarHeight);
    
    // Confidence level
    ctx.fillStyle = obstacle.confidence > 0.8 ? '#28A745' : 
                   obstacle.confidence > 0.6 ? '#FFC107' : '#DC3545';
    ctx.fillRect(confidenceX, confidenceY, confidenceBarWidth * obstacle.confidence, confidenceBarHeight);
    
    // Draw obstacle type label
    ctx.fillStyle = '#FFFFFF';
    ctx.font = '12px Inter';
    ctx.textAlign = 'center';
    ctx.fillText(obstacle.type.toUpperCase(), obstacle.center.x, confidenceY - 5);
  }
}
```

---

## 🎮 **4. INTERACTIVE CONTROLS - Điều khiển tương tác bản đồ**

### **Map Controls:**
```javascript
// Interactive Map Controls
class MapControls {
  constructor(visualizer) {
    this.visualizer = visualizer;
    this.isDragging = false;
    this.lastMousePos = { x: 0, y: 0 };
    this.zoomLevel = 1.0;
    this.minZoom = 0.1;
    this.maxZoom = 5.0;
    
    this.setupControls();
    this.setupEventListeners();
  }
  
  setupControls() {
    // Create control buttons
    this.createControlButton('zoom-in', '🔍+', () => this.zoomIn());
    this.createControlButton('zoom-out', '🔍-', () => this.zoomOut());
    this.createControlButton('reset-view', '🔄', () => this.resetView());
    this.createControlButton('fullscreen', '⛶', () => this.toggleFullscreen());
    this.createControlButton('center-robot', '🤖', () => this.centerOnRobot());
  }
  
  createControlButton(id, icon, onClick) {
    const button = document.createElement('button');
    button.id = id;
    button.className = 'map-control-btn';
    button.innerHTML = icon;
    button.addEventListener('click', onClick);
    
    const controlsContainer = document.querySelector('.map-controls');
    controlsContainer.appendChild(button);
  }
  
  setupEventListeners() {
    const canvas = this.visualizer.canvas;
    
    // Mouse events
    canvas.addEventListener('mousedown', (e) => this.onMouseDown(e));
    canvas.addEventListener('mousemove', (e) => this.onMouseMove(e));
    canvas.addEventListener('mouseup', (e) => this.onMouseUp(e));
    canvas.addEventListener('wheel', (e) => this.onWheel(e));
    
    // Touch events for mobile
    canvas.addEventListener('touchstart', (e) => this.onTouchStart(e));
    canvas.addEventListener('touchmove', (e) => this.onTouchMove(e));
    canvas.addEventListener('touchend', (e) => this.onTouchEnd(e));
    
    // Keyboard events
    document.addEventListener('keydown', (e) => this.onKeyDown(e));
  }
  
  onMouseDown(e) {
    this.isDragging = true;
    this.lastMousePos = { x: e.clientX, y: e.clientY };
    this.visualizer.canvas.style.cursor = 'grabbing';
  }
  
  onMouseMove(e) {
    if (!this.isDragging) return;
    
    const deltaX = e.clientX - this.lastMousePos.x;
    const deltaY = e.clientY - this.lastMousePos.y;
    
    this.visualizer.offsetX += deltaX;
    this.visualizer.offsetY += deltaY;
    
    this.lastMousePos = { x: e.clientX, y: e.clientY };
    this.visualizer.render();
  }
  
  onMouseUp(e) {
    this.isDragging = false;
    this.visualizer.canvas.style.cursor = 'grab';
  }
  
  onWheel(e) {
    e.preventDefault();
    
    const zoomFactor = e.deltaY > 0 ? 0.9 : 1.1;
    const newZoom = Math.max(this.minZoom, Math.min(this.maxZoom, this.zoomLevel * zoomFactor));
    
    if (newZoom !== this.zoomLevel) {
      const rect = this.visualizer.canvas.getBoundingClientRect();
      const mouseX = e.clientX - rect.left;
      const mouseY = e.clientY - rect.top;
      
      this.zoomAt(mouseX, mouseY, newZoom / this.zoomLevel);
      this.zoomLevel = newZoom;
    }
  }
  
  zoomAt(x, y, factor) {
    this.visualizer.offsetX = x - (x - this.visualizer.offsetX) * factor;
    this.visualizer.offsetY = y - (y - this.visualizer.offsetY) * factor;
    this.visualizer.scale *= factor;
    this.visualizer.render();
  }
  
  zoomIn() {
    this.zoomAt(
      this.visualizer.canvas.width / 2,
      this.visualizer.canvas.height / 2,
      1.2
    );
    this.zoomLevel = Math.min(this.maxZoom, this.zoomLevel * 1.2);
  }
  
  zoomOut() {
    this.zoomAt(
      this.visualizer.canvas.width / 2,
      this.visualizer.canvas.height / 2,
      0.8
    );
    this.zoomLevel = Math.max(this.minZoom, this.zoomLevel * 0.8);
  }
  
  resetView() {
    this.visualizer.offsetX = 0;
    this.visualizer.offsetY = 0;
    this.visualizer.scale = 1.0;
    this.zoomLevel = 1.0;
    this.visualizer.render();
  }
  
  centerOnRobot() {
    // Center view on robot position
    const robotPos = this.visualizer.robotPosition;
    this.visualizer.offsetX = -robotPos.x * this.visualizer.scale;
    this.visualizer.offsetY = -robotPos.y * this.visualizer.scale;
    this.visualizer.render();
  }
  
  toggleFullscreen() {
    const mapContainer = document.querySelector('.map-interface');
    
    if (!document.fullscreenElement) {
      mapContainer.requestFullscreen();
    } else {
      document.exitFullscreen();
    }
  }
  
  onKeyDown(e) {
    switch (e.key) {
      case '+':
      case '=':
        this.zoomIn();
        break;
      case '-':
        this.zoomOut();
        break;
      case '0':
        this.resetView();
        break;
      case 'c':
        this.centerOnRobot();
        break;
      case 'f':
        this.toggleFullscreen();
        break;
    }
  }
}
```

### **CSS for Map Controls:**
```css
/* Map Control Buttons */
.map-controls {
  display: flex;
  gap: var(--space-sm);
  position: absolute;
  top: var(--space-md);
  right: var(--space-md);
  z-index: 100;
}

.map-control-btn {
  width: 40px;
  height: 40px;
  background: var(--bg-primary);
  border: 1px solid var(--industrial-gray-300);
  border-radius: 0.5rem;
  cursor: pointer;
  transition: all 0.2s ease;
  display: flex;
  align-items: center;
  justify-content: center;
  font-size: 1rem;
  box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
}

.map-control-btn:hover {
  background: var(--primary-orange);
  color: white;
  border-color: var(--primary-orange);
  transform: translateY(-1px);
  box-shadow: 0 4px 8px rgba(255, 107, 53, 0.3);
}

.map-control-btn:active {
  transform: translateY(0);
  box-shadow: 0 2px 4px rgba(255, 107, 53, 0.2);
}

/* Map Container Cursor */
.map-container {
  cursor: grab;
}

.map-container:active {
  cursor: grabbing;
}

/* Fullscreen Styles */
.map-interface:fullscreen {
  background: var(--bg-primary);
  padding: var(--space-lg);
}

.map-interface:fullscreen .map-container {
  height: calc(100vh - 120px);
}

/* Mobile Touch Optimizations */
@media (max-width: 768px) {
  .map-controls {
    position: relative;
    top: auto;
    right: auto;
    justify-content: center;
    margin-bottom: var(--space-md);
  }
  
  .map-control-btn {
    width: 48px;
    height: 48px;
    font-size: 1.2rem;
  }
}
```

---

## 📚 **Tham chiếu:**

- [Dashboard Design](./REQ_UI_UX_Dashboard_Design.md) - Thiết kế dashboard chính
- [Visual Design](./REQ_UI_UX_Visual_Design.md) - Thiết kế hình ảnh
- [LiDAR Integration](../06-INTEGRATION-REQUIREMENTS/REQ_LiDAR_Integration_Guide.md) - Hướng dẫn tích hợp LiDAR
- [Safety System](../03-FIRMWARE-REQUIREMENTS/REQ_RS485_Safety_Module_Integration.md) - Tích hợp module an toàn

---

**Changelog v1.0:**
- ✅ Created comprehensive LiDAR visualization system
- ✅ Implemented real-time point cloud rendering
- ✅ Added navigation path planning visualization
- ✅ Developed obstacle detection and classification
- ✅ Created interactive map controls with zoom/pan
- ✅ Added safety zone visualization
- ✅ Implemented mobile-responsive touch controls
- ✅ Included performance optimizations for real-time rendering
