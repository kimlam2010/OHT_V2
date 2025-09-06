/**
 * @file api_integration_example.c
 * @brief API Integration Example for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-28
 * @team FW
 * @task FW-01 (WebSocket & Integration Implementation)
 */

#include "api_manager.h"
#include "api_endpoints.h"
#include "http_server.h"
#include "websocket_server.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/**
 * @brief Example of how to integrate API Manager in main.c
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_integration_example(void) {
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Integration: Starting example...");
    
    // 1. Initialize API Manager with default configuration
    api_mgr_config_t api_config = {0};
    api_manager_set_default_config(&api_config);
    api_config.http_port = 8080;
    api_config.websocket_port = 8081;
    api_config.enable_cors = true;
    strcpy(api_config.cors_origin, "*");
    
    hal_status_t result = api_manager_init(&api_config);
    if (result != HAL_STATUS_OK) {
        hal_log_error("API_INTEGRATION", "api_integration_example", __LINE__, 
                     result, "Failed to initialize API Manager");
        return result;
    }
    
    // 2. Initialize API Endpoints
    result = api_endpoints_init();
    if (result != HAL_STATUS_OK) {
        hal_log_error("API_INTEGRATION", "api_integration_example", __LINE__, 
                     result, "Failed to initialize API Endpoints");
        api_manager_deinit();
        return result;
    }
    
    // 3. Start API Manager services
    result = api_manager_start();
    if (result != HAL_STATUS_OK) {
        hal_log_error("API_INTEGRATION", "api_integration_example", __LINE__, 
                     result, "Failed to start API Manager");
        api_endpoints_deinit();
        api_manager_deinit();
        return result;
    }
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Integration: Services started successfully");
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Integration: HTTP server on port %d", api_config.http_port);
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Integration: WebSocket server on port %d", api_config.websocket_port);
    
    // 4. Example: Broadcast telemetry data via WebSocket
    char telemetry_json[512];
    snprintf(telemetry_json, sizeof(telemetry_json),
        "{\n"
        "  \"timestamp\": %lu,\n"
        "  \"system_status\": \"idle\",\n"
        "  \"battery_level\": 87,\n"
        "  \"position\": {\"x\": 150.5, \"y\": 200.3},\n"
        "  \"safety_ok\": true\n"
        "}",
        hal_get_timestamp_ms()
    );
    
    result = api_manager_broadcast_websocket_message(telemetry_json, strlen(telemetry_json));
    if (result != HAL_STATUS_OK) {
        hal_log_error("API_INTEGRATION", "api_integration_example", __LINE__, 
                     result, "Failed to broadcast telemetry");
    } else {
        hal_log_message(HAL_LOG_LEVEL_INFO, "API Integration: Telemetry broadcasted successfully");
    }
    
    // 5. Example: Get API Manager status
    api_mgr_status_t api_status;
    result = api_manager_get_status(&api_status);
    if (result == HAL_STATUS_OK) {
        hal_log_message(HAL_LOG_LEVEL_INFO, "API Integration: Status - HTTP: %s, WebSocket: %s, Clients: %u",
                       api_status.http_server_running ? "running" : "stopped",
                       api_status.websocket_server_running ? "running" : "stopped",
                       api_status.active_websocket_connections);
    }
    
    // 6. Example: Get API Endpoints status
    bool endpoints_initialized;
    uint32_t endpoint_count;
    result = api_endpoints_get_status(&endpoints_initialized, &endpoint_count);
    if (result == HAL_STATUS_OK) {
        hal_log_message(HAL_LOG_LEVEL_INFO, "API Integration: Endpoints - Initialized: %s, Count: %u",
                       endpoints_initialized ? "yes" : "no", endpoint_count);
    }
    
    // 7. Example: Test API endpoints (simulate HTTP requests)
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Integration: Testing endpoints...");
    
    // Test system status endpoint
    api_system_status_t system_status;
    result = api_get_system_status(&system_status);
    if (result == HAL_STATUS_OK) {
        hal_log_message(HAL_LOG_LEVEL_INFO, "API Integration: System Status - %s, Uptime: %lu ms",
                       system_status.status, system_status.uptime_ms);
    }
    
    // Test safety status endpoint
    api_safety_status_t safety_status;
    result = api_get_safety_status(&safety_status);
    if (result == HAL_STATUS_OK) {
        hal_log_message(HAL_LOG_LEVEL_INFO, "API Integration: Safety Status - OK: %s, E-Stop: %s",
                       safety_status.safety_ok ? "yes" : "no",
                       safety_status.estop_active ? "active" : "inactive");
    }
    
    // Test modules list endpoint
    api_modules_list_t modules_list;
    result = api_get_modules_list(&modules_list);
    if (result == HAL_STATUS_OK) {
        hal_log_message(HAL_LOG_LEVEL_INFO, "API Integration: Modules - Count: %u", modules_list.module_count);
        for (uint32_t i = 0; i < modules_list.module_count; i++) {
            hal_log_message(HAL_LOG_LEVEL_INFO, "API Integration: Module %u - ID: %u, Type: %s, Online: %s",
                           i, modules_list.modules[i].module_id,
                           modules_list.modules[i].module_type,
                           modules_list.modules[i].online ? "yes" : "no");
        }
    }
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Integration: Example completed successfully");
    
    return HAL_STATUS_OK;
}

/**
 * @brief Example of how to cleanup API services
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_integration_cleanup_example(void) {
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Integration: Starting cleanup...");
    
    // 1. Stop API Manager services
    hal_status_t result = api_manager_stop();
    if (result != HAL_STATUS_OK) {
        hal_log_error("API_INTEGRATION", "api_integration_cleanup_example", __LINE__, 
                     result, "Failed to stop API Manager");
    }
    
    // 2. Deinitialize API Endpoints
    result = api_endpoints_deinit();
    if (result != HAL_STATUS_OK) {
        hal_log_error("API_INTEGRATION", "api_integration_cleanup_example", __LINE__, 
                     result, "Failed to deinitialize API Endpoints");
    }
    
    // 3. Deinitialize API Manager
    result = api_manager_deinit();
    if (result != HAL_STATUS_OK) {
        hal_log_error("API_INTEGRATION", "api_integration_cleanup_example", __LINE__, 
                     result, "Failed to deinitialize API Manager");
    }
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Integration: Cleanup completed");
    
    return HAL_STATUS_OK;
}

/**
 * @brief Example of how to add custom endpoints
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_integration_custom_endpoints_example(void) {
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Integration: Adding custom endpoints...");
    
    // Example custom endpoint handler
    hal_status_t custom_handler(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
        (void)request; // Unused parameter
        
        char custom_json[256];
        snprintf(custom_json, sizeof(custom_json),
            "{\n"
            "  \"message\": \"Hello from custom endpoint!\",\n"
            "  \"timestamp\": %lu,\n"
            "  \"version\": \"1.0.0\"\n"
            "}",
            hal_get_timestamp_ms()
        );
        
        return api_manager_create_success_response(response, custom_json);
    }
    
    // Register custom endpoint
    api_mgr_endpoint_t custom_endpoint = {
        .method = API_MGR_HTTP_GET,
        .path = "/api/v1/custom/hello",
        .handler = custom_handler,
        .requires_authentication = false,
        .description = "Custom hello endpoint"
    };
    
    hal_status_t result = api_manager_register_endpoint(&custom_endpoint);
    if (result != HAL_STATUS_OK) {
        hal_log_error("API_INTEGRATION", "api_integration_custom_endpoints_example", __LINE__, 
                     result, "Failed to register custom endpoint");
        return result;
    }
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Integration: Custom endpoint registered successfully");
    
    return HAL_STATUS_OK;
}

/**
 * @brief Example of WebSocket message handler
 * @param message WebSocket message
 * @param message_length Message length
 * @param client WebSocket client
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_websocket_message_handler_example(const char *message, size_t message_length, void *client) {
    (void)client; // Unused parameter
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Integration: Received WebSocket message: %.*s", 
                   (int)message_length, message);
    
    // Example: Echo message back to client
    char echo_message[512];
    snprintf(echo_message, sizeof(echo_message),
        "{\n"
        "  \"type\": \"echo\",\n"
        "  \"original_message\": \"%.*s\",\n"
        "  \"timestamp\": %lu\n"
        "}",
        (int)message_length, message,
        hal_get_timestamp_ms()
    );
    
    // Note: In real implementation, you would send this back to the specific client
    // For this example, we'll just log it
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Integration: Echo response: %s", echo_message);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Example of how to register WebSocket message handler
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_integration_websocket_handler_example(void) {
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Integration: Registering WebSocket message handler...");
    
    // Register WebSocket message handler
    hal_status_t result = api_manager_register_websocket_handler(api_websocket_message_handler_example);
    if (result != HAL_STATUS_OK) {
        hal_log_error("API_INTEGRATION", "api_integration_websocket_handler_example", __LINE__, 
                     result, "Failed to register WebSocket message handler");
        return result;
    }
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Integration: WebSocket message handler registered successfully");
    
    return HAL_STATUS_OK;
}
