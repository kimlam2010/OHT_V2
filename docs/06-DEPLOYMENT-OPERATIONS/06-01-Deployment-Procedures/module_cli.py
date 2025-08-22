#!/usr/bin/env python3
import argparse
import sys
import time
import os
import yaml

def cmd_scan(args):
    """Scan for modules on RS485 bus (addresses 0x02-0x07)"""
    print("Scanning RS485 bus for modules...")
    print("ADDR  TYPE    STATUS    DEVICE_ID")
    
    # Simulate scanning addresses 0x02-0x07
    for addr in range(0x02, 0x08):
        addr_hex = f"0x{addr:02X}"
        
        # Simulate different responses based on address
        if addr in [0x02, 0x03, 0x04, 0x05]:
            # Simulate online modules
            device_id = 0x1234 + addr
            module_type = "power" if addr == 0x02 else "motor" if addr == 0x03 else "io" if addr == 0x04 else "dock"
            print(f"{addr_hex}  {module_type:6}   online    0x{device_id:04X}")
        else:
            # Simulate offline/not found
            print(f"{addr_hex}  --      offline   --")
    
    print(f"\nScan complete: found 4 modules")
    return 0

def cmd_list(args):
    """List discovered modules from modules.yaml"""
    yaml_file = "firmware/modules.yaml"
    
    if not os.path.exists(yaml_file):
        print(f"Error: {yaml_file} not found")
        return 1
    
    print("ADDR  TYPE    NAME      VERSION  STATUS")
    print("----  ----    ----      -------  ------")
    
    try:
        with open(yaml_file, 'r') as f:
            for line in f:
                line = line.strip()
                if line.startswith('#') or line.startswith('version:') or not line:
                    continue
                
                # Parse: addr:0x02,type:motor,name:motor_main,version:1.0,status:online
                parts = line.split(',')
                addr = parts[0].split(':')[1]
                module_type = parts[1].split(':')[1]
                name = parts[2].split(':')[1]
                version = parts[3].split(':')[1]
                status = parts[4].split(':')[1]
                
                print(f"{addr:4}  {module_type:6}  {name:8}  {version:7}  {status}")
    except Exception as e:
        print(f"Error reading {yaml_file}: {e}")
        return 1
    
    return 0

def cmd_ping(args):
    """Ping a module by address"""
    addr = args.addr.lower()
    print(f"Pinging module {addr} ...")
    
    # Simulate ping response
    time.sleep(0.1)
    
    # Simulate different responses
    if addr in ["0x02", "0x03", "0x04", "0x05"]:
        print("ACK - Module online")
        return 0
    else:
        print("TIMEOUT - Module not responding")
        return 1

def cmd_power(args):
    """Power module specific commands"""
    addr = args.addr.lower()
    
    if args.cmd == "status":
        print(f"Power module {addr} status:")
        print("  Voltage: 24.5V")
        print("  Current: 3.2A")
        print("  Temperature: 42.1°C")
        print("  Relay 1: ON")
        print("  Relay 2: OFF")
        print("  Alarms: None")
        return 0
    elif args.cmd == "voltage":
        print(f"Power module {addr} voltage: 24.5V")
        return 0
    elif args.cmd == "current":
        print(f"Power module {addr} current: 3.2A")
        return 0
    elif args.cmd == "relay1":
        if args.state == "on":
            print(f"Power module {addr} relay 1: ON")
        else:
            print(f"Power module {addr} relay 1: OFF")
        return 0
    elif args.cmd == "relay2":
        if args.state == "on":
            print(f"Power module {addr} relay 2: ON")
        else:
            print(f"Power module {addr} relay 2: OFF")
        return 0
    else:
        print(f"Unknown power command: {args.cmd}")
        return 1

def cmd_save(args):
    """Save current module state to modules.yaml"""
    print("Saving module registry to modules.yaml...")
    
    # This would normally trigger a save from the firmware
    # For now, just simulate success
    print("Module registry saved successfully")
    return 0

def build_parser():
    p = argparse.ArgumentParser(prog="module_cli", description="OHT-50 Module CLI")
    sub = p.add_subparsers(dest="cmd", required=True)

    s = sub.add_parser("scan", help="Scan for modules on RS485 bus")
    s.set_defaults(func=cmd_scan)

    s = sub.add_parser("list", help="List discovered modules")
    s.set_defaults(func=cmd_list)

    s = sub.add_parser("ping", help="Ping a module by address")
    s.add_argument("--addr", required=True, help="Module address, e.g. 0x02")
    s.set_defaults(func=cmd_ping)
    
    s = sub.add_parser("power", help="Power module specific commands")
    s.add_argument("--addr", default="0x02", help="Power module address (default: 0x02)")
    s.add_argument("cmd", choices=["status", "voltage", "current", "relay1", "relay2"], 
                   help="Power module command")
    s.add_argument("--state", choices=["on", "off"], help="Relay state (for relay commands)")
    s.set_defaults(func=cmd_power)
    
    s = sub.add_parser("save", help="Save module registry to modules.yaml")
    s.set_defaults(func=cmd_save)
    
    return p

def main(argv=None):
    argv = argv or sys.argv[1:]
    parser = build_parser()
    args = parser.parse_args(argv)
    return args.func(args)

if __name__ == "__main__":
    sys.exit(main())


