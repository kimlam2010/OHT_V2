# FAQ - OHT-50 Backend

## 📋 Câu hỏi thường gặp

### 🔧 **Cài đặt & Cấu hình**

**Q: Làm thế nào để cài đặt OHT-50 Backend?**
A: 
```bash
git clone https://github.com/your-org/OHT-50.git
cd OHT-50
python -m venv venv
source venv/bin/activate
pip install -r requirements.txt
python scripts/setup_database.py
uvicorn app.main:app --reload
```

**Q: Cấu hình database như thế nào?**
A: Tạo file `.env` với nội dung:
```env
DATABASE_URL=postgresql://user:password@localhost/oht50_db
SECRET_KEY=your-secret-key
DEBUG=True
```

**Q: Làm thế nào để chạy tests?**
A: 
```bash
pytest tests/
pytest --cov=app tests/
make test
```

### 🔐 **Bảo mật & Xác thực**

**Q: Làm thế nào để tạo user mới?**
A: Sử dụng API endpoint:
```bash
curl -X POST http://localhost:8000/api/v1/users/ \
  -H "Content-Type: application/json" \
  -d '{"username": "newuser", "email": "user@example.com", "password": "password123"}'
```

**Q: Làm thế nào để lấy access token?**
A: 
```bash
curl -X POST http://localhost:8000/api/v1/auth/login \
  -H "Content-Type: application/json" \
  -d '{"username": "admin", "password": "password"}'
```

**Q: Token có hiệu lực trong bao lâu?**
A: Access token có hiệu lực 30 phút, refresh token có hiệu lực 7 ngày.

### 📊 **API & Dữ liệu**

**Q: Làm thế nào để tạo mission mới?**
A: 
```bash
curl -X POST http://localhost:8000/api/v1/missions/ \
  -H "Authorization: Bearer $TOKEN" \
  -H "Content-Type: application/json" \
  -d '{"mission_type": "move_to", "parameters": {"target_position": {"x": 100, "y": 200, "z": 0}}}'
```

**Q: Làm thế nào để lấy telemetry data?**
A: 
```bash
curl -X GET http://localhost:8000/api/v1/telemetry/ \
  -H "Authorization: Bearer $TOKEN"
```

**Q: WebSocket endpoint là gì?**
A: `ws://localhost:8000/ws/telemetry` cho real-time telemetry data.

### 🚨 **Sự cố & Xử lý lỗi**

**Q: Lỗi "Database connection failed" - làm gì?**
A: 
```bash
sudo systemctl start postgresql
python scripts/setup_database.py
```

**Q: Lỗi "Invalid token" - làm gì?**
A: Token đã hết hạn, lấy token mới:
```bash
curl -X POST http://localhost:8000/api/v1/auth/refresh \
  -H "Authorization: Bearer $REFRESH_TOKEN"
```

**Q: API response chậm - làm gì?**
A: 
```bash
# Kiểm tra database performance
SELECT query, mean_time FROM pg_stat_statements ORDER BY mean_time DESC LIMIT 5;

# Kiểm tra system resources
htop
free -h
```

### 🔄 **Deployment & Production**

**Q: Làm thế nào để deploy lên production?**
A: 
```bash
docker build -t oht50-backend .
docker run -d -p 8000:8000 oht50-backend
```

**Q: Làm thế nào để backup database?**
A: 
```bash
pg_dump -h localhost -U user oht50_db > backup.sql
```

**Q: Làm thế nào để monitor application?**
A: 
```bash
# Health check
curl http://localhost:8000/health

# Metrics
curl http://localhost:8000/metrics

# Grafana dashboard
# http://localhost:3000
```

### 🛠️ **Development**

**Q: Làm thế nào để thêm API endpoint mới?**
A: 
1. Tạo schema trong `app/schemas/`
2. Tạo model trong `app/models/`
3. Tạo CRUD trong `app/crud/`
4. Tạo service trong `app/services/`
5. Tạo endpoint trong `app/api/v1/`
6. Thêm tests

**Q: Làm thế nào để debug code?**
A: 
```python
import pdb; pdb.set_trace()
# hoặc
import ipdb; ipdb.set_trace()
```

**Q: Làm thế nào để format code?**
A: 
```bash
make format
# hoặc
black app/
isort app/
```

### 📈 **Performance & Monitoring**

**Q: Làm thế nào để optimize performance?**
A: 
- Thêm database indexes
- Sử dụng caching (Redis)
- Optimize database queries
- Implement pagination
- Use connection pooling

**Q: Làm thế nào để monitor errors?**
A: 
```bash
# Check logs
tail -f logs/app.log

# Check metrics
curl http://localhost:8000/metrics | grep error

# Set up alerting
# Configure Prometheus alerts
```

**Q: Làm thế nào để scale application?**
A: 
```bash
# Horizontal scaling
docker-compose scale app=3

# Load balancing
# Use nginx or haproxy

# Database scaling
# Use read replicas
```

### 🔒 **Security**

**Q: Làm thế nào để secure API?**
A: 
- Sử dụng HTTPS
- Implement rate limiting
- Validate input data
- Use proper authentication
- Log security events

**Q: Làm thế nào để audit logs?**
A: 
```bash
# Check authentication logs
grep "authentication" logs/app.log

# Check security events
grep "security" logs/app.log

# Check failed login attempts
grep "login failed" logs/app.log
```

### 📚 **Documentation**

**Q: Tài liệu API ở đâu?**
A: 
- Swagger UI: `http://localhost:8000/docs`
- ReDoc: `http://localhost:8000/redoc`
- API docs: `docs/backend/api-specs/`

**Q: Làm thế nào để contribute?**
A: 
1. Fork repository
2. Create feature branch
3. Make changes
4. Add tests
5. Submit pull request

**Q: Làm thế nào để report bug?**
A: 
- Tạo issue trên GitHub
- Cung cấp error logs
- Mô tả steps to reproduce
- Include environment details

---

**Changelog v1.0:**
- ✅ Created comprehensive FAQ
- ✅ Added installation and configuration questions
- ✅ Included security and authentication FAQs
- ✅ Added API and data questions
- ✅ Included troubleshooting FAQs
- ✅ Added deployment and production questions
- ✅ Included development FAQs
- ✅ Added performance and monitoring questions
- ✅ Included security FAQs
- ✅ Added documentation questions
