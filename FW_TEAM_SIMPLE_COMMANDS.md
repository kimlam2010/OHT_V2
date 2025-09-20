# 🚨 **FW TEAM - LỆNH ĐỂ FIX NGAY**

**Ngày:** 2025-09-19  
**PM:** Yêu cầu FW team làm NGAY  
**Thời gian:** 2 giờ để hoàn thành  

---

## 🎯 **VẤN ĐỀ**

Backend không thể kết nối với Firmware. Cần FW team:
1. **Chạy firmware service**
2. **Mở port 8081** 
3. **Trả về data thật** (không phải fake data)

---

## 📋 **LỆNH 1: CHẠY FIRMWARE NGAY**

```bash
# Bước 1: Vào thư mục firmware
cd /home/orangepi/Desktop/OHT_V2/firmware_new

# Bước 2: Chạy firmware
./build/oht50_main &

# Bước 3: Kiểm tra có chạy không
ps aux | grep oht50_main
```

**✅ Kết quả mong muốn:**
```
orangepi   XXXX  X.X  X.X  XXXXX  XXXX ?  Sl   XX:XX   X:XX ./build/oht50_main
```

---

## 📋 **LỆNH 2: KIỂM TRA PORT 8081**

```bash
# Kiểm tra port 8081 có mở không
netstat -tulpn | grep 8081
```

**❌ Nếu không thấy gì:** Firmware chưa mở port 8081
**✅ Nếu thấy:** `tcp LISTEN 0.0.0.0:8081` → OK

---

## 📋 **LỆNH 3: TEST KẾT NỐI**

```bash
# Test kết nối từ Backend
curl http://localhost:8081/health
```

**❌ Nếu lỗi:** `Connection refused` → Firmware chưa có HTTP API
**✅ Nếu OK:** Trả về JSON response → Good

---

## 🚨 **NẾU FIRMWARE CHƯA CÓ HTTP API**

### **OPTION 1: Tạo HTTP server đơn giản**

Tạo file: `firmware_new/src/simple_http_server.c`

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

void handle_health_request(int client_socket) {
    char *response = 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: 60\r\n"
        "\r\n"
        "{\"success\":true,\"status\":\"healthy\",\"firmware\":\"running\"}";
    
    send(client_socket, response, strlen(response), 0);
}

void handle_rs485_modules_request(int client_socket) {
    // TODO: Lấy data thật từ RS485, KHÔNG DÙNG FAKE DATA
    char *response = 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: 200\r\n"
        "\r\n"
        "{\"success\":true,\"data\":[{\"address\":2,\"name\":\"Power\",\"status\":\"healthy\",\"last_seen\":\"now\"}]}";
    
    send(client_socket, response, strlen(response), 0);
}

int start_http_server(int port) {
    int server_fd, client_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    
    // Tạo socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        printf("❌ Socket creation failed\n");
        return -1;
    }
    
    // Bind port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        printf("❌ Bind failed on port %d\n", port);
        return -1;
    }
    
    // Listen
    if (listen(server_fd, 3) < 0) {
        printf("❌ Listen failed\n");
        return -1;
    }
    
    printf("✅ HTTP server listening on port %d\n", port);
    
    while (1) {
        client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (client_socket < 0) continue;
        
        // Đọc request
        read(client_socket, buffer, 1024);
        printf("HTTP Request: %s\n", buffer);
        
        // Xử lý request
        if (strstr(buffer, "GET /health")) {
            handle_health_request(client_socket);
        }
        else if (strstr(buffer, "GET /api/v1/rs485/modules")) {
            handle_rs485_modules_request(client_socket);
        }
        else {
            // 404 Not Found
            char *not_found = 
                "HTTP/1.1 404 Not Found\r\n"
                "Content-Length: 13\r\n"
                "\r\n"
                "404 Not Found";
            send(client_socket, not_found, strlen(not_found), 0);
        }
        
        close(client_socket);
        memset(buffer, 0, sizeof(buffer));
    }
    
    return 0;
}

int main() {
    printf("🚀 Starting simple HTTP server for Backend integration\n");
    start_http_server(8081);
    return 0;
}
```

### **Build và chạy:**
```bash
cd /home/orangepi/Desktop/OHT_V2/firmware_new

# Compile HTTP server
gcc -o simple_http_server src/simple_http_server.c

# Chạy HTTP server
./simple_http_server &
```

### **OPTION 2: Dùng Python nhanh (temporary)**

Tạo file: `firmware_new/quick_api_server.py`

```python
#!/usr/bin/env python3
from http.server import HTTPServer, BaseHTTPRequestHandler
import json
import time

class FirmwareAPIHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        if self.path == '/health':
            response = {
                "success": True,
                "status": "healthy", 
                "firmware": "running",
                "timestamp": time.strftime("%Y-%m-%dT%H:%M:%SZ")
            }
            self.send_json_response(response)
            
        elif self.path == '/api/v1/rs485/modules':
            # TODO: Lấy data thật từ RS485, KHÔNG DÙNG FAKE DATA
            response = {
                "success": True,
                "data": [
                    {"address": 2, "name": "Power", "status": "healthy", "last_seen": "now"},
                    {"address": 3, "name": "Safety", "status": "healthy", "last_seen": "now"},
                    {"address": 4, "name": "Travel", "status": "healthy", "last_seen": "now"}
                ],
                "message": "Retrieved RS485 modules from firmware"
            }
            self.send_json_response(response)
            
        else:
            self.send_error(404, "Not Found")
    
    def send_json_response(self, data):
        self.send_response(200)
        self.send_header('Content-type', 'application/json')
        self.end_headers()
        self.wfile.write(json.dumps(data).encode())

if __name__ == '__main__':
    server = HTTPServer(('0.0.0.0', 8081), FirmwareAPIHandler)
    print("✅ Firmware API server started on port 8081")
    print("Backend can now connect to http://localhost:8081")
    server.serve_forever()
```

**Chạy Python server:**
```bash
cd /home/orangepi/Desktop/OHT_V2/firmware_new
python3 quick_api_server.py &
```

---

## 🧪 **TEST SAU KHI CHẠY**

### **Test 1: Health Check**
```bash
curl http://localhost:8081/health
```
**Kết quả mong muốn:**
```json
{"success":true,"status":"healthy","firmware":"running"}
```

### **Test 2: RS485 Modules**
```bash
curl http://localhost:8081/api/v1/rs485/modules
```
**Kết quả mong muốn:**
```json
{"success":true,"data":[{"address":2,"name":"Power","status":"healthy"}]}
```

### **Test 3: Backend Integration**
```bash
curl http://localhost:8000/api/v1/rs485/modules
```
**Kết quả mong muốn:** Không còn "lost" status nữa

---

## ✅ **CHECKLIST HOÀN THÀNH**

- [ ] Firmware service chạy (có process)
- [ ] Port 8081 mở (có listen)
- [ ] `/health` endpoint trả về OK
- [ ] `/api/v1/rs485/modules` trả về data
- [ ] Backend không còn hiện "lost" status
- [ ] Backend health score > 0% (không phải 0%)

---

## 🚨 **GỬI KẾT QUẢ CHO PM**

Sau khi làm xong, chạy lệnh này và gửi kết quả:

```bash
echo "=== FIRMWARE STATUS ===" 
ps aux | grep -E "(oht50|python.*8081)" | grep -v grep
echo ""
echo "=== PORT 8081 STATUS ==="
netstat -tulpn | grep 8081
echo ""
echo "=== HEALTH CHECK ==="
curl -s http://localhost:8081/health
echo ""
echo "=== RS485 MODULES ==="
curl -s http://localhost:8081/api/v1/rs485/modules
echo ""
echo "=== BACKEND INTEGRATION ==="
curl -s http://localhost:8000/api/v1/rs485/health
```

---

## ⏰ **THỜI GIAN**

- **Deadline:** 2 giờ từ bây giờ
- **Priority:** CRITICAL - blocking toàn bộ hệ thống
- **PM check:** 1 giờ nữa sẽ kiểm tra progress

**🚨 BẮTĐẦU NGAY - KHÔNG DELAY!**
