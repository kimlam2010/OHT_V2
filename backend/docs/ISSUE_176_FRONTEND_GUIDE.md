# 📱 Frontend Integration Guide - Issue #176

**Quick Start Guide for Frontend Team**

---

## 🎯 **WHAT'S NEW**

Backend đã expose 2 endpoints mới để lấy register data với **address** và **mode**:

1. ✅ `GET /api/v1/modules/{id}/registers` - Get register data
2. ✅ `POST /api/v1/modules/{id}/registers/{addr}` - Write register (Admin only)

---

## 🚀 **QUICK START**

### **1. Get Register Data (TypeScript/JavaScript)**

```typescript
// src/services/moduleService.ts
import api from '@/lib/api';

export interface RegisterData {
  address: string;        // "0x0000" - HEX ADDRESS
  name: string;          // "Battery Pack Voltage"
  mode: string;          // "READ" | "WRITE" | "READ_WRITE" - MODE
  data_type: string;     // "UINT16"
  description: string;   // "Battery Pack Voltage"
  value: number | null;  // 24400
  timestamp: string | null; // "2025-10-10T10:30:45.123Z"
}

export interface ModuleRegistersResponse {
  success: boolean;
  data: {
    module_addr: number;
    module_name: string;
    online: boolean;
    register_count: number;
    registers: RegisterData[];
  };
}

// Get all registers for a module
export async function getModuleRegisters(moduleId: number): Promise<ModuleRegistersResponse> {
  const response = await api.get<ModuleRegistersResponse>(
    `/api/v1/modules/${moduleId}/registers`
  );
  return response.data;
}

// Write register value (Admin only)
export async function writeModuleRegister(
  moduleId: number, 
  regAddr: string, 
  value: number
): Promise<{success: boolean; message: string}> {
  const response = await api.post(
    `/api/v1/modules/${moduleId}/registers/${regAddr}`,
    { value }
  );
  return response.data;
}
```

---

### **2. Display in React Component**

```tsx
// src/components/ModuleRegisters.tsx
import { useQuery, useMutation } from '@tanstack/react-query';
import { getModuleRegisters, writeModuleRegister } from '@/services/moduleService';

export function ModuleRegisters({ moduleId }: { moduleId: number }) {
  // Fetch registers
  const { data, isLoading, error, refetch } = useQuery({
    queryKey: ['module-registers', moduleId],
    queryFn: () => getModuleRegisters(moduleId),
    refetchInterval: 5000, // Auto-refresh every 5s
  });

  // Write mutation
  const writeMutation = useMutation({
    mutationFn: ({ addr, value }: { addr: string; value: number }) => 
      writeModuleRegister(moduleId, addr, value),
    onSuccess: () => {
      refetch(); // Refresh data after write
    },
  });

  if (isLoading) return <div>Loading registers...</div>;
  if (error) return <div>Error: {error.message}</div>;
  if (!data?.success) return <div>Module offline</div>;

  return (
    <div className="space-y-4">
      <h2 className="text-xl font-bold">
        {data.data.module_name} Registers ({data.data.register_count})
      </h2>
      
      <div className="grid gap-2">
        {data.data.registers.map((reg) => (
          <RegisterRow 
            key={reg.address}
            register={reg}
            onWrite={(value) => writeMutation.mutate({ addr: reg.address, value })}
          />
        ))}
      </div>
    </div>
  );
}

function RegisterRow({ 
  register, 
  onWrite 
}: { 
  register: RegisterData; 
  onWrite: (value: number) => void;
}) {
  const isWritable = register.mode === "WRITE" || register.mode === "READ_WRITE";
  
  return (
    <div className="flex items-center gap-4 p-3 border rounded">
      {/* Address - NEW! */}
      <div className="font-mono text-sm text-blue-600 w-20">
        {register.address}
      </div>
      
      {/* Name */}
      <div className="flex-1">
        <div className="font-medium">{register.name}</div>
        <div className="text-xs text-gray-500">{register.description}</div>
      </div>
      
      {/* Mode Badge - NEW! */}
      <div className={`px-2 py-1 rounded text-xs font-medium ${
        register.mode === 'READ' ? 'bg-blue-100 text-blue-700' :
        register.mode === 'WRITE' ? 'bg-orange-100 text-orange-700' :
        'bg-green-100 text-green-700'
      }`}>
        {register.mode}
      </div>
      
      {/* Value */}
      <div className="w-32 text-right">
        {register.value !== null ? (
          <span className="font-mono">{register.value}</span>
        ) : (
          <span className="text-gray-400">--</span>
        )}
      </div>
      
      {/* Write Button (if writable) */}
      {isWritable && (
        <button
          onClick={() => {
            const newValue = prompt(`Enter value for ${register.name}:`);
            if (newValue !== null) {
              onWrite(Number(newValue));
            }
          }}
          className="px-3 py-1 bg-blue-500 text-white rounded text-sm hover:bg-blue-600"
        >
          Write
        </button>
      )}
    </div>
  );
}
```

---

### **3. Filter by Mode**

```tsx
// Filter registers by mode
const readOnlyRegisters = data.data.registers.filter(r => r.mode === "READ");
const writableRegisters = data.data.registers.filter(r => 
  r.mode === "WRITE" || r.mode === "READ_WRITE"
);

// Group by mode
const registersByMode = {
  READ: data.data.registers.filter(r => r.mode === "READ"),
  WRITE: data.data.registers.filter(r => r.mode === "WRITE"),
  READ_WRITE: data.data.registers.filter(r => r.mode === "READ_WRITE"),
};
```

---

## 🎨 **UI EXAMPLES**

### **Register Table**
```
┌─────────────────────────────────────────────────────────────────────────┐
│ Power Module Registers (58)                                             │
├─────────────────────────────────────────────────────────────────────────┤
│ Address  │ Name                    │ Mode       │ Value    │ Action     │
├──────────┼─────────────────────────┼────────────┼──────────┼────────────┤
│ 0x0000   │ Battery Pack Voltage    │ READ       │ 24400 mV │            │
│ 0x0001   │ Battery Pack Current    │ READ       │ 5400 mA  │            │
│ 0x0002   │ State of Charge         │ READ       │ 67%      │            │
│ 0x0049   │ 12V Relay State         │ READ_WRITE │ 1        │ [Write]    │
│ 0x004A   │ 5V Relay State          │ READ_WRITE │ 1        │ [Write]    │
└──────────┴─────────────────────────┴────────────┴──────────┴────────────┘
```

### **Mode Badge Colors**
- 🔵 `READ` - Blue badge (read-only)
- 🟠 `WRITE` - Orange badge (write-only)
- 🟢 `READ_WRITE` - Green badge (read + write)

---

## 📊 **API RESPONSE STRUCTURE**

```json
{
  "success": true,
  "data": {
    "module_addr": 2,
    "module_name": "Power Module",
    "online": true,
    "register_count": 58,
    "registers": [
      {
        "address": "0x0000",           // ← NEW: Hex address
        "name": "Battery Pack Voltage",
        "mode": "READ",                // ← NEW: Access mode
        "data_type": "UINT16",
        "description": "Battery Pack Voltage",
        "value": 24400,
        "timestamp": "2025-10-10T10:30:45.123Z"
      }
    ]
  }
}
```

---

## 🔒 **AUTHENTICATION**

### **Read Registers (GET):**
- ✅ Requires user login
- ✅ Any authenticated user can read
- Token: `Bearer <access_token>`

### **Write Registers (POST):**
- ✅ Requires **ADMIN** permission
- ❌ Normal users cannot write
- Token: `Bearer <admin_access_token>`

```typescript
// Check if user can write
const canWrite = user.role === 'ADMIN' && register.mode !== 'READ';
```

---

## ⚠️ **ERROR HANDLING**

```typescript
try {
  const data = await getModuleRegisters(2);
  // Success
} catch (error) {
  if (error.response?.status === 404) {
    // Module offline or not found
    toast.error('Module is offline');
  } else if (error.response?.status === 401) {
    // Not authenticated
    toast.error('Please login');
  } else {
    // Other errors
    toast.error('Failed to load registers');
  }
}
```

---

## 🧪 **TESTING**

### **Test with curl:**
```bash
# Login
TOKEN=$(curl -s -X POST http://localhost:8000/api/v1/auth/login \
  -H "Content-Type: application/json" \
  -d '{"username":"admin","password":"admin123"}' \
  | jq -r '.access_token')

# Get registers
curl -H "Authorization: Bearer $TOKEN" \
  http://localhost:8000/api/v1/modules/2/registers | jq

# Write register (Admin only)
curl -H "Authorization: Bearer $TOKEN" \
  -H "Content-Type: application/json" \
  -X POST \
  -d '{"value": 1}' \
  http://localhost:8000/api/v1/modules/2/registers/0x0049 | jq
```

---

## 📋 **IMPLEMENTATION CHECKLIST**

### **Phase 1: Display (High Priority)**
- [ ] Update API client with new endpoints
- [ ] Create `RegisterData` TypeScript interface
- [ ] Display register **address** in UI
- [ ] Display register **mode** badge in UI
- [ ] Show register values
- [ ] Add auto-refresh (5s interval)

### **Phase 2: Write Controls (Medium Priority)**
- [ ] Check user role (Admin only)
- [ ] Add write button for writable registers
- [ ] Implement write form/modal
- [ ] Add value validation
- [ ] Show success/error toast
- [ ] Refresh data after write

### **Phase 3: Advanced (Low Priority)**
- [ ] Filter registers by mode
- [ ] Search registers by name/address
- [ ] Group registers by category
- [ ] Export register data to CSV
- [ ] Real-time updates (WebSocket)

---

## 🎯 **MODULES TO UPDATE**

| **Module ID** | **Name** | **Registers** |
|---------------|----------|---------------|
| 2 | Power Module | 58 |
| 3 | Safety Module | 30+ |
| 4 | Travel Motor Module | 40+ |
| 5 | Dock Module | 20+ |

---

## 📞 **SUPPORT**

**Backend Team Contact:**
- Endpoint documentation: `backend/docs/ISSUE_176_IMPLEMENTATION.md`
- Test script: `backend/test_issue_176.sh`
- API Base URL: `http://localhost:8000`

**Questions?**
- Check Firmware API docs: `firmware_new/docs/06-REFERENCE/REF-005_API_Complete_Documentation.md`
- Run test script: `./backend/test_issue_176.sh`
- Contact Backend team on Slack

---

**Last Updated:** 2025-01-11  
**Status:** ✅ Ready for Frontend Integration  
**Priority:** 🔥 High (blocking Frontend issue #176)

