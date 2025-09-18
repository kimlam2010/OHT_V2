#!/usr/bin/env python3
"""
Test script to generate mock alerts and test WebSocket broadcasting
Tests Issue #79 - WebSocket cho active alert
"""

import asyncio
import logging
import sys
import os
from datetime import datetime, timezone

# Add project root to Python path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..'))

from app.services.alert_system import alert_system, AlertType, AlertSeverity, AlertSource

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)


async def generate_test_alerts():
    """Generate test alerts to trigger WebSocket notifications"""
    logger.info("🚨 Generating test alerts...")
    
    # Test data for different alert types
    test_scenarios = [
        {
            "source": AlertSource.PERFORMANCE_MONITOR,
            "data": {"cpu_usage": 85.0, "threshold": 80.0},
            "description": "High CPU usage alert"
        },
        {
            "source": AlertSource.PERFORMANCE_MONITOR,
            "data": {"memory_usage": 90.0, "threshold": 85.0},
            "description": "High memory usage alert"
        },
        {
            "source": AlertSource.SAFETY_SYSTEM,
            "data": {"emergency_stop": True, "reason": "Manual activation"},
            "description": "Emergency stop alert"
        },
        {
            "source": AlertSource.ANOMALY_DETECTOR,
            "data": {"is_anomaly": True, "severity": "high", "confidence": 0.95},
            "description": "Anomaly detection alert"
        },
        {
            "source": AlertSource.TELEMETRY,
            "data": {"communication_status": "lost", "last_seen": datetime.now(timezone.utc).isoformat()},
            "description": "Communication lost alert"
        }
    ]
    
    generated_alerts = []
    
    for i, scenario in enumerate(test_scenarios):
        try:
            logger.info(f"📝 Generating alert {i+1}: {scenario['description']}")
            
            # Process alert data through alert system
            alerts = await alert_system.process_alert_data(
                source=scenario["source"],
                data=scenario["data"]
            )
            
            if alerts:
                for alert in alerts:
                    generated_alerts.append(alert)
                    logger.info(f"✅ Generated alert: {alert.title} (ID: {alert.alert_id})")
            else:
                logger.warning(f"⚠️ No alerts generated for scenario {i+1}")
            
            # Wait between alerts
            await asyncio.sleep(2)
            
        except Exception as e:
            logger.error(f"❌ Failed to generate alert {i+1}: {e}")
    
    logger.info(f"🎯 Generated {len(generated_alerts)} alerts total")
    return generated_alerts


async def test_alert_actions(alerts):
    """Test alert acknowledgment and resolution"""
    if not alerts:
        logger.warning("⚠️ No alerts to test actions on")
        return
    
    logger.info("🔧 Testing alert actions...")
    
    try:
        # Test acknowledging first alert
        if len(alerts) > 0:
            alert = alerts[0]
            logger.info(f"👤 Acknowledging alert: {alert.title}")
            
            success = await alert_system.acknowledge_alert(alert.alert_id, "test_user")
            if success:
                logger.info("✅ Alert acknowledged successfully")
            else:
                logger.warning("⚠️ Failed to acknowledge alert")
            
            await asyncio.sleep(2)
        
        # Test resolving second alert
        if len(alerts) > 1:
            alert = alerts[1]
            logger.info(f"✅ Resolving alert: {alert.title}")
            
            success = await alert_system.resolve_alert(alert.alert_id, "test_user")
            if success:
                logger.info("✅ Alert resolved successfully")
            else:
                logger.warning("⚠️ Failed to resolve alert")
            
            await asyncio.sleep(2)
        
    except Exception as e:
        logger.error(f"❌ Error testing alert actions: {e}")


async def print_alert_statistics():
    """Print current alert statistics"""
    logger.info("📊 Current Alert Statistics:")
    
    try:
        stats = alert_system.get_alert_statistics()
        
        logger.info(f"  Total alerts: {stats['total_alerts']}")
        logger.info(f"  Active alerts: {stats['active_alerts']}")
        logger.info(f"  Acknowledged alerts: {stats['acknowledged_alerts']}")
        logger.info(f"  Resolved alerts: {stats['resolved_alerts']}")
        
        logger.info("  Alerts by severity:")
        for severity, count in stats['alerts_by_severity'].items():
            logger.info(f"    {severity}: {count}")
        
        logger.info("  Alerts by type:")
        for alert_type, count in stats['alerts_by_type'].items():
            logger.info(f"    {alert_type}: {count}")
        
        # Show active alerts
        active_alerts = alert_system.get_active_alerts()
        logger.info(f"\n📋 Active Alerts ({len(active_alerts)}):")
        
        for alert in active_alerts[:5]:  # Show first 5
            logger.info(f"  - {alert.title} ({alert.severity.value}) - {alert.status.value}")
        
        if len(active_alerts) > 5:
            logger.info(f"  ... and {len(active_alerts) - 5} more")
        
    except Exception as e:
        logger.error(f"❌ Error getting statistics: {e}")


async def main():
    """Main test function"""
    logger.info("🚀 Starting Alert Generation Test for Issue #79")
    logger.info("This will generate test alerts that should trigger WebSocket notifications")
    
    try:
        # Print initial statistics
        logger.info("📊 Initial state:")
        await print_alert_statistics()
        
        # Generate test alerts
        alerts = await generate_test_alerts()
        
        # Print statistics after generation
        logger.info("\n📊 After alert generation:")
        await print_alert_statistics()
        
        # Test alert actions
        await test_alert_actions(alerts)
        
        # Print final statistics
        logger.info("\n📊 Final state:")
        await print_alert_statistics()
        
        logger.info("\n🎉 Alert generation test completed!")
        logger.info("💡 To see WebSocket notifications, run test_websocket_alerts.py in another terminal")
        
    except KeyboardInterrupt:
        logger.info("🛑 Test interrupted by user")
    except Exception as e:
        logger.error(f"❌ Test failed: {e}")


if __name__ == "__main__":
    asyncio.run(main())
