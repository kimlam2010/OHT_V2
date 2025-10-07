# 🔐 Security Policy - OHT-50 Firmware

**Version:** 1.0.1  
**Last Updated:** 2025-10-07

---

## 📋 **SUPPORTED VERSIONS**

| Version | Supported          |
| ------- | ------------------ |
| 1.0.x   | ✅ Yes             |
| < 1.0   | ❌ No              |

---

## 🛡️ **SECURITY FEATURES**

### **Current Security**

✅ **Basic Authentication**: API authentication support  
✅ **Input Validation**: Request parameter validation  
✅ **Audit Logging**: Security event logging  
✅ **Auto Cleanup**: Process và port cleanup  
⚠️ **HTTPS Support**: Optional (not enabled by default)  
⚠️ **Rate Limiting**: Basic (can be enhanced)

### **Planned Security (Future)**

- [ ] OAuth2/JWT authentication
- [ ] Role-based access control (RBAC)
- [ ] API rate limiting (advanced)
- [ ] Intrusion detection
- [ ] Encrypted communication (TLS)

---

## 🚨 **REPORTING VULNERABILITIES**

### **How to Report**

🔒 **DO NOT** create public GitHub issues for security vulnerabilities!

**Report privately:**

1. **Email:** security@oht50.com
2. **PGP Key:** Available at https://oht50.com/pgp
3. **Include:**
   - Description of vulnerability
   - Steps to reproduce
   - Potential impact
   - Suggested fix (if any)

### **Response Timeline**

- **24 hours:** Initial acknowledgment
- **7 days:** Assessment và triage
- **30 days:** Fix released (if confirmed)
- **60 days:** Public disclosure (coordinated)

---

## ⚠️ **KNOWN SECURITY CONSIDERATIONS**

### **1. Default Configuration**

❌ **Not Production-Ready Out-of-the-Box**

```yaml
# Default config (INSECURE):
auth:
  enabled: false           # ❌ Auth disabled
  
network:
  https: false             # ❌ HTTP only
  rate_limit: basic        # ⚠️ Basic rate limiting
```

**Recommended Production Config:**

```yaml
auth:
  enabled: true
  method: jwt
  
network:
  https: true
  rate_limit: advanced
  
logging:
  audit_enabled: true
```

### **2. Network Exposure**

⚠️ **Default:** API exposed on `0.0.0.0:8080` (all interfaces)

**Recommendation:** Bind to specific interface

```c
// Edit http_server.c
#define HTTP_BIND_ADDRESS "127.0.0.1"  // Localhost only
```

### **3. Hardware Access**

⚠️ **GPIO/UART Access:** Requires root or dialout group

**Recommendation:** Use dedicated service user

```bash
sudo useradd -r -s /bin/false oht50service
sudo usermod -a -G dialout,gpio oht50service
```

---

## 🔒 **SECURITY BEST PRACTICES**

### **Deployment Security**

```bash
# 1. Use non-root user
sudo useradd -r oht50
sudo chown oht50:oht50 /usr/local/bin/oht50_main

# 2. Enable firewall
sudo ufw enable
sudo ufw allow 8080/tcp  # Only if needed externally

# 3. Use systemd security features
[Service]
User=oht50
Group=oht50
PrivateTmp=yes
ProtectSystem=strict
ProtectHome=yes
NoNewPrivileges=yes
```

### **Network Security**

```bash
# Use reverse proxy (nginx/apache)
# Example nginx config:
server {
    listen 443 ssl;
    server_name oht50.example.com;
    
    ssl_certificate /path/to/cert.pem;
    ssl_certificate_key /path/to/key.pem;
    
    location / {
        proxy_pass http://localhost:8080;
        proxy_set_header X-Real-IP $remote_addr;
    }
}
```

---

## 📊 **SECURITY AUDIT**

### **Self-Assessment Checklist**

```bash
# Authentication
[ ] API authentication enabled
[ ] Strong passwords enforced
[ ] Session timeout configured

# Network
[ ] HTTPS enabled (production)
[ ] Firewall configured
[ ] Rate limiting active

# Access Control
[ ] Least privilege principle
[ ] No unnecessary permissions
[ ] Audit logging enabled

# Code
[ ] Input validation implemented
[ ] No hardcoded secrets
[ ] Dependencies updated
[ ] Security patches applied
```

---

## 🏆 **SECURITY HALL OF FAME**

Contributors who report valid security vulnerabilities:

- (No reports yet - be the first!)

---

## 📞 **CONTACT**

**Security Team:**
- 📧 **Email:** security@oht50.com
- 🔐 **PGP:** https://oht50.com/pgp
- 💬 **Private Slack:** Request invite via email

---

**Version:** 1.0.1  
**Maintained By:** OHT-50 Security Team

