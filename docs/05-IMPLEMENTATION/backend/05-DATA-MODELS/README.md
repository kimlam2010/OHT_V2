# 📁 05-DATA-MODELS - Data Models & Database

**Mục đích:** Data models, database schema, và data validation  
**Đối tượng:** Backend Developers, Database Administrators  
**Cập nhật:** Khi có thay đổi data models

---

## 📋 **DANH SÁCH TÀI LIỆU**

### **📊 Core Data Models:**
- **DATA_MODELS.md** - Tổng quan tất cả data models
- **DATABASE_SCHEMA.md** - Database schema và relationships
- **VALIDATION_RULES.md** - Data validation rules và constraints

### **🔧 Pydantic Models:**
- **MISSION_MODELS.md** - Mission-related data models
- **STATE_MODELS.md** - State management data models
- **SAFETY_MODELS.md** - Safety-related data models
- **TELEMETRY_MODELS.md** - Telemetry data models
- **CONFIG_MODELS.md** - Configuration data models

### **🗄️ Database Management:**
- **MIGRATION_GUIDE.md** - Database migration procedures
- **BACKUP_RESTORE.md** - Database backup và restore procedures
- **PERFORMANCE_OPTIMIZATION.md** - Database performance optimization

---

## 🎯 **HƯỚNG DẪN ĐỌC**

### **Cho Backend Developer:**
1. **DATA_MODELS.md** - Hiểu tổng quan data models
2. **Pydantic Models files** - Chi tiết từng model
3. **VALIDATION_RULES.md** - Nắm validation rules
4. **MIGRATION_GUIDE.md** - Hiểu migration process

### **Cho Database Administrator:**
1. **DATABASE_SCHEMA.md** - Review database design
2. **PERFORMANCE_OPTIMIZATION.md** - Optimize database performance
3. **BACKUP_RESTORE.md** - Setup backup procedures

---

## 🔄 **QUY TRÌNH CẬP NHẬT**

### **Khi thêm model mới:**
1. Tạo file `{MODEL_NAME}_MODELS.md`
2. Cập nhật **DATA_MODELS.md**
3. Cập nhật **DATABASE_SCHEMA.md**
4. Tạo migration script

### **Khi thay đổi model:**
1. Cập nhật model file tương ứng
2. Tạo migration script
3. Cập nhật validation rules
4. Test migration

---

## 📊 **TRẠNG THÁI DATA MODELS**

| Model Category | Trạng thái | File | Ghi chú |
|----------------|------------|------|---------|
| Mission Models | ✅ Complete | MISSION_MODELS.md | Core business models |
| State Models | ✅ Complete | STATE_MODELS.md | State machine models |
| Safety Models | ✅ Complete | SAFETY_MODELS.md | Safety critical models |
| Telemetry Models | ✅ Complete | TELEMETRY_MODELS.md | Real-time data models |
| Config Models | ✅ Complete | CONFIG_MODELS.md | Configuration models |

---

## 🗄️ **DATABASE SCHEMA OVERVIEW**

### **Core Tables:**
- **missions** - Mission data và lifecycle
- **states** - System state history
- **telemetry** - Real-time sensor data
- **configs** - System configuration
- **logs** - System logs và events

### **Relationships:**
- **missions** ↔ **states** (One-to-Many)
- **missions** ↔ **telemetry** (One-to-Many)
- **configs** ↔ **states** (One-to-Many)

---

## 🔗 **LIÊN KẾT QUAN TRỌNG**

- **02-ARCHITECTURE/** - System architecture
- **03-API-SPECIFICATIONS/** - API data contracts
- **04-IMPLEMENTATION/** - Implementation details
- **06-DEPLOYMENT/** - Database deployment

---

**📝 Lưu ý:** Tất cả data models phải tuân thủ validation rules và được test đầy đủ trước khi deploy.
