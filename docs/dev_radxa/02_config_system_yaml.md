# 02) Cấu hình hệ thống (`config/system.yaml`) - Master Module

## Mục tiêu
- Chuẩn hoá cấu hình cho Master Module OHT-50
- Hỗ trợ RS485, Ethernet, WiFi, LED status, E-Stop, relay
- Cấu hình network và communication protocols

## Cấu trúc đề xuất
```yaml
# Master Module Configuration
master_module:
  name: "OHT-50-Master"
  version: "3.0.0"
  platform: "Orange Pi 5B (RK3588)"
  
# Communication Interfaces
communication:
  rs485:
    port: /dev/ttyOHT485
    baud: 115200
    parity: none
    stop_bits: 1
    timeout_ms: 1000
    retry_count: 3
    termination: 120  # ohms
    
  ethernet:
    interface: eth0
    dhcp: true
    static_ip: "192.168.1.100"  # fallback
    gateway: "192.168.1.1"
    dns: ["8.8.8.8", "8.8.4.4"]
    
  wifi:
    interface: wlan0
    ssid: "OHT50-Network"
    password: "secure_password"
    security: "WPA3"
    backup_only: true  # Use as backup to Ethernet
    
  usb_debug:
    port: /dev/ttyACM0
    baud: 115200
    enabled: true

# LED Status Configuration
leds:
  power:
    pin: 32  # GPIO1_A0
    color: "green"
    function: "24V power status"
    default_state: "on"
    
  system:
    pin: 33  # GPIO1_A1
    color: "blue"
    function: "system status"
    default_state: "blink_slow"
    
  communication:
    pin: 34  # GPIO1_A2
    color: "yellow"
    function: "RS485 traffic"
    default_state: "off"
    
  network:
    pin: 35  # GPIO1_A3
    color: "green"
    function: "LAN/WiFi status"
    default_state: "off"
    
  error:
    pin: 36  # GPIO1_A4
    color: "red"
    function: "error status"
    default_state: "off"

# Safety System Configuration
safety:
  estop:
    channel1_pin: 48  # GPIO1_B0
    channel2_pin: 49  # GPIO1_B1
    voltage: 24  # V DC
    response_time_ms: 100
    safety_level: "SIL2"
    dual_channel: true
    
  relay:
    output_pin: 51  # GPIO1_D3
    voltage: 24  # V DC
    current_max: 2  # A
    type: "solid_state"
    default_state: "off"

# Network Services
network:
  center:
    mode: "ws"  # ws|mqtt|http
    url: "ws://center.local:8080/robot"
    auth_token: "oht50_master_token"
    heartbeat_s: 2
    reconnect_timeout_s: 30
    
  web_interface:
    port: 8080
    ssl: false
    auth_required: true
    max_connections: 10
    
  ota:
    enabled: true
    server_url: "http://ota.oht50.local"
    check_interval_h: 24
    auto_update: false

# Module Configuration
modules:
  auto_discovery: true
  discovery_timeout_ms: 5000
  max_modules: 16
  
  # Pre-configured modules
  safety: 0x10
  dio_upper: 0x11
  aio_upper: 0x12
  dcm1: 0x13
  stm1: 0x14
  dcm2: 0x15
  stm2: 0x16
  dio_cargo: 0x17
  aio_cargo: 0x18
  dcm3: 0x19
  location: 0x1A

# System Configuration
system:
  hostname: "oht50-master"
  timezone: "Asia/Ho_Chi_Minh"
  ntp_servers: ["pool.ntp.org", "time.google.com"]
  
  logging:
    level: "info"  # debug|info|warning|error
    file: "/var/log/oht50/master.log"
    max_size_mb: 100
    max_files: 10
    
  monitoring:
    cpu_threshold: 80  # %
    memory_threshold: 85  # %
    temperature_threshold: 70  # °C
    disk_threshold: 90  # %

# Telemetry Configuration
telemetry:
  rate_hz: 20
  fields: [system_status, led_status, estop_status, relay_status, network_status, module_status]
  
  storage:
    local_db: "/var/lib/oht50/telemetry.db"
    retention_days: 30
    compression: true

# Security Configuration
security:
  ssl_cert: "/etc/ssl/certs/oht50-master.crt"
  ssl_key: "/etc/ssl/private/oht50-master.key"
  api_key_required: true
  max_login_attempts: 3
  session_timeout_min: 30
```

## Ghi chú
- Địa chỉ/module khớp `module_map.md`
- LED configuration theo master module specification
- E-Stop dual-channel safety theo chuẩn SIL2
- Network configuration hỗ trợ failover Ethernet → WiFi
- Security configuration cho production deployment
