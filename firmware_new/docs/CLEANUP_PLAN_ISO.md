# 📋 FIRMWARE DOCS CLEANUP PLAN - ISO STANDARDIZATION

**Ngày thực hiện:** 2025-10-07  
**Mục tiêu:** Dọn dẹp và chuẩn hóa tài liệu firmware theo ISO 9001:2015  
**Team:** PM + QA

---

## 🎯 **PHÂN TÍCH HIỆN TRẠNG**

### **✅ CẤU TRÚC ISO ĐÃ TẠO (GIỮ LẠI):**
```
01-QUALITY_POLICY/           # ✅ GIỮ - ISO compliant
├── QP-001_Security_Policy.md
├── QP-002_License.md
└── README.md

02-QUALITY_MANUAL/           # ✅ GIỮ - ISO compliant
├── QM-001_Project_Overview.md
├── QM-002_Code_Structure.md
└── README.md

03-PROCEDURES/               # ✅ GIỮ - ISO compliant
├── PR-001_Build_Procedure.md
├── PR-002_Contributing_Procedure.md
├── PR-003_Code_Quality_Procedure.md
└── README.md

04-WORK_INSTRUCTIONS/        # ✅ GIỮ - ISO compliant
├── WI-001_Installation_Guide.md
├── WI-002_Usage_Guide.md
├── WI-003_Development_Guide.md
├── WI-004_Troubleshooting_Guide.md
└── README.md

05-FORMS_RECORDS/            # ✅ GIỮ - ISO compliant
├── REC-001_Change_Log.md
├── REC-002_Migration_Log_v1.0.1.md
├── REC-003_Domain_Driven_Migration_Summary.md
├── REC-004_Cleanup_Summary.md
├── REC-005_Final_Cleanup_Report.md
├── REC-006_Documentation_Update_Summary.md
└── README.md

06-REFERENCE/                # ✅ GIỮ - ISO compliant
├── REF-001_API_Reference.md
├── REF-002_Architecture_Quick_Reference.md
├── REF-003_Documentation_Index.md
├── REF-004_Documentation_Navigation.md
└── README.md
```

---

## ❌ **CẤU TRÚC CŨ (CẦN DỌN DẸP):**

### **1. FOLDERS RỖNG (XÓA NGAY):**
```
❌ api/              - EMPTY, XÓA
❌ architecture/     - EMPTY, XÓA
❌ safety/           - EMPTY, XÓA
❌ user_guide/       - EMPTY, XÓA
```

### **2. CẤU TRÚC CŨ (CẦN DI CHUYỂN/GỘP):**

#### **00-OVERVIEW/ (ĐÁNH GIÁ)**
```
⚠️ 00-OVERVIEW/README.md
   → NỘI DUNG: Basic overview, ISO/IEC 12207 compliant
   → HÀNH ĐỘNG: GỘP vào 02-QUALITY_MANUAL/QM-001_Project_Overview.md
   → SAU ĐÓ: XÓA folder 00-OVERVIEW/
```

#### **01-ARCHITECTURE/ (ĐÁNH GIÁ)**
```
⚠️ 01-ARCHITECTURE/02-HAL/
   ├── CONTROL_LOOP_DEV_GUIDE.md           → DI CHUYỂN sang 06-REFERENCE/
   ├── LIDAR_HAL_API_REFERENCE.md          → DI CHUYỂN sang 06-REFERENCE/
   ├── LIDAR_HAL_IMPLEMENTATION_SUMMARY.md → DI CHUYỂN sang 05-FORMS_RECORDS/
   ├── README.md                            → GỘP vào 06-REFERENCE/
   └── 03-lidar/LIDAR_DEBUG_GUIDE.md       → DI CHUYỂN sang 04-WORK_INSTRUCTIONS/
```

#### **02-IMPLEMENTATION/ (ĐÁNH GIÁ)**
```
⚠️ 02-IMPLEMENTATION/
   ├── API_DOCUMENTATION.md (3557 lines!)   → TÁCH & DI CHUYỂN
   │   → Quá lớn! Cần tách thành:
   │      - API Reference → 06-REFERENCE/REF-001_API_Reference.md (UPDATE)
   │      - Backend Integration → 04-WORK_INSTRUCTIONS/WI-005_Backend_Integration.md (MỚI)
   │      - Frontend Integration → 04-WORK_INSTRUCTIONS/WI-006_Frontend_Integration.md (MỚI)
   │
   ├── 01-QMS/                              → GIỮ (quan trọng)
   │   ├── 01-policies/quality_policy.md    → DI CHUYỂN sang 01-QUALITY_POLICY/
   │   └── README.md                        → UPDATE references
   │
   ├── 03-REQUIREMENTS/                     → GIỮ (specs quan trọng)
   │   ├── 03-FIRMWARE-REQUIREMENTS/        → GIỮ
   │   │   ├── REQ_*.md (8 files)           → GIỮ (technical specs)
   │   │   └── 04-IMPLEMENTED-MODULES/      → GIỮ
   │   └── README.md                        → GIỮ
   │
   ├── 04-SLC/                              → GIỮ (planning docs)
   │   ├── 01-planning/*.md (7 files)       → GIỮ (implementation plans)
   │   ├── CHANGELOG.md                     → GỘP vào 05-FORMS_RECORDS/REC-001
   │   ├── OHT50_FIRMWARE_IMPLEMENTATION_PLAN.md → GIỮ
   │   └── README.md                        → UPDATE
   │
   ├── 05-SAFETY/                           → ĐÁNH GIÁ
   │   ├── FW_APP_SAFETY_ARCHITECTURE.md    → DI CHUYỂN sang 06-REFERENCE/
   │   └── README.md                        → UPDATE
   │
   └── 06-QUALITY/                          → GIỮ
       └── README.md                        → UPDATE
```

#### **03-DEPLOYMENT/ (ĐÁNH GIÁ)**
```
⚠️ 03-DEPLOYMENT/
   ├── plant/NETWORK_MANAGEMENT_PLANT_IMPLEMENTATION.md
   └── technical/NETWORK_MANAGEMENT_TECHNICAL.md
   
   → NỘI DUNG: Network management implementation
   → HÀNH ĐỘNG: DI CHUYỂN sang 04-WORK_INSTRUCTIONS/ hoặc 03-PROCEDURES/
   → SAU ĐÓ: XÓA folder 03-DEPLOYMENT/
```

#### **04-OPERATIONS/ (ĐÁNH GIÁ)**
```
⚠️ 04-OPERATIONS/
   └── tracking/NETWORK_MANAGEMENT_TRACKING_PLANT.md
   
   → NỘI DUNG: Tracking documents
   → HÀNH ĐỘNG: DI CHUYỂN sang 05-FORMS_RECORDS/
   → SAU ĐÓ: XÓA folder 04-OPERATIONS/
```

#### **ROOT DOCS FILES (ĐÁNH GIÁ)**
```
⚠️ CTO_REFACTOR_ORDER.md         → DI CHUYỂN sang 05-FORMS_RECORDS/ (record)
⚠️ FW_APP_IMPLEMENTATION_PLAN.md → DI CHUYỂN sang 02-IMPLEMENTATION/04-SLC/01-planning/
⚠️ FW_APP_SAFETY_ARCHITECTURE.md → ĐÃ CÓ trong 02-IMPLEMENTATION/05-SAFETY/ (TRÙNG!)
⚠️ README.md                      → UPDATE với ISO structure
```

---

## 🔧 **HÀNH ĐỘNG CHI TIẾT**

### **PHASE 1: XÓA FOLDERS RỖNG** ⏱️ 5 phút
```bash
❌ rm -rf api/
❌ rm -rf architecture/
❌ rm -rf safety/
❌ rm -rf user_guide/
```

### **PHASE 2: DI CHUYỂN HAL DOCS** ⏱️ 10 phút
```bash
# Di chuyển HAL docs sang 06-REFERENCE/
✅ mv 01-ARCHITECTURE/02-HAL/CONTROL_LOOP_DEV_GUIDE.md → 06-REFERENCE/REF-005_Control_Loop_Guide.md
✅ mv 01-ARCHITECTURE/02-HAL/LIDAR_HAL_API_REFERENCE.md → 06-REFERENCE/REF-006_LiDAR_HAL_API.md
✅ mv 01-ARCHITECTURE/02-HAL/03-lidar/LIDAR_DEBUG_GUIDE.md → 04-WORK_INSTRUCTIONS/WI-005_LiDAR_Debug_Guide.md

# Di chuyển implementation summary
✅ mv 01-ARCHITECTURE/02-HAL/LIDAR_HAL_IMPLEMENTATION_SUMMARY.md → 05-FORMS_RECORDS/REC-007_LiDAR_HAL_Implementation.md

# Gộp HAL README
✅ GỘP 01-ARCHITECTURE/02-HAL/README.md → 06-REFERENCE/README.md (append section)

# Xóa folder cũ
❌ rm -rf 01-ARCHITECTURE/
```

### **PHASE 3: TÁCH API_DOCUMENTATION.md (3557 LINES!)** ⏱️ 30 phút
```bash
# File này quá lớn - TÁCH thành 3 parts:

1️⃣ API Reference (endpoints only)
   → UPDATE 06-REFERENCE/REF-001_API_Reference.md
   
2️⃣ Backend Integration Guide
   → TẠO MỚI 04-WORK_INSTRUCTIONS/WI-005_Backend_Integration.md
   
3️⃣ Frontend Integration Guide  
   → TẠO MỚI 04-WORK_INSTRUCTIONS/WI-006_Frontend_Integration.md

# Sau khi tách xong
❌ XÓA 02-IMPLEMENTATION/API_DOCUMENTATION.md
```

### **PHASE 4: DI CHUYỂN QMS POLICY** ⏱️ 5 phút
```bash
✅ mv 02-IMPLEMENTATION/01-QMS/01-policies/quality_policy.md → 01-QUALITY_POLICY/QP-003_Quality_Policy.md
✅ UPDATE 02-IMPLEMENTATION/01-QMS/README.md (update references)
```

### **PHASE 5: DI CHUYỂN SAFETY DOCS** ⏱️ 10 phút
```bash
# Safety architecture
✅ mv 02-IMPLEMENTATION/05-SAFETY/FW_APP_SAFETY_ARCHITECTURE.md → 06-REFERENCE/REF-007_Safety_Architecture.md

# Root safety file (DUPLICATE - xóa)
❌ rm FW_APP_SAFETY_ARCHITECTURE.md

# Update README
✅ UPDATE 02-IMPLEMENTATION/05-SAFETY/README.md
```

### **PHASE 6: DI CHUYỂN DEPLOYMENT & OPERATIONS** ⏱️ 10 phút
```bash
# Deployment docs → Work Instructions
✅ mv 03-DEPLOYMENT/plant/NETWORK_MANAGEMENT_PLANT_IMPLEMENTATION.md → 04-WORK_INSTRUCTIONS/WI-007_Network_Deployment.md
✅ mv 03-DEPLOYMENT/technical/NETWORK_MANAGEMENT_TECHNICAL.md → 06-REFERENCE/REF-008_Network_Technical.md

# Operations docs → Forms/Records
✅ mv 04-OPERATIONS/tracking/NETWORK_MANAGEMENT_TRACKING_PLANT.md → 05-FORMS_RECORDS/REC-007_Network_Tracking.md

# Xóa folders cũ
❌ rm -rf 03-DEPLOYMENT/
❌ rm -rf 04-OPERATIONS/
```

### **PHASE 7: DI CHUYỂN ROOT FILES** ⏱️ 5 phút
```bash
✅ mv CTO_REFACTOR_ORDER.md → 05-FORMS_RECORDS/REC-008_CTO_Refactor_Order.md
✅ mv FW_APP_IMPLEMENTATION_PLAN.md → 02-IMPLEMENTATION/04-SLC/01-planning/
❌ rm FW_APP_SAFETY_ARCHITECTURE.md (DUPLICATE - đã có trong 02-IMPLEMENTATION/05-SAFETY/)
```

### **PHASE 8: GỘP OVERVIEW** ⏱️ 10 phút
```bash
✅ GỘP 00-OVERVIEW/README.md → 02-QUALITY_MANUAL/QM-001_Project_Overview.md
❌ rm -rf 00-OVERVIEW/
```

### **PHASE 9: GỘP CHANGELOGS** ⏱️ 10 phút
```bash
✅ GỘP 02-IMPLEMENTATION/04-SLC/CHANGELOG.md → 05-FORMS_RECORDS/REC-001_Change_Log.md
```

### **PHASE 10: UPDATE README.md & INDEXES** ⏱️ 15 phút
```bash
✅ UPDATE docs/README.md (main index)
✅ UPDATE all category README.md files
✅ UPDATE cross-references
```

---

## 📊 **SUMMARY**

### **Actions:**
| Action | Count | Time |
|--------|-------|------|
| ❌ **XÓA folders rỗng** | 4 folders | 5 min |
| ❌ **XÓA files duplicate** | 2 files | 2 min |
| ✅ **DI CHUYỂN files** | 12 files | 40 min |
| ✅ **GỘP files** | 3 files | 20 min |
| ✅ **TẠO MỚI files** | 2 files | 20 min |
| ✅ **UPDATE files** | 8 files | 15 min |
| **TOTAL** | **31 actions** | **~102 min** |

### **Before Cleanup:**
- **Folders:** 13 folders (4 rỗng, 9 có nội dung)
- **Files:** ~80 .md files
- **Structure:** Mixed (cũ + ISO)

### **After Cleanup:**
- **Folders:** 6 ISO folders + 2 implementation folders
- **Files:** ~70 .md files (organized)
- **Structure:** 100% ISO 9001:2015 compliant

---

## ✅ **EXPECTED RESULT**

```
firmware_new/docs/
├── README.md                        # ✅ Master index (updated)
│
├── 📜 01-QUALITY_POLICY/           # ✅ ISO Level 1
│   ├── QP-001_Security_Policy.md
│   ├── QP-002_License.md
│   ├── QP-003_Quality_Policy.md    # ← MỚI (từ QMS)
│   └── README.md
│
├── 📘 02-QUALITY_MANUAL/           # ✅ ISO Level 2
│   ├── QM-001_Project_Overview.md  # ← UPDATED (gộp OVERVIEW)
│   ├── QM-002_Code_Structure.md
│   └── README.md
│
├── 🔧 03-PROCEDURES/               # ✅ ISO Level 3
│   ├── PR-001_Build_Procedure.md
│   ├── PR-002_Contributing_Procedure.md
│   ├── PR-003_Code_Quality_Procedure.md
│   └── README.md
│
├── 📚 04-WORK_INSTRUCTIONS/        # ✅ ISO Level 4
│   ├── WI-001_Installation_Guide.md
│   ├── WI-002_Usage_Guide.md
│   ├── WI-003_Development_Guide.md
│   ├── WI-004_Troubleshooting_Guide.md
│   ├── WI-005_LiDAR_Debug_Guide.md        # ← MỚI
│   ├── WI-006_Backend_Integration.md      # ← MỚI (từ API_DOCUMENTATION)
│   ├── WI-007_Frontend_Integration.md     # ← MỚI (từ API_DOCUMENTATION)
│   ├── WI-008_Network_Deployment.md       # ← MỚI (từ DEPLOYMENT)
│   └── README.md                          # ← UPDATED
│
├── 📋 05-FORMS_RECORDS/            # ✅ Records & History
│   ├── REC-001_Change_Log.md              # ← UPDATED (gộp SLC CHANGELOG)
│   ├── REC-002_Migration_Log_v1.0.1.md
│   ├── REC-003_Domain_Driven_Migration_Summary.md
│   ├── REC-004_Cleanup_Summary.md
│   ├── REC-005_Final_Cleanup_Report.md
│   ├── REC-006_Documentation_Update_Summary.md
│   ├── REC-007_LiDAR_HAL_Implementation.md  # ← MỚI
│   ├── REC-008_Network_Tracking.md          # ← MỚI
│   ├── REC-009_CTO_Refactor_Order.md        # ← MỚI
│   └── README.md                            # ← UPDATED
│
├── 📚 06-REFERENCE/                # ✅ Technical Reference
│   ├── REF-001_API_Reference.md           # ← UPDATED (API core)
│   ├── REF-002_Architecture_Quick_Reference.md
│   ├── REF-003_Documentation_Index.md
│   ├── REF-004_Documentation_Navigation.md
│   ├── REF-005_Control_Loop_Guide.md      # ← MỚI
│   ├── REF-006_LiDAR_HAL_API.md           # ← MỚI
│   ├── REF-007_Safety_Architecture.md     # ← MỚI
│   ├── REF-008_Network_Technical.md       # ← MỚI
│   └── README.md                          # ← UPDATED
│
└── 🏗️ 02-IMPLEMENTATION/          # ✅ GIỮ (technical specs)
    ├── 01-QMS/                    # Quality Management
    ├── 03-REQUIREMENTS/           # Requirements specs
    ├── 04-SLC/                    # Implementation planning
    ├── 05-SAFETY/                 # Safety systems
    └── 06-QUALITY/                # Quality assurance
```

---

## 🚨 **RISKS & MITIGATION**

### **Risks:**
1. **Broken Links:** Internal references có thể bị broken
2. **Lost Content:** Có thể mất content khi di chuyển
3. **Build Impact:** Có thể ảnh hưởng build nếu có references

### **Mitigation:**
1. ✅ **Backup trước khi dọn:** `tar -czf docs_backup_$(date +%Y%m%d).tar.gz docs/`
2. ✅ **Test links:** Kiểm tra tất cả internal links sau khi di chuyển
3. ✅ **Verify content:** Đảm bảo không mất nội dung quan trọng
4. ✅ **Update references:** Cập nhật tất cả references trong code

---

## ⏱️ **TIMELINE**

- **Phase 1-2:** 15 phút (xóa rỗng + di chuyển HAL)
- **Phase 3:** 30 phút (tách API_DOCUMENTATION)
- **Phase 4-7:** 30 phút (di chuyển files)
- **Phase 8-9:** 20 phút (gộp files)
- **Phase 10:** 15 phút (update indexes)
- **TOTAL:** ~110 phút (< 2 giờ)

---

## ✅ **ACCEPTANCE CRITERIA**

- [ ] All empty folders deleted
- [ ] All duplicate files removed
- [ ] All docs properly categorized in ISO structure
- [ ] All README.md files updated
- [ ] All cross-references working
- [ ] No broken links
- [ ] Backup created
- [ ] Final report generated

---

**Status:** 📋 PLAN READY - Awaiting Execution Approval

