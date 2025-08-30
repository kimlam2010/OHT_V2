# 🧭 OHT-50 NAVIGATION GUIDE - V2.1

**Phiên bản:** v2.1  
**Ngày cập nhật:** 2025-01-28  
**Mục tiêu:** Hướng dẫn navigation system với TOP NAVIGATION bắt buộc và không dùng sidebar bên trái

---

## 🚨 NAVIGATION POLICY

- ✅ Bắt buộc: Top Navigation Bar với Emergency Controls (E-STOP, Pause, Reset)
- ✅ Bắt buộc: Breadcrumb trong main content
- ✅ Bắt buộc: Responsive + Keyboard + Screen reader
- ❌ Không sử dụng: Sidebar bên trái trên mọi trang

---

## 🧱 CẤU TRÚC TOP NAVIGATION (BẮT BUỘC)

```html
<nav class="top-navigation" role="navigation" aria-label="Main navigation">
  <div class="nav-brand">
    <i data-lucide="activity" class="w-6 h-6" aria-hidden="true"></i>
    <span>OHT-50</span>
  </div>

  <!-- Emergency Controls: ALWAYS VISIBLE -->
  <div class="nav-emergency-controls">
    <button class="btn-emergency-nav" id="emergency-stop" aria-label="Emergency Stop">
      <i data-lucide="square" class="w-4 h-4" aria-hidden="true"></i>
      E-STOP
    </button>
    <button class="btn-nav btn-warning-nav" id="pause-system" aria-label="Pause System">
      <i data-lucide="pause" class="w-4 h-4" aria-hidden="true"></i>
      Pause
    </button>
    <button class="btn-nav btn-success-nav" id="reset-system" aria-label="Reset System">
      <i data-lucide="refresh-cw" class="w-4 h-4" aria-hidden="true"></i>
      Reset
    </button>
  </div>

  <div class="nav-menu">
    <a href="/dashboard" class="nav-item" aria-current="page">
      <i data-lucide="layout-dashboard" class="w-4 h-4" aria-hidden="true"></i>
      Dashboard
    </a>
    <a href="/control" class="nav-item">
      <i data-lucide="gamepad-2" class="w-4 h-4" aria-hidden="true"></i>
      Control Panel
    </a>
    <a href="/map" class="nav-item">
      <i data-lucide="map" class="w-4 h-4" aria-hidden="true"></i>
      Map & Navigation
    </a>
    <a href="/monitoring" class="nav-item">
      <i data-lucide="monitor" class="w-4 h-4" aria-hidden="true"></i>
      Monitoring
    </a>
    <a href="/config" class="nav-item">
      <i data-lucide="settings" class="w-4 h-4" aria-hidden="true"></i>
      Configuration
    </a>
    <a href="/reports" class="nav-item">
      <i data-lucide="file-text" class="w-4 h-4" aria-hidden="true"></i>
      Reports
    </a>
  </div>

  <div class="nav-actions">
    <button class="nav-btn" aria-label="Notifications"><i data-lucide="bell" class="w-5 h-5" aria-hidden="true"></i></button>
    <button class="nav-btn" aria-label="User menu"><i data-lucide="user" class="w-5 h-5" aria-hidden="true"></i></button>
  </div>
</nav>
```

---

## 🎨 CSS CỐT LÕI

```css
.top-navigation {
  position: fixed; top: 0; left: 0; right: 0;
  display: flex; align-items: center; justify-content: space-between;
  padding: var(--space-md) var(--space-lg);
  background: var(--bg-primary);
  border-bottom: var(--border-width-1) solid var(--border-color-light);
  box-shadow: var(--shadow-sm);
  z-index: var(--z-sticky);
}
.nav-emergency-controls { display: flex; gap: var(--space-sm); margin-left: var(--space-lg); }
.btn-emergency-nav { background: var(--safety-red); color: #fff; border: 0; border-radius: var(--radius-md); padding: var(--space-sm) var(--space-md); font-weight: var(--font-bold); }
.btn-emergency-nav:hover { background: var(--error-red-dark); transform: translateY(-1px); }
.nav-menu { display: flex; gap: var(--space-lg); }
.nav-item { display: flex; gap: var(--space-xs); padding: var(--space-sm) var(--space-md); border-radius: var(--radius-md); color: var(--industrial-gray-700); text-decoration: none; }
.nav-item:hover { background: var(--industrial-gray-100); color: var(--primary-orange); }
.nav-item.active { background: var(--primary-orange); color: #fff; }
.nav-actions { display: flex; gap: var(--space-sm); }

/* Responsive */
@media (max-width: 768px) {
  .nav-menu { display: none; }
  .nav-emergency-controls { margin-left: var(--space-sm); gap: var(--space-xs); }
  .btn-emergency-nav, .btn-nav { padding: var(--space-xs) var(--space-sm); font-size: var(--text-xs); }
}
```

---

## 🧭 BREADCRUMB

```html
<nav class="breadcrumb-nav" aria-label="Breadcrumb">
  <a href="/dashboard" class="breadcrumb-item">Dashboard</a>
  <span class="breadcrumb-separator">/</span>
  <span class="breadcrumb-item active">Current Page</span>
</nav>
```

```css
.breadcrumb-nav { display:flex; gap: var(--space-sm); padding: var(--space-sm) 0; }
.breadcrumb-item { color: var(--industrial-gray-600); font-size: var(--text-sm); text-decoration:none; }
.breadcrumb-item:hover { color: var(--primary-orange); }
.breadcrumb-item.active { color: var(--industrial-gray-900); font-weight: var(--font-medium); }
.breadcrumb-separator { color: var(--industrial-gray-400); }
```

---

## ♿ ACCESSIBILITY

- Skip link: `<a href="#main-content" class="skip-link">Skip to main content</a>`
- Keyboard: Escape = E-STOP, P = Pause, R = Reset
- ARIA labels cho nav, buttons, breadcrumb

---

## ✅ CHECKLIST (MUST PASS)

- Top nav hiện trên mọi trang và cố định (fixed)
- Emergency Controls luôn hiển thị và hoạt động
- Breadcrumb đúng và cập nhật theo trang
- Mobile responsive, keyboard navigation, screen reader OK
- Không có sidebar bên trái ở bất kỳ trang nào

---

**Changelog v2.1:**
- Loại bỏ hoàn toàn left sidebar
- Bắt buộc Top Navigation + Emergency Controls
- Bổ sung CSS/HTML mẫu + checklist
