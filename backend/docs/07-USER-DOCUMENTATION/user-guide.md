# 📖 OHT-50 Backend User Guide

## 📋 **Introduction**

Welcome to the OHT-50 Backend User Guide! This guide will help you understand and use the OHT-50 robot control and mapping system effectively.

### **What is OHT-50?**
OHT-50 is an advanced robot control and mapping system that provides:
- **Real-time robot control** and monitoring
- **Autonomous mapping** of indoor environments
- **Firmware-based localization** using hardware modules
- **Real-time telemetry** data processing
- **Web-based dashboard** for monitoring and control

### **Who is this guide for?**
- **Robot operators** who need to control and monitor robots
- **System administrators** who manage the backend system
- **Developers** who integrate with the API
- **Maintenance personnel** who troubleshoot issues

---

## 🚀 **Getting Started**

### **Prerequisites**
- Basic understanding of robot operations
- Access to the OHT-50 system
- Web browser (Chrome, Firefox, Safari, Edge)
- Network connection to the robot system

### **System Requirements**
- **Operating System:** Windows 10+, macOS 10.15+, or Linux
- **Browser:** Chrome 90+, Firefox 88+, Safari 14+, Edge 90+
- **Network:** Stable internet connection
- **Hardware:** Minimum 4GB RAM, 2GB free disk space

### **Accessing the System**
1. Open your web browser
2. Navigate to the OHT-50 dashboard URL
3. Log in with your credentials
4. You'll see the main dashboard

---

## 🎛️ **Dashboard Overview**

### **Main Dashboard**
The main dashboard provides an overview of the robot system:

```
┌─────────────────────────────────────────────────────────────┐
│  OHT-50 Robot Control System                    [User] [⚙️] │
├─────────────────────────────────────────────────────────────┤
│  [Dashboard] [Map] [Control] [Telemetry] [Reports] [Settings] │
├─────────────────────────────────────────────────────────────┤
│  Robot Status: [🟢 Online]  Battery: [85%]  Temp: [42°C]   │
│  Position: X:100.5 Y:200.3  Speed: 0.5 m/s  Mode: [AUTO]   │
├─────────────────────────────────────────────────────────────┤
│  [Start Mapping] [Stop Mapping] [Emergency Stop] [Home]     │
├─────────────────────────────────────────────────────────────┤
│  Real-time Map View                                        │
│  ┌─────────────────────────────────────────────────────┐   │
│  │                                                     │   │
│  │           🚗 Robot Position                         │   │
│  │                                                     │   │
│  │  [Obstacles] [Free Space] [Unknown]                │   │
│  └─────────────────────────────────────────────────────┘   │
├─────────────────────────────────────────────────────────────┤
│  Firmware Status: [RS485:🟢] [HTTP:🟢] [Modules:🟢] │
└─────────────────────────────────────────────────────────────┘
```

### **Navigation Menu**
- **Dashboard:** Main overview and status
- **Map:** Map management and visualization
- **Control:** Robot control and navigation
- **Telemetry:** Real-time data monitoring
- **Reports:** System reports and analytics
- **Settings:** System configuration

---

## 🗺️ **Map Management**

### **Creating a New Map**

#### **Step 1: Start Mapping**
1. Click **"Start Mapping"** button
2. Enter map details:
   - **Map Name:** Give your map a descriptive name
   - **Resolution:** Set grid resolution (default: 50mm)
   - **Width:** Map width in meters (default: 1000m)
   - **Height:** Map height in meters (default: 1000m)
3. Click **"Start"** to begin mapping

#### **Step 2: Mapping Process**
During mapping, the system will:
- Collect telemetry data from firmware modules via RS485
- Build an occupancy grid map
- Track robot position in real-time
- Display mapping progress

#### **Step 3: Stop Mapping**
1. Click **"Stop Mapping"** when complete
2. The system will finalize the map
3. Map will be saved and available for use

### **Loading an Existing Map**
1. Go to **Map** section
2. Select map from the list
3. Click **"Load Map"**
4. Map will be loaded and ready for navigation

### **Map Visualization**
The map shows:
- **🟢 Green areas:** Free space (safe to navigate)
- **🔴 Red areas:** Obstacles (avoid)
- **⚪ Gray areas:** Unknown space
- **🚗 Robot icon:** Current robot position
- **📍 Waypoints:** Navigation targets

---

## 🎮 **Robot Control**

### **Control Modes**

#### **Automatic Mode (AUTO)**
- Robot navigates autonomously
- Uses map for path planning
- Avoids obstacles automatically
- Best for routine operations

#### **Manual Mode (MANUAL)**
- Direct control via joystick/buttons
- Operator controls all movements
- Useful for precise positioning
- Emergency situations

#### **Semi-Automatic Mode (SEMI)**
- Combines automatic and manual control
- Robot follows waypoints automatically
- Operator can override when needed
- Good for complex maneuvers

### **Basic Controls**

#### **Movement Controls**
```
    [↑ Forward]
[← Left] [Stop] [Right →]
    [↓ Backward]
```

#### **Speed Control**
- **Slow:** 0.1 m/s (precise movements)
- **Normal:** 0.5 m/s (standard operation)
- **Fast:** 1.0 m/s (efficient travel)
- **Max:** 2.0 m/s (emergency only)

#### **Emergency Controls**
- **🛑 Emergency Stop:** Immediately stops all movement
- **⏸️ Pause:** Temporarily stops operation
- **🔄 Reset:** Resets robot to safe state
- **🏠 Home:** Returns robot to home position

### **Navigation**

#### **Setting Waypoints**
1. Click on the map where you want the robot to go
2. Click **"Add Waypoint"**
3. Repeat for multiple waypoints
4. Click **"Start Navigation"**

#### **Path Planning**
The system automatically:
- Calculates optimal path
- Avoids obstacles
- Considers robot dimensions
- Plans smooth trajectories

---

## 📡 **Sensor Management**

### **Sensor Types**

#### **RFID Sensors**
- **Purpose:** Position landmarks and navigation
- **Range:** Up to 10 meters
- **Accuracy:** ±15mm
- **Status Indicators:** 🟢 Online, 🟡 Warning, 🔴 Offline

#### **Accelerometer**
- **Purpose:** Motion detection and orientation
- **Range:** ±16g
- **Frequency:** 100Hz
- **Status Indicators:** 🟢 Online, 🟡 Warning, 🔴 Offline

#### **Proximity Sensors**
- **Purpose:** Obstacle detection
- **Range:** 0.1-5 meters
- **Accuracy:** ±5cm
- **Status Indicators:** 🟢 Online, 🟡 Warning, 🔴 Offline

#### **LiDAR Scanner**
- **Purpose:** Environment mapping
- **Range:** 0.1-50 meters
- **Resolution:** 1 degree
- **Status Indicators:** 🟢 Online, 🟡 Warning, 🔴 Offline

### **Sensor Configuration**

#### **Configuring a Sensor**
1. Go to **Sensors** section
2. Select sensor to configure
3. Set parameters:
   - **Name:** Descriptive name
   - **Description:** Purpose and location
   - **Configuration:** Technical parameters
   - **Calibration:** Offset and scaling
4. Click **"Save Configuration"**

#### **Sensor Calibration**
1. Select sensor to calibrate
2. Follow calibration procedure:
   - **RFID:** Place known tags at known positions
   - **Accelerometer:** Level the sensor
   - **Proximity:** Measure known distances
   - **LiDAR:** Scan known objects
3. Enter calibration data
4. Click **"Apply Calibration"**

### **Sensor Monitoring**

#### **Real-time Status**
- **Online/Offline:** Connection status
- **Last Reading:** Timestamp of last data
- **Error Count:** Number of errors
- **Health Score:** Overall sensor health (0-1)

#### **Data Quality**
- **Excellent:** > 0.9 (green)
- **Good:** 0.7-0.9 (yellow)
- **Fair:** 0.5-0.7 (orange)
- **Poor:** 0.3-0.5 (red)
- **Bad:** < 0.3 (dark red)

---

## 📊 **Reports and Analytics**

### **System Reports**

#### **Mapping Report**
- **Map Coverage:** Percentage of area mapped
- **Mapping Quality:** Overall map quality score
- **Total Scans:** Number of sensor scans
- **Mapping Time:** Duration of mapping session

#### **Localization Report**
- **Position Accuracy:** Average position accuracy
- **Sensor Usage:** Usage statistics by sensor type
- **Confidence Scores:** Average confidence levels
- **Error Analysis:** Common error patterns

#### **Performance Report**
- **Response Times:** API response times
- **System Resources:** CPU, memory, disk usage
- **Uptime:** System availability
- **Error Rates:** Error frequency and types

### **Exporting Reports**
1. Go to **Reports** section
2. Select report type
3. Choose date range
4. Click **"Generate Report"**
5. Download as PDF or CSV

---

## ⚙️ **System Settings**

### **General Settings**

#### **System Configuration**
- **Language:** Select interface language
- **Timezone:** Set system timezone
- **Units:** Metric or Imperial units
- **Theme:** Light or dark theme

#### **Performance Settings**
- **Update Frequency:** Real-time update rates
- **Cache Size:** Data cache limits
- **Log Level:** Logging verbosity
- **Auto-save:** Automatic data saving

### **Security Settings**

#### **User Management**
- **Add User:** Create new user accounts
- **Edit User:** Modify user permissions
- **Delete User:** Remove user accounts
- **Password Policy:** Set password requirements

#### **Access Control**
- **Role-based Access:** Assign user roles
- **Permission Levels:** Set access levels
- **Session Timeout:** Automatic logout time
- **IP Restrictions:** Limit access by IP

### **Network Settings**

#### **Connection Settings**
- **Firmware URL:** Firmware server address
- **Timeout Settings:** Connection timeouts
- **Retry Logic:** Automatic retry settings
- **Proxy Settings:** Network proxy configuration

---

## 🚨 **Troubleshooting**

### **Common Issues**

#### **Robot Not Responding**
1. **Check Connection:** Verify network connection
2. **Check Status:** Look at robot status indicators
3. **Restart System:** Try restarting the robot
4. **Check Logs:** Review system logs for errors

#### **Mapping Issues**
1. **Check Sensors:** Verify all sensors are online
2. **Check Environment:** Ensure adequate lighting
3. **Check Calibration:** Verify sensor calibration
4. **Restart Mapping:** Stop and restart mapping

#### **Sensor Problems**
1. **Check Connections:** Verify physical connections
2. **Check Power:** Ensure sensors have power
3. **Check Configuration:** Verify sensor settings
4. **Check Calibration:** Recalibrate if needed

#### **Performance Issues**
1. **Check Resources:** Monitor CPU and memory usage
2. **Check Network:** Verify network performance
3. **Check Database:** Monitor database performance
4. **Restart Services:** Restart backend services

### **Error Messages**

#### **Connection Errors**
- **"Connection Failed":** Check network connection
- **"Timeout Error":** Increase timeout settings
- **"Authentication Failed":** Check login credentials
- **"Permission Denied":** Check user permissions

#### **Sensor Errors**
- **"Sensor Offline":** Check sensor connection
- **"Data Quality Poor":** Check sensor calibration
- **"Configuration Error":** Verify sensor settings
- **"Calibration Required":** Perform sensor calibration

#### **System Errors**
- **"Database Error":** Check database connection
- **"Memory Error":** Restart system
- **"Disk Full":** Free up disk space
- **"Service Unavailable":** Restart services

### **Getting Help**

#### **Self-Help Resources**
- **User Guide:** This document
- **FAQ:** Frequently asked questions
- **Video Tutorials:** Step-by-step videos
- **Knowledge Base:** Searchable help articles

#### **Contact Support**
- **Email:** support@oht50.com
- **Phone:** +1-800-OHT-HELP
- **Chat:** Live chat support
- **Ticket System:** Submit support tickets

---

## 🔒 **Safety Guidelines**

### **General Safety**
- **Never operate robot in unsafe conditions**
- **Always maintain safe distance from robot**
- **Follow all safety procedures and protocols**
- **Report any safety concerns immediately**

### **Emergency Procedures**
- **Emergency Stop:** Press emergency stop button immediately
- **Power Off:** Turn off robot power if needed
- **Evacuate Area:** Clear area of personnel
- **Contact Emergency Services:** Call 911 if necessary

### **Maintenance Safety**
- **Power Off:** Always power off before maintenance
- **Lock Out/Tag Out:** Follow LOTO procedures
- **Use PPE:** Wear appropriate personal protective equipment
- **Follow Procedures:** Follow all maintenance procedures

---

## 📚 **Best Practices**

### **Mapping Best Practices**
- **Plan Your Route:** Plan mapping route in advance
- **Check Environment:** Ensure environment is suitable
- **Monitor Progress:** Watch mapping progress closely
- **Verify Quality:** Check map quality before use

### **Operation Best Practices**
- **Start Slow:** Begin with slow speeds
- **Monitor Sensors:** Watch sensor status continuously
- **Plan Paths:** Plan navigation paths carefully
- **Test First:** Test operations in safe areas

### **Maintenance Best Practices**
- **Regular Checks:** Perform regular system checks
- **Keep Clean:** Keep sensors and robot clean
- **Update Software:** Keep software up to date
- **Document Issues:** Document any issues found

---

## 📞 **Support and Resources**

### **Documentation**
- **API Documentation:** `/docs` (Swagger UI)
- **Technical Documentation:** `/docs/technical`
- **User Guide:** This document
- **Video Tutorials:** Available online

### **Training**
- **Online Training:** Self-paced online courses
- **In-person Training:** On-site training sessions
- **Certification:** Operator certification programs
- **Workshops:** Regular training workshops

### **Community**
- **User Forum:** Connect with other users
- **Knowledge Base:** Share knowledge and tips
- **User Groups:** Local user group meetings
- **Events:** Conferences and trade shows

### **Contact Information**
- **Technical Support:** support@oht50.com
- **Sales:** sales@oht50.com
- **Training:** training@oht50.com
- **General:** info@oht50.com

---

## 📝 **Appendices**

### **Appendix A: Keyboard Shortcuts**
- **Ctrl+S:** Save current work
- **Ctrl+Z:** Undo last action
- **Ctrl+R:** Refresh data
- **F5:** Start/stop mapping
- **F6:** Emergency stop
- **F7:** Home position
- **F8:** Toggle manual mode

### **Appendix B: Status Codes**
- **🟢 Green:** Normal operation
- **🟡 Yellow:** Warning condition
- **🔴 Red:** Error condition
- **⚪ Gray:** Unknown/offline
- **🔵 Blue:** Information

### **Appendix C: Units and Measurements**
- **Distance:** Meters (m)
- **Speed:** Meters per second (m/s)
- **Angle:** Radians (rad)
- **Temperature:** Celsius (°C)
- **Battery:** Percentage (%)
- **Time:** ISO 8601 format

---

**Last Updated:** 2025-01-28  
**Version:** 1.0.0  
**For OHT-50 Backend System**
