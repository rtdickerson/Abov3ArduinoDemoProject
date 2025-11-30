#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ============================================================================
// HARDWARE CONFIGURATION
// ============================================================================

// LED Configuration
#define LED_PIN 5                    // GPIO pin for WS2812B data line
#define LED_COUNT 16                 // 16 LEDs per ring
#define LED_BRIGHTNESS 200           // 0-255 brightness level

// ============================================================================
// MESH NETWORK CONFIGURATION
// ============================================================================

// Network settings
#define MESH_SSID "ESP32_MESH"       // Mesh network name
#define MESH_PASSWORD "meshpass123"  // Mesh network password
#define MESH_PORT 5555               // Communication port

// Node identification
#define NODE_TYPE_MASTER 1            // Node type: master
#define NODE_TYPE_CLIENT 0            // Node type: client
#define MASTER_NODE_ID 1              // Master node ID

// ============================================================================
// TIMING CONFIGURATION
// ============================================================================

// Color broadcast interval (milliseconds)
#define COLOR_BROADCAST_INTERVAL 2000 // Master sends new color every 2 seconds

// Mesh initialization timeout
#define MESH_INIT_TIMEOUT 10000       // Wait 10 seconds for mesh to establish

// LED update timeout (safety check)
#define LED_UPDATE_TIMEOUT 5000       // Max time to wait for LED update

// ============================================================================
// COMMUNICATION PROTOCOL
// ============================================================================

// Message types
#define MSG_TYPE_COLOR_UPDATE 1       // Color update message from master
#define MSG_TYPE_HEARTBEAT 2          // Heartbeat/keep-alive
#define MSG_TYPE_NODE_DISCOVERY 3     // Node discovery request/response

// Payload sizes
#define COLOR_PAYLOAD_SIZE 7          // 1 byte type + 2 bytes R + 2 bytes G + 2 bytes B
#define MAX_MESSAGE_SIZE 255          // Maximum message size for mesh

// ============================================================================
// DEBUG CONFIGURATION
// ============================================================================

#define DEBUG_ENABLED 1               // Enable serial debug output
#define BAUD_RATE 115200              // Serial communication speed

#if DEBUG_ENABLED
  #define DEBUG_PRINT(x) Serial.print(x)
  #define DEBUG_PRINTLN(x) Serial.println(x)
  #define DEBUG_PRINTF(fmt, ...) Serial.printf(fmt, __VA_ARGS__)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
  #define DEBUG_PRINTF(fmt, ...)
#endif

#endif // CONFIG_H
