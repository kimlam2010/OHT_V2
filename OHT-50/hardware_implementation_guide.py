#!/usr/bin/env python3
"""
Hardware Implementation Guide - OHT-50 RS485
Hướng dẫn kết nối RS485 transceiver hardware

Author: EMBED Team
Version: 1.0
Date: 2025-08-16
"""

import time
import os

def print_header():
    """Print header"""
    print("🔧 Hardware Implementation Guide - OHT-50 RS485")
    print("=" * 70)
    print("Hướng dẫn kết nối RS485 transceiver hardware")
    print("=" * 70)

def print_components():
    """Print required components"""
    print("\n📋 COMPONENTS REQUIRED:")
    print("-" * 40)
    print("1. RS485 Transceiver Module:")
    print("   - Model: MAX485 hoặc SN75176B")
    print("   - Voltage: 3.3V")
    print("   - Package: DIP-8 hoặc SOIC-8")
    print()
    print("2. Resistors:")
    print("   - Termination: 120Ω (2 cái)")
    print("   - Bias: 680Ω-10kΩ (2 cái) - optional")
    print()
    print("3. Connectors:")
    print("   - 26-pin header cho Orange Pi 5B")
    print("   - Screw terminal cho module OHT-50")
    print("   - Jumper wires")

def print_wiring_steps():
    """Print wiring steps"""
    print("\n🔌 WIRING STEPS:")
    print("-" * 40)
    print("Step 1: Power Connections")
    print("  Orange Pi 5B → RS485 Transceiver")
    print("  ├─ 3.3V ─────────→ VCC (Pin 8)")
    print("  └─ GND ───────────→ GND (Pin 5)")
    print()
    print("Step 2: GPIO Control")
    print("  Orange Pi 5B → RS485 Transceiver")
    print("  ├─ GPIO 46 (wPi 1) → TX (Pin 4)  # Output")
    print("  └─ GPIO 47 (wPi 0) → RX (Pin 1)  # Input")
    print()
    print("Step 3: RS485 Bus")
    print("  RS485 Transceiver → Module OHT-50")
    print("  ├─ A (Pin 6) ─────→ A (RS485+)")
    print("  └─ B (Pin 7) ─────→ B (RS485-)")
    print()
    print("Step 4: Termination")
    print("  ├─ A line ──[120Ω]─── GND")
    print("  └─ B line ──[120Ω]─── GND")

def print_verification_steps():
    """Print verification steps"""
    print("\n🔍 VERIFICATION STEPS:")
    print("-" * 40)
    print("1. Continuity Test:")
    print("   - TX → TX: Continuity")
    print("   - RX → RX: Continuity")
    print("   - GND → GND: Continuity")
    print()
    print("2. Voltage Test:")
    print("   - VCC: 3.3V ±5%")
    print("   - A-B differential: 0V (idle)")
    print()
    print("3. GPIO Test:")
    print("   - GPIO 46: Output mode")
    print("   - GPIO 47: Input mode")

def print_test_commands():
    """Print test commands"""
    print("\n🧪 TEST COMMANDS:")
    print("-" * 40)
    print("1. Test GPIO (after wiring):")
    print("   sudo python3 test_gpio_sysfs.py")
    print()
    print("2. Test RS485 communication:")
    print("   sudo python3 quick_test.py")
    print()
    print("3. Test with module:")
    print("   sudo python3 test_with_module.py")
    print()
    print("4. Production test:")
    print("   sudo python3 production_test.py")

def print_safety_notes():
    """Print safety notes"""
    print("\n🚨 SAFETY NOTES:")
    print("-" * 40)
    print("1. Power Supply:")
    print("   - Không vượt quá 3.3V cho logic")
    print("   - Đảm bảo common ground")
    print()
    print("2. ESD Protection:")
    print("   - Sử dụng ESD strap")
    print("   - Tránh chạm vào pins khi có điện")
    print()
    print("3. EMI Considerations:")
    print("   - Sử dụng shielded cable cho RS485 bus")
    print("   - Termination đúng impedance")

def print_troubleshooting():
    """Print troubleshooting guide"""
    print("\n🔧 TROUBLESHOOTING:")
    print("-" * 40)
    print("1. No communication:")
    print("   - Kiểm tra wiring connections")
    print("   - Kiểm tra termination resistors")
    print("   - Kiểm tra power supply")
    print()
    print("2. Signal noise:")
    print("   - Kiểm tra grounding")
    print("   - Kiểm tra shielding")
    print("   - Kiểm tra cable quality")
    print()
    print("3. Wrong baud rate:")
    print("   - Verify 115200 baud")
    print("   - Check 8N1 format")
    print()
    print("4. GPIO issues:")
    print("   - Check GPIO permissions")
    print("   - Verify pin assignments")
    print("   - Check sysfs access")

def main():
    """Main function"""
    print_header()
    print_components()
    print_wiring_steps()
    print_verification_steps()
    print_test_commands()
    print_safety_notes()
    print_troubleshooting()
    
    print("\n" + "=" * 70)
    print("📝 NEXT STEPS:")
    print("=" * 70)
    print("1. Chuẩn bị components theo danh sách")
    print("2. Kết nối wiring theo hướng dẫn")
    print("3. Chạy verification tests")
    print("4. Test với module OHT-50")
    print("5. Chạy production test")
    print("\n🎯 Goal: RS485 communication với module OHT-50")

if __name__ == "__main__":
    main()
