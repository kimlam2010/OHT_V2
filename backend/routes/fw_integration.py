"""
FW Integration Endpoints for OHT-50 Master Module Backend
Version: 1.0.0
Date: 2025-01-27
Team: Backend
Task: BE-INT-03 (Integration Endpoints)
"""

from flask import Blueprint, request, jsonify, current_app
from typing import Dict, List, Optional, Any
import asyncio
import logging
from datetime import datetime

# Import FW client and protocol
from ..lib.fw_client import FWClient, FWConfig, FWClientError
from ..lib.fw_protocol import FWProtocol, FWCommand, FWCommandType

logger = logging.getLogger(__name__)

# Create Blueprint
fw_integration = Blueprint('fw_integration', __name__, url_prefix='/api/v1/fw')

# Global FW client instance
fw_client: Optional[FWClient] = None
fw_protocol: Optional[FWProtocol] = None

def init_fw_integration(app):
    """Initialize FW integration"""
    global fw_client, fw_protocol
    
    # Create FW client configuration
    fw_config = FWConfig(
        host=app.config.get('FW_HOST', 'localhost'),
        http_port=app.config.get('FW_HTTP_PORT', 8080),
        ws_port=app.config.get('FW_WS_PORT', 8081),
        timeout=app.config.get('FW_TIMEOUT', 30),
        auth_token=app.config.get('FW_AUTH_TOKEN')
    )
    
    # Create FW client and protocol
    fw_client = FWClient(fw_config)
    fw_protocol = FWProtocol(secret_key=app.config.get('FW_SECRET_KEY'))
    
    # Register blueprint
    app.register_blueprint(fw_integration)
    
    logger.info("FW Integration initialized")

@fw_integration.route('/system/status', methods=['GET'])
def get_fw_system_status():
    """Get FW system status"""
    try:
        if not fw_client or not fw_client.is_connected():
            return jsonify({
                'error': 'FW client not connected',
                'status': 'disconnected'
            }), 503
        
        # Get system status from FW
        loop = asyncio.new_event_loop()
        asyncio.set_event_loop(loop)
        try:
            status = loop.run_until_complete(fw_client.get_system_status())
            return jsonify({
                'success': True,
                'data': {
                    'system_name': status.system_name,
                    'version': status.version,
                    'status': status.status,
                    'uptime_ms': status.uptime_ms,
                    'active_modules': status.active_modules,
                    'estop_active': status.estop_active,
                    'safety_ok': status.safety_ok
                },
                'timestamp': datetime.now().isoformat()
            })
        finally:
            loop.close()
            
    except FWClientError as e:
        logger.error(f"FW system status error: {e}")
        return jsonify({
            'error': str(e),
            'status': 'error'
        }), 500
    except Exception as e:
        logger.error(f"Unexpected error: {e}")
        return jsonify({
            'error': 'Internal server error',
            'status': 'error'
        }), 500

@fw_integration.route('/modules', methods=['GET'])
def get_fw_modules():
    """Get FW modules list"""
    try:
        if not fw_client or not fw_client.is_connected():
            return jsonify({
                'error': 'FW client not connected',
                'status': 'disconnected'
            }), 503
        
        # Get modules from FW
        loop = asyncio.new_event_loop()
        asyncio.set_event_loop(loop)
        try:
            modules = loop.run_until_complete(fw_client.get_modules_list())
            return jsonify({
                'success': True,
                'data': {
                    'modules': modules,
                    'count': len(modules)
                },
                'timestamp': datetime.now().isoformat()
            })
        finally:
            loop.close()
            
    except FWClientError as e:
        logger.error(f"FW modules error: {e}")
        return jsonify({
            'error': str(e),
            'status': 'error'
        }), 500
    except Exception as e:
        logger.error(f"Unexpected error: {e}")
        return jsonify({
            'error': 'Internal server error',
            'status': 'error'
        }), 500

@fw_integration.route('/modules/<int:module_id>', methods=['GET'])
def get_fw_module_info(module_id):
    """Get specific FW module information"""
    try:
        if not fw_client or not fw_client.is_connected():
            return jsonify({
                'error': 'FW client not connected',
                'status': 'disconnected'
            }), 503
        
        # Get module info from FW
        loop = asyncio.new_event_loop()
        asyncio.set_event_loop(loop)
        try:
            modules = loop.run_until_complete(fw_client.get_modules_list())
            
            # Find specific module
            module = None
            for m in modules:
                if m['id'] == module_id:
                    module = m
                    break
            
            if not module:
                return jsonify({
                    'error': f'Module {module_id} not found',
                    'status': 'not_found'
                }), 404
            
            return jsonify({
                'success': True,
                'data': module,
                'timestamp': datetime.now().isoformat()
            })
        finally:
            loop.close()
            
    except FWClientError as e:
        logger.error(f"FW module info error: {e}")
        return jsonify({
            'error': str(e),
            'status': 'error'
        }), 500
    except Exception as e:
        logger.error(f"Unexpected error: {e}")
        return jsonify({
            'error': 'Internal server error',
            'status': 'error'
        }), 500

@fw_integration.route('/modules/<int:module_id>/command', methods=['POST'])
def send_fw_module_command(module_id):
    """Send command to FW module"""
    try:
        if not fw_client or not fw_client.is_connected():
            return jsonify({
                'error': 'FW client not connected',
                'status': 'disconnected'
            }), 503
        
        # Get command data from request
        data = request.get_json()
        if not data:
            return jsonify({
                'error': 'No command data provided',
                'status': 'bad_request'
            }), 400
        
        command = data.get('command')
        parameters = data.get('parameters', {})
        
        if not command:
            return jsonify({
                'error': 'Command not specified',
                'status': 'bad_request'
            }), 400
        
        # Send command to FW
        loop = asyncio.new_event_loop()
        asyncio.set_event_loop(loop)
        try:
            result = loop.run_until_complete(
                fw_client.send_module_command(module_id, command, parameters)
            )
            
            return jsonify({
                'success': True,
                'data': result,
                'timestamp': datetime.now().isoformat()
            })
        finally:
            loop.close()
            
    except FWClientError as e:
        logger.error(f"FW module command error: {e}")
        return jsonify({
            'error': str(e),
            'status': 'error'
        }), 500
    except Exception as e:
        logger.error(f"Unexpected error: {e}")
        return jsonify({
            'error': 'Internal server error',
            'status': 'error'
        }), 500

@fw_integration.route('/safety/status', methods=['GET'])
def get_fw_safety_status():
    """Get FW safety status"""
    try:
        if not fw_client or not fw_client.is_connected():
            return jsonify({
                'error': 'FW client not connected',
                'status': 'disconnected'
            }), 503
        
        # Get safety status from FW
        loop = asyncio.new_event_loop()
        asyncio.set_event_loop(loop)
        try:
            safety = loop.run_until_complete(fw_client.get_safety_status())
            return jsonify({
                'success': True,
                'data': safety,
                'timestamp': datetime.now().isoformat()
            })
        finally:
            loop.close()
            
    except FWClientError as e:
        logger.error(f"FW safety status error: {e}")
        return jsonify({
            'error': str(e),
            'status': 'error'
        }), 500
    except Exception as e:
        logger.error(f"Unexpected error: {e}")
        return jsonify({
            'error': 'Internal server error',
            'status': 'error'
        }), 500

@fw_integration.route('/safety/estop', methods=['POST'])
def send_fw_estop():
    """Send emergency stop command to FW"""
    try:
        if not fw_client or not fw_client.is_connected():
            return jsonify({
                'error': 'FW client not connected',
                'status': 'disconnected'
            }), 503
        
        # Send E-Stop command to FW
        loop = asyncio.new_event_loop()
        asyncio.set_event_loop(loop)
        try:
            result = loop.run_until_complete(fw_client.send_estop())
            
            if result:
                return jsonify({
                    'success': True,
                    'message': 'Emergency stop command sent successfully',
                    'timestamp': datetime.now().isoformat()
                })
            else:
                return jsonify({
                    'error': 'Failed to send emergency stop command',
                    'status': 'error'
                }), 500
        finally:
            loop.close()
            
    except FWClientError as e:
        logger.error(f"FW E-Stop error: {e}")
        return jsonify({
            'error': str(e),
            'status': 'error'
        }), 500
    except Exception as e:
        logger.error(f"Unexpected error: {e}")
        return jsonify({
            'error': 'Internal server error',
            'status': 'error'
        }), 500

@fw_integration.route('/config', methods=['GET'])
def get_fw_config():
    """Get FW configuration"""
    try:
        if not fw_client or not fw_client.is_connected():
            return jsonify({
                'error': 'FW client not connected',
                'status': 'disconnected'
            }), 503
        
        # Get config from FW
        loop = asyncio.new_event_loop()
        asyncio.set_event_loop(loop)
        try:
            config = loop.run_until_complete(fw_client.get_config())
            return jsonify({
                'success': True,
                'data': config,
                'timestamp': datetime.now().isoformat()
            })
        finally:
            loop.close()
            
    except FWClientError as e:
        logger.error(f"FW config error: {e}")
        return jsonify({
            'error': str(e),
            'status': 'error'
        }), 500
    except Exception as e:
        logger.error(f"Unexpected error: {e}")
        return jsonify({
            'error': 'Internal server error',
            'status': 'error'
        }), 500

@fw_integration.route('/config', methods=['PUT'])
def update_fw_config():
    """Update FW configuration"""
    try:
        if not fw_client or not fw_client.is_connected():
            return jsonify({
                'error': 'FW client not connected',
                'status': 'disconnected'
            }), 503
        
        # Get config data from request
        data = request.get_json()
        if not data:
            return jsonify({
                'error': 'No configuration data provided',
                'status': 'bad_request'
            }), 400
        
        # Update config in FW
        loop = asyncio.new_event_loop()
        asyncio.set_event_loop(loop)
        try:
            result = loop.run_until_complete(fw_client.update_config(data))
            
            if result:
                return jsonify({
                    'success': True,
                    'message': 'Configuration updated successfully',
                    'timestamp': datetime.now().isoformat()
                })
            else:
                return jsonify({
                    'error': 'Failed to update configuration',
                    'status': 'error'
                }), 500
        finally:
            loop.close()
            
    except FWClientError as e:
        logger.error(f"FW config update error: {e}")
        return jsonify({
            'error': str(e),
            'status': 'error'
        }), 500
    except Exception as e:
        logger.error(f"Unexpected error: {e}")
        return jsonify({
            'error': 'Internal server error',
            'status': 'error'
        }), 500

@fw_integration.route('/diagnostics', methods=['GET'])
def get_fw_diagnostics():
    """Get FW diagnostics"""
    try:
        if not fw_client or not fw_client.is_connected():
            return jsonify({
                'error': 'FW client not connected',
                'status': 'disconnected'
            }), 503
        
        # Get diagnostics from FW
        loop = asyncio.new_event_loop()
        asyncio.set_event_loop(loop)
        try:
            diagnostics = loop.run_until_complete(fw_client.get_diagnostics())
            return jsonify({
                'success': True,
                'data': diagnostics,
                'timestamp': datetime.now().isoformat()
            })
        finally:
            loop.close()
            
    except FWClientError as e:
        logger.error(f"FW diagnostics error: {e}")
        return jsonify({
            'error': str(e),
            'status': 'error'
        }), 500
    except Exception as e:
        logger.error(f"Unexpected error: {e}")
        return jsonify({
            'error': 'Internal server error',
            'status': 'error'
        }), 500

@fw_integration.route('/connect', methods=['POST'])
def connect_to_fw():
    """Connect to FW"""
    try:
        global fw_client
        
        if fw_client and fw_client.is_connected():
            return jsonify({
                'success': True,
                'message': 'Already connected to FW',
                'status': 'connected'
            })
        
        # Connect to FW
        loop = asyncio.new_event_loop()
        asyncio.set_event_loop(loop)
        try:
            connected = loop.run_until_complete(fw_client.connect())
            
            if connected:
                return jsonify({
                    'success': True,
                    'message': 'Connected to FW successfully',
                    'status': 'connected'
                })
            else:
                return jsonify({
                    'error': 'Failed to connect to FW',
                    'status': 'connection_failed'
                }), 500
        finally:
            loop.close()
            
    except Exception as e:
        logger.error(f"FW connection error: {e}")
        return jsonify({
            'error': str(e),
            'status': 'error'
        }), 500

@fw_integration.route('/disconnect', methods=['POST'])
def disconnect_from_fw():
    """Disconnect from FW"""
    try:
        global fw_client
        
        if not fw_client:
            return jsonify({
                'success': True,
                'message': 'Not connected to FW',
                'status': 'disconnected'
            })
        
        # Disconnect from FW
        loop = asyncio.new_event_loop()
        asyncio.set_event_loop(loop)
        try:
            loop.run_until_complete(fw_client.disconnect())
            
            return jsonify({
                'success': True,
                'message': 'Disconnected from FW successfully',
                'status': 'disconnected'
            })
        finally:
            loop.close()
            
    except Exception as e:
        logger.error(f"FW disconnection error: {e}")
        return jsonify({
            'error': str(e),
            'status': 'error'
        }), 500

@fw_integration.route('/status', methods=['GET'])
def get_fw_connection_status():
    """Get FW connection status"""
    try:
        if not fw_client:
            return jsonify({
                'connected': False,
                'status': 'not_initialized',
                'message': 'FW client not initialized'
            })
        
        return jsonify({
            'connected': fw_client.is_connected(),
            'status': fw_client.state.value,
            'config': {
                'host': fw_client.config.host,
                'http_port': fw_client.config.http_port,
                'ws_port': fw_client.config.ws_port
            },
            'timestamp': datetime.now().isoformat()
        })
        
    except Exception as e:
        logger.error(f"FW status error: {e}")
        return jsonify({
            'error': str(e),
            'status': 'error'
        }), 500

# Error handlers
@fw_integration.errorhandler(404)
def not_found(error):
    return jsonify({
        'error': 'Endpoint not found',
        'status': 'not_found'
    }), 404

@fw_integration.errorhandler(500)
def internal_error(error):
    return jsonify({
        'error': 'Internal server error',
        'status': 'error'
    }), 500
