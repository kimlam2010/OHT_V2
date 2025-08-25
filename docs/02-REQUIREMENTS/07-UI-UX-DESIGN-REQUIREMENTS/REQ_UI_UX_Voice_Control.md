# 🎤 REQ_UI_UX_Voice_Control - Điều khiển bằng giọng nói

**Phiên bản:** v1.0  
**Ngày tạo:** 2025-01-28  
**Trạng thái:** ✅ Hoàn thành  
**Đối tượng đọc:** UI/UX Designers, Frontend Developers, Product Managers

---

## 🎯 **Mục tiêu Voice Control:**

### **Concept chính:**
- **System:** Voice control interface cho robot OHT-50
- **Focus:** Hands-free operation với clear feedback
- **Style:** Industrial theme với intuitive voice commands
- **Target:** Quản lý và Kỹ sư kỹ thuật cao

### **Nguyên tắc thiết kế:**
1. **Clarity:** Lệnh giọng nói rõ ràng và dễ hiểu
2. **Feedback:** Phản hồi ngay lập tức cho mọi lệnh
3. **Safety:** Xác nhận cho các lệnh quan trọng
4. **Accessibility:** Hỗ trợ nhiều ngôn ngữ và accent

---

## 📊 **BẢNG MAPPING - TÀI LIỆU NÀY:**

| **Phần** | **Mục đích** | **Đối tượng đọc** | **Liên quan đến** | **Trạng thái** |
|----------|--------------|-------------------|-------------------|----------------|
| Voice Commands | Lệnh giọng nói | Designers, Developers | Interaction Design | ✅ Hoàn thành |
| Speech Recognition | Nhận diện giọng nói | Designers, Developers | Technical Implementation | ✅ Hoàn thành |
| Voice Feedback | Phản hồi giọng nói | Designers, Developers | User Experience | ✅ Hoàn thành |
| Voice Interface | Giao diện điều khiển | Designers, Developers | Visual Design | ✅ Hoàn thành |

---

## 🎤 **1. VOICE COMMANDS - Lệnh giọng nói**

### **Command Categories:**
```html
<!-- Voice Command Categories -->
<div class="voice-command-categories">
  <div class="command-category emergency">
    <h3>🚨 Emergency Commands</h3>
    <div class="command-list">
      <div class="command-item">
        <span class="command-phrase">"Emergency Stop"</span>
        <span class="command-action">Immediate halt</span>
      </div>
      <div class="command-item">
        <span class="command-phrase">"Stop Robot"</span>
        <span class="command-action">Safe stop</span>
      </div>
      <div class="command-item">
        <span class="command-phrase">"Pause Operation"</span>
        <span class="command-action">Temporary pause</span>
      </div>
    </div>
  </div>

  <div class="command-category movement">
    <h3>🚀 Movement Commands</h3>
    <div class="command-list">
      <div class="command-item">
        <span class="command-phrase">"Move Forward"</span>
        <span class="command-action">Advance robot</span>
      </div>
      <div class="command-item">
        <span class="command-phrase">"Move Backward"</span>
        <span class="command-action">Reverse robot</span>
      </div>
      <div class="command-item">
        <span class="command-phrase">"Turn Left" / "Turn Right"</span>
        <span class="command-action">Rotate robot</span>
      </div>
      <div class="command-item">
        <span class="command-phrase">"Go to Position X Y"</span>
        <span class="command-action">Navigate to coordinates</span>
      </div>
    </div>
  </div>

  <div class="command-category status">
    <h3>📊 Status Commands</h3>
    <div class="command-list">
      <div class="command-item">
        <span class="command-phrase">"Robot Status"</span>
        <span class="command-action">Get current status</span>
      </div>
      <div class="command-item">
        <span class="command-phrase">"Battery Level"</span>
        <span class="command-action">Check battery</span>
      </div>
      <div class="command-item">
        <span class="command-phrase">"Current Position"</span>
        <span class="command-action">Get coordinates</span>
      </div>
      <div class="command-item">
        <span class="command-phrase">"System Health"</span>
        <span class="command-action">Check system status</span>
      </div>
    </div>
  </div>

  <div class="command-category control">
    <h3>🎮 Control Commands</h3>
    <div class="command-list">
      <div class="command-item">
        <span class="command-phrase">"Start Auto Mode"</span>
        <span class="command-action">Enable autonomous</span>
      </div>
      <div class="command-item">
        <span class="command-phrase">"Manual Control"</span>
        <span class="command-action">Switch to manual</span>
      </div>
      <div class="command-item">
        <span class="command-phrase">"Set Speed X"</span>
        <span class="command-action">Adjust speed</span>
      </div>
      <div class="command-item">
        <span class="command-phrase">"Return to Base"</span>
        <span class="command-action">Navigate to dock</span>
      </div>
    </div>
  </div>
</div>
```

### **Command Categories CSS:**
```css
/* Voice Command Categories */
.voice-command-categories {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
  gap: var(--space-lg);
  margin-bottom: var(--space-xl);
}

.command-category {
  background: var(--bg-primary);
  border: 1px solid var(--industrial-gray-300);
  border-radius: 0.75rem;
  padding: var(--space-lg);
  box-shadow: 0 2px 8px rgba(0, 0, 0, 0.1);
}

.command-category h3 {
  margin-bottom: var(--space-lg);
  padding-bottom: var(--space-sm);
  border-bottom: 2px solid var(--primary-orange);
  color: var(--industrial-gray-700);
  font-weight: var(--font-bold);
}

.command-list {
  display: flex;
  flex-direction: column;
  gap: var(--space-md);
}

.command-item {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: var(--space-md);
  background: var(--bg-secondary);
  border-radius: 0.5rem;
  border-left: 3px solid var(--primary-orange);
  transition: all 0.2s ease;
}

.command-item:hover {
  background: rgba(255, 107, 53, 0.1);
  transform: translateX(4px);
}

.command-phrase {
  font-weight: var(--font-semibold);
  color: var(--industrial-gray-700);
  font-family: var(--font-mono);
  font-size: 0.875rem;
}

.command-action {
  font-size: 0.75rem;
  color: var(--industrial-gray-500);
  text-align: right;
  max-width: 120px;
}

/* Category-specific styling */
.command-category.emergency .command-item {
  border-left-color: var(--error-red);
}

.command-category.emergency .command-item:hover {
  background: rgba(220, 53, 69, 0.1);
}

.command-category.movement .command-item {
  border-left-color: var(--info-blue);
}

.command-category.movement .command-item:hover {
  background: rgba(23, 162, 184, 0.1);
}

.command-category.status .command-item {
  border-left-color: var(--warning-yellow);
}

.command-category.status .command-item:hover {
  background: rgba(255, 193, 7, 0.1);
}

.command-category.control .command-item {
  border-left-color: var(--success-green);
}

.command-category.control .command-item:hover {
  background: rgba(40, 167, 69, 0.1);
}
```

---

## 🎧 **2. SPEECH RECOGNITION - Nhận diện giọng nói**

### **Voice Recognition Interface:**
```html
<!-- Voice Recognition System -->
<div class="voice-recognition-system">
  <div class="recognition-header">
    <h3>🎧 Voice Recognition</h3>
    <div class="recognition-status">
      <span class="status-indicator" id="recognitionStatus"></span>
      <span class="status-text" id="recognitionText">Ready</span>
    </div>
  </div>

  <div class="recognition-controls">
    <button class="voice-btn primary" id="startListening">
      <div class="btn-icon">🎤</div>
      <div class="btn-content">
        <span class="btn-label">Start Listening</span>
        <span class="btn-desc">Click or say "Hey Robot"</span>
      </div>
    </button>

    <button class="voice-btn secondary" id="stopListening" disabled>
      <div class="btn-icon">⏹️</div>
      <div class="btn-content">
        <span class="btn-label">Stop Listening</span>
        <span class="btn-desc">End voice recognition</span>
      </div>
    </button>
  </div>

  <div class="recognition-display">
    <div class="waveform-container" id="waveformContainer">
      <canvas id="waveformCanvas" class="waveform-canvas"></canvas>
    </div>

    <div class="recognition-text">
      <div class="text-display" id="textDisplay">
        <span class="placeholder">Say a command...</span>
      </div>
      <div class="confidence-meter">
        <div class="confidence-bar" id="confidenceBar"></div>
        <span class="confidence-text" id="confidenceText">0%</span>
      </div>
    </div>
  </div>

  <div class="recognition-settings">
    <h4>Recognition Settings</h4>
    <div class="settings-grid">
      <div class="setting-item">
        <label for="languageSelect">Language:</label>
        <select id="languageSelect" class="setting-select">
          <option value="en-US">English (US)</option>
          <option value="en-GB">English (UK)</option>
          <option value="vi-VN">Vietnamese</option>
          <option value="zh-CN">Chinese</option>
        </select>
      </div>

      <div class="setting-item">
        <label for="sensitivitySlider">Sensitivity:</label>
        <input type="range" id="sensitivitySlider" min="0" max="100" value="70">
        <span class="setting-value" id="sensitivityValue">70%</span>
      </div>

      <div class="setting-item">
        <label for="timeoutSlider">Timeout:</label>
        <input type="range" id="timeoutSlider" min="1" max="10" value="5">
        <span class="setting-value" id="timeoutValue">5s</span>
      </div>
    </div>
  </div>
</div>
```

### **Speech Recognition CSS:**
```css
/* Voice Recognition System */
.voice-recognition-system {
  background: var(--bg-primary);
  border: 1px solid var(--industrial-gray-300);
  border-radius: 0.75rem;
  padding: var(--space-lg);
  margin-bottom: var(--space-lg);
  box-shadow: 0 2px 8px rgba(0, 0, 0, 0.1);
}

.recognition-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: var(--space-lg);
  padding-bottom: var(--space-sm);
  border-bottom: 1px solid var(--industrial-gray-300);
}

.recognition-status {
  display: flex;
  align-items: center;
  gap: var(--space-sm);
}

.status-indicator {
  width: 12px;
  height: 12px;
  border-radius: 50%;
  background: var(--success-green);
  animation: pulse 2s infinite;
}

.status-indicator.listening {
  background: var(--primary-orange);
  animation: listening-pulse 1s infinite;
}

.status-indicator.error {
  background: var(--error-red);
  animation: error-pulse 0.5s infinite;
}

.status-text {
  font-weight: var(--font-medium);
  color: var(--industrial-gray-600);
}

/* Recognition Controls */
.recognition-controls {
  display: flex;
  gap: var(--space-lg);
  margin-bottom: var(--space-lg);
}

.voice-btn {
  display: flex;
  align-items: center;
  gap: var(--space-md);
  padding: var(--space-lg);
  border: 2px solid var(--industrial-gray-300);
  border-radius: 0.75rem;
  cursor: pointer;
  transition: all 0.3s ease;
  background: var(--bg-secondary);
  min-width: 200px;
}

.voice-btn:hover:not(:disabled) {
  border-color: var(--primary-orange);
  background: rgba(255, 107, 53, 0.1);
  transform: translateY(-2px);
  box-shadow: 0 4px 12px rgba(255, 107, 53, 0.2);
}

.voice-btn:disabled {
  opacity: 0.5;
  cursor: not-allowed;
}

.voice-btn.primary {
  border-color: var(--primary-orange);
  background: var(--primary-orange);
  color: white;
}

.voice-btn.primary:hover:not(:disabled) {
  background: var(--primary-orange-light);
  border-color: var(--primary-orange-light);
}

.btn-icon {
  font-size: 2rem;
  flex-shrink: 0;
}

.btn-content {
  display: flex;
  flex-direction: column;
  align-items: flex-start;
}

.btn-label {
  font-weight: var(--font-bold);
  font-size: 1rem;
}

.btn-desc {
  font-size: 0.875rem;
  opacity: 0.8;
}

/* Recognition Display */
.recognition-display {
  background: var(--bg-secondary);
  border-radius: 0.75rem;
  padding: var(--space-lg);
  margin-bottom: var(--space-lg);
}

.waveform-container {
  height: 80px;
  background: var(--bg-primary);
  border-radius: 0.5rem;
  margin-bottom: var(--space-md);
  overflow: hidden;
  position: relative;
}

.waveform-canvas {
  width: 100%;
  height: 100%;
  background: linear-gradient(45deg, var(--bg-primary) 25%, transparent 25%),
              linear-gradient(-45deg, var(--bg-primary) 25%, transparent 25%),
              linear-gradient(45deg, transparent 75%, var(--bg-primary) 75%),
              linear-gradient(-45deg, transparent 75%, var(--bg-primary) 75%);
  background-size: 20px 20px;
  background-position: 0 0, 0 10px, 10px -10px, -10px 0px;
}

.recognition-text {
  display: flex;
  flex-direction: column;
  gap: var(--space-sm);
}

.text-display {
  min-height: 60px;
  background: var(--bg-primary);
  border: 1px solid var(--industrial-gray-300);
  border-radius: 0.5rem;
  padding: var(--space-md);
  display: flex;
  align-items: center;
  font-family: var(--font-mono);
  font-size: 1rem;
  color: var(--industrial-gray-700);
}

.placeholder {
  color: var(--industrial-gray-500);
  font-style: italic;
}

.confidence-meter {
  display: flex;
  align-items: center;
  gap: var(--space-sm);
}

.confidence-bar {
  flex: 1;
  height: 8px;
  background: var(--industrial-gray-300);
  border-radius: 4px;
  overflow: hidden;
  position: relative;
}

.confidence-bar::before {
  content: '';
  position: absolute;
  top: 0;
  left: 0;
  height: 100%;
  background: linear-gradient(90deg, var(--error-red) 0%, var(--warning-yellow) 50%, var(--success-green) 100%);
  width: 0%;
  transition: width 0.3s ease;
}

.confidence-text {
  font-size: 0.875rem;
  color: var(--industrial-gray-600);
  font-weight: var(--font-medium);
  min-width: 40px;
}

/* Recognition Settings */
.recognition-settings {
  background: var(--bg-secondary);
  border-radius: 0.75rem;
  padding: var(--space-lg);
}

.recognition-settings h4 {
  margin-bottom: var(--space-lg);
  color: var(--industrial-gray-700);
  font-weight: var(--font-semibold);
}

.settings-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
  gap: var(--space-lg);
}

.setting-item {
  display: flex;
  flex-direction: column;
  gap: var(--space-sm);
}

.setting-item label {
  font-size: 0.875rem;
  color: var(--industrial-gray-700);
  font-weight: var(--font-medium);
}

.setting-select {
  padding: var(--space-sm);
  border: 1px solid var(--industrial-gray-300);
  border-radius: 0.5rem;
  background: var(--bg-primary);
  font-size: 0.875rem;
  color: var(--industrial-gray-700);
}

.setting-select:focus {
  outline: none;
  border-color: var(--primary-orange);
  box-shadow: 0 0 0 2px rgba(255, 107, 53, 0.2);
}

.setting-item input[type="range"] {
  height: 6px;
  border-radius: 3px;
  background: var(--industrial-gray-300);
  outline: none;
  -webkit-appearance: none;
}

.setting-item input[type="range"]::-webkit-slider-thumb {
  -webkit-appearance: none;
  appearance: none;
  width: 16px;
  height: 16px;
  border-radius: 50%;
  background: var(--primary-orange);
  cursor: pointer;
}

.setting-value {
  font-size: 0.875rem;
  color: var(--industrial-gray-600);
  text-align: right;
}

/* Recognition Animations */
@keyframes listening-pulse {
  0% { 
    transform: scale(1);
    opacity: 1;
  }
  50% { 
    transform: scale(1.2);
    opacity: 0.7;
  }
  100% { 
    transform: scale(1);
    opacity: 1;
  }
}

@keyframes error-pulse {
  0%, 100% { 
    transform: scale(1);
  }
  50% { 
    transform: scale(1.1);
  }
}

@keyframes pulse {
  0% { opacity: 1; }
  50% { opacity: 0.5; }
  100% { opacity: 1; }
}
```

---

## 🔊 **3. VOICE FEEDBACK - Phản hồi giọng nói**

### **Voice Feedback System:**
```html
<!-- Voice Feedback System -->
<div class="voice-feedback-system">
  <div class="feedback-header">
    <h3>🔊 Voice Feedback</h3>
    <div class="feedback-controls">
      <button class="feedback-btn" id="testVoice">Test Voice</button>
      <button class="feedback-btn" id="muteVoice">Mute</button>
    </div>
  </div>

  <div class="feedback-settings">
    <div class="setting-group">
      <h4>Voice Settings</h4>
      <div class="settings-row">
        <div class="setting-item">
          <label for="voiceSelect">Voice:</label>
          <select id="voiceSelect" class="setting-select">
            <option value="male">Male Voice</option>
            <option value="female">Female Voice</option>
            <option value="neutral">Neutral Voice</option>
          </select>
        </div>

        <div class="setting-item">
          <label for="speedSlider">Speed:</label>
          <input type="range" id="speedSlider" min="0.5" max="2" step="0.1" value="1">
          <span class="setting-value" id="speedValue">1.0x</span>
        </div>

        <div class="setting-item">
          <label for="volumeSlider">Volume:</label>
          <input type="range" id="volumeSlider" min="0" max="100" value="80">
          <span class="setting-value" id="volumeValue">80%</span>
        </div>
      </div>
    </div>

    <div class="setting-group">
      <h4>Feedback Types</h4>
      <div class="feedback-types">
        <label class="feedback-type">
          <input type="checkbox" id="commandConfirm" checked>
          <span class="checkmark"></span>
          <span class="type-label">Command Confirmation</span>
        </label>

        <label class="feedback-type">
          <input type="checkbox" id="statusUpdates" checked>
          <span class="checkmark"></span>
          <span class="type-label">Status Updates</span>
        </label>

        <label class="feedback-type">
          <input type="checkbox" id="errorAlerts" checked>
          <span class="checkmark"></span>
          <span class="type-label">Error Alerts</span>
        </label>

        <label class="feedback-type">
          <input type="checkbox" id="completionNotices">
          <span class="checkmark"></span>
          <span class="type-label">Completion Notices</span>
        </label>
      </div>
    </div>
  </div>

  <div class="feedback-preview">
    <h4>Voice Preview</h4>
    <div class="preview-messages">
      <div class="preview-item" data-voice="Command confirmed. Moving forward.">
        <span class="preview-text">"Command confirmed. Moving forward."</span>
        <button class="preview-btn" data-action="play">▶️</button>
      </div>

      <div class="preview-item" data-voice="Battery level is 25 percent.">
        <span class="preview-text">"Battery level is 25 percent."</span>
        <button class="preview-btn" data-action="play">▶️</button>
      </div>

      <div class="preview-item" data-voice="Warning. Obstacle detected ahead.">
        <span class="preview-text">"Warning. Obstacle detected ahead."</span>
        <button class="preview-btn" data-action="play">▶️</button>
      </div>

      <div class="preview-item" data-voice="Task completed successfully.">
        <span class="preview-text">"Task completed successfully."</span>
        <button class="preview-btn" data-action="play">▶️</button>
      </div>
    </div>
  </div>
</div>
```

### **Voice Feedback CSS:**
```css
/* Voice Feedback System */
.voice-feedback-system {
  background: var(--bg-primary);
  border: 1px solid var(--industrial-gray-300);
  border-radius: 0.75rem;
  padding: var(--space-lg);
  margin-bottom: var(--space-lg);
  box-shadow: 0 2px 8px rgba(0, 0, 0, 0.1);
}

.feedback-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: var(--space-lg);
  padding-bottom: var(--space-sm);
  border-bottom: 1px solid var(--industrial-gray-300);
}

.feedback-controls {
  display: flex;
  gap: var(--space-sm);
}

.feedback-btn {
  padding: var(--space-sm) var(--space-md);
  background: var(--bg-secondary);
  border: 1px solid var(--industrial-gray-300);
  border-radius: 0.5rem;
  cursor: pointer;
  font-size: 0.875rem;
  transition: all 0.2s ease;
}

.feedback-btn:hover {
  background: var(--primary-orange);
  color: white;
  border-color: var(--primary-orange);
}

/* Feedback Settings */
.feedback-settings {
  display: grid;
  gap: var(--space-lg);
  margin-bottom: var(--space-lg);
}

.setting-group {
  background: var(--bg-secondary);
  border-radius: 0.75rem;
  padding: var(--space-lg);
}

.setting-group h4 {
  margin-bottom: var(--space-lg);
  color: var(--industrial-gray-700);
  font-weight: var(--font-semibold);
}

.settings-row {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
  gap: var(--space-lg);
}

/* Feedback Types */
.feedback-types {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
  gap: var(--space-md);
}

.feedback-type {
  display: flex;
  align-items: center;
  gap: var(--space-sm);
  cursor: pointer;
  padding: var(--space-sm);
  border-radius: 0.5rem;
  transition: all 0.2s ease;
}

.feedback-type:hover {
  background: var(--bg-primary);
}

.feedback-type input {
  display: none;
}

.checkmark {
  width: 20px;
  height: 20px;
  border: 2px solid var(--industrial-gray-300);
  border-radius: 4px;
  position: relative;
  transition: all 0.2s ease;
}

.feedback-type input:checked + .checkmark {
  background: var(--primary-orange);
  border-color: var(--primary-orange);
}

.feedback-type input:checked + .checkmark::after {
  content: '✓';
  position: absolute;
  top: 50%;
  left: 50%;
  transform: translate(-50%, -50%);
  color: white;
  font-size: 12px;
  font-weight: bold;
}

.type-label {
  font-size: 0.875rem;
  color: var(--industrial-gray-700);
  font-weight: var(--font-medium);
}

/* Feedback Preview */
.feedback-preview {
  background: var(--bg-secondary);
  border-radius: 0.75rem;
  padding: var(--space-lg);
}

.feedback-preview h4 {
  margin-bottom: var(--space-lg);
  color: var(--industrial-gray-700);
  font-weight: var(--font-semibold);
}

.preview-messages {
  display: flex;
  flex-direction: column;
  gap: var(--space-md);
}

.preview-item {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: var(--space-md);
  background: var(--bg-primary);
  border-radius: 0.5rem;
  border-left: 3px solid var(--primary-orange);
  transition: all 0.2s ease;
}

.preview-item:hover {
  background: rgba(255, 107, 53, 0.1);
  transform: translateX(4px);
}

.preview-text {
  font-family: var(--font-mono);
  font-size: 0.875rem;
  color: var(--industrial-gray-700);
  flex: 1;
}

.preview-btn {
  background: none;
  border: none;
  font-size: 1.25rem;
  cursor: pointer;
  padding: var(--space-xs);
  border-radius: 0.25rem;
  transition: all 0.2s ease;
  opacity: 0.7;
}

.preview-btn:hover {
  opacity: 1;
  background: var(--industrial-gray-200);
  transform: scale(1.1);
}

.preview-btn.playing {
  color: var(--primary-orange);
  animation: playing-pulse 1s infinite;
}

@keyframes playing-pulse {
  0%, 100% { 
    transform: scale(1);
  }
  50% { 
    transform: scale(1.2);
  }
}
```

---

## 📚 **Tham chiếu:**

- [Dashboard Design](./REQ_UI_UX_Dashboard_Design.md) - Thiết kế dashboard chính
- [Control Panel](./REQ_UI_UX_Control_Panel.md) - Bảng điều khiển tay
- [Alert System](./REQ_UI_UX_Alert_System.md) - Hệ thống cảnh báo
- [Interaction Design](./REQ_UI_UX_Interaction_Design.md) - Thiết kế tương tác

---

**Changelog v1.0:**
- ✅ Created comprehensive voice control system design
- ✅ Defined voice command categories and phrases
- ✅ Designed speech recognition interface
- ✅ Implemented voice feedback system
- ✅ Added voice settings and customization
- ✅ Included accessibility features
- ✅ Created responsive design for voice components
- ✅ Added voice preview and testing functionality
