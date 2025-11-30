/**
 * ESP32 Mesh LED Controller - Main Firmware
 * 
 * This firmware implements a mesh network system for synchronized LED control.
 * - Master node generates color patterns and broadcasts to all clients
 * - Client nodes receive color updates and display them on local LED rings
 * 
 * Hardware: ESP32 + WS2812B LED Ring (16 LEDs)
 * Network: painlessMesh WiFi mesh
 * 
 * Author: [Your Name]
 * Version: 1.0.0
 * Date: 2025-11-29
 */

#include <Arduino.h>
#include <painlessMesh.h>
#include <FastLED.h>
#include "config.h"

// ============================================================================
// GLOBAL OBJECTS
// ============================================================================

// Mesh network object
painlessMesh mesh;

// LED array
CRGB leds[LED_COUNT];

// Task scheduler (built into painlessMesh)
Scheduler userScheduler;

// ============================================================================
// GLOBAL VARIABLES
// ============================================================================

// Node configuration (set this to NODE_TYPE_MASTER for master node)
const uint8_t nodeType = NODE_TYPE_CLIENT; // Change to NODE_TYPE_MASTER for master

// Timing variables
unsigned long lastColorBroadcast = 0;
unsigned long lastLedUpdate = 0;

// Current color state
CRGB currentColor = CRGB::Black;

// Node statistics
uint32_t receivedMessages = 0;
uint32_t sentMessages = 0;

// ============================================================================
// FORWARD DECLARATIONS
// ============================================================================

void sendColorUpdate(CRGB color);
void receivedCallback(uint32_t from, String &msg);
void newConnectionCallback(uint32_t nodeId);
void changedConnectionCallback();
void nodeTimeAdjustedCallback(int32_t offset);
void generateNewColor();
void updateLEDs(CRGB color);
void taskBroadcastColor();

// ============================================================================
// TASK DEFINITIONS (for painlessMesh scheduler)
// ============================================================================

Task taskBroadcastColorTask(COLOR_BROADCAST_INTERVAL, TASK_FOREVER, &taskBroadcastColor);

// ============================================================================
// COLOR GENERATION (Master Node)
// ============================================================================

/**
 * Generate a new color using HSV color wheel
 * Creates smooth color transitions for visual effect
 */
void generateNewColor() {
    static uint8_t hue = 0;
    
    // Increment hue for rainbow effect
    hue += 5; // Adjust speed of color change
    
    // Convert HSV to RGB (saturation=255, value=LED_BRIGHTNESS)
    currentColor = CHSV(hue, 255, LED_BRIGHTNESS);
    
    DEBUG_PRINTF("Generated new color: R=%d G=%d B=%d (Hue=%d)\n", 
                 currentColor.r, currentColor.g, currentColor.b, hue);
}

// ============================================================================
// MESH NETWORK FUNCTIONS
// ============================================================================

/**
 * Send color update message to all nodes in mesh
 * Message format: [MSG_TYPE][R][G][B]
 */
void sendColorUpdate(CRGB color) {
    // Build JSON message
    String msg = "{\"type\":";
    msg += MSG_TYPE_COLOR_UPDATE;
    msg += ",\"r\":";
    msg += color.r;
    msg += ",\"g\":";
    msg += color.g;
    msg += ",\"b\":";
    msg += color.b;
    msg += "}";
    
    // Broadcast to all nodes
    mesh.sendBroadcast(msg);
    sentMessages++;
    
    DEBUG_PRINTF("Broadcast color: R=%d G=%d B=%d (msg #%u)\n", 
                 color.r, color.g, color.b, sentMessages);
}

/**
 * Callback when message is received from mesh network
 */
void receivedCallback(uint32_t from, String &msg) {
    receivedMessages++;
    
    DEBUG_PRINTF("Received message from %u: %s\n", from, msg.c_str());
    
    // Parse JSON message
    // Simple parsing (for production, consider ArduinoJson library)
    int typeStart = msg.indexOf("\"type\":") + 7;
    int typeEnd = msg.indexOf(",", typeStart);
    int msgType = msg.substring(typeStart, typeEnd).toInt();
    
    // Handle color update messages
    if (msgType == MSG_TYPE_COLOR_UPDATE) {
        // Extract RGB values
        int rStart = msg.indexOf("\"r\":") + 4;
        int rEnd = msg.indexOf(",", rStart);
        int r = msg.substring(rStart, rEnd).toInt();
        
        int gStart = msg.indexOf("\"g\":") + 4;
        int gEnd = msg.indexOf(",", gStart);
        int g = msg.substring(gStart, gEnd).toInt();
        
        int bStart = msg.indexOf("\"b\":") + 4;
        int bEnd = msg.indexOf("}", bStart);
        int b = msg.substring(bStart, bEnd).toInt();
        
        // Update LEDs with received color
        CRGB newColor = CRGB(r, g, b);
        updateLEDs(newColor);
        
        DEBUG_PRINTF("Applied color: R=%d G=%d B=%d\n", r, g, b);
    }
}

/**
 * Callback when new node joins the mesh
 */
void newConnectionCallback(uint32_t nodeId) {
    DEBUG_PRINTF("New Connection, nodeId = %u\n", nodeId);
    DEBUG_PRINTF("Total nodes in mesh: %d\n", mesh.getNodeList().size() + 1); // +1 for this node
}

/**
 * Callback when mesh topology changes
 */
void changedConnectionCallback() {
    DEBUG_PRINTLN("Mesh topology changed");
    
    // Get current node list
    auto nodes = mesh.getNodeList();
    DEBUG_PRINTF("Connected nodes: %d\n", nodes.size());
    
    // Print node IDs
    for (auto node : nodes) {
        DEBUG_PRINTF("  - Node ID: %u\n", node);
    }
}

/**
 * Callback when mesh time is adjusted (for synchronization)
 */
void nodeTimeAdjustedCallback(int32_t offset) {
    DEBUG_PRINTF("Adjusted time by %d us\n", offset);
}

// ============================================================================
// LED CONTROL FUNCTIONS
// ============================================================================

/**
 * Update LED ring with new color
 * Implements smooth transition and hardware abstraction
 */
void updateLEDs(CRGB color) {
    // Set all LEDs to the same color
    fill_solid(leds, LED_COUNT, color);
    
    // Update hardware
    FastLED.show();
    
    // Store current color
    currentColor = color;
    lastLedUpdate = millis();
    
    // Optional: Add fade effect
    // FastLED.setBrightness(LED_BRIGHTNESS);
}

/**
 * Initialize LED subsystem
 */
void initLEDs() {
    DEBUG_PRINTLN("Initializing LED subsystem...");
    
    // Configure FastLED
    FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, LED_COUNT);
    FastLED.setBrightness(LED_BRIGHTNESS);
    
    // Initial state: all LEDs off
    fill_solid(leds, LED_COUNT, CRGB::Black);
    FastLED.show();
    
    // Boot animation: quick flash to show hardware is working
    for (int i = 0; i < 3; i++) {
        fill_solid(leds, LED_COUNT, CRGB::Blue);
        FastLED.show();
        delay(100);
        fill_solid(leds, LED_COUNT, CRGB::Black);
        FastLED.show();
        delay(100);
    }
    
    DEBUG_PRINTLN("LED subsystem initialized");
}

// ============================================================================
// TASK FUNCTIONS (for scheduler)
// ============================================================================

/**
 * Task: Broadcast new color (Master node only)
 * Runs every COLOR_BROADCAST_INTERVAL milliseconds
 */
void taskBroadcastColor() {
    if (nodeType == NODE_TYPE_MASTER) {
        // Generate new color
        generateNewColor();
        
        // Update local LEDs
        updateLEDs(currentColor);
        
        // Broadcast to mesh
        sendColorUpdate(currentColor);
    }
}

// ============================================================================
// SETUP FUNCTION
// ============================================================================

void setup() {
    // Initialize serial communication
    Serial.begin(BAUD_RATE);
    delay(1000); // Wait for serial to stabilize
    
    DEBUG_PRINTLN("\n\n========================================");
    DEBUG_PRINTLN("ESP32 Mesh LED Controller");
    DEBUG_PRINTLN("========================================");
    DEBUG_PRINT("Firmware Version: 1.0.0\n");
    DEBUG_PRINT("Node Type: ");
    DEBUG_PRINTLN(nodeType == NODE_TYPE_MASTER ? "MASTER" : "CLIENT");
    DEBUG_PRINTF("Chip ID: 0x%08X\n", (uint32_t)ESP.getEfuseMac());
    DEBUG_PRINTLN("========================================\n");
    
    // Initialize LED hardware
    initLEDs();
    
    // Configure mesh network
    DEBUG_PRINTLN("Initializing mesh network...");
    
    // Set debug message types (comment out for production)
    // mesh.setDebugMsgTypes(ERROR | STARTUP | CONNECTION);
    
    // Initialize mesh
    mesh.init(MESH_SSID, MESH_PASSWORD, &userScheduler, MESH_PORT);
    
    // Set callbacks
    mesh.onReceive(&receivedCallback);
    mesh.onNewConnection(&newConnectionCallback);
    mesh.onChangedConnections(&changedConnectionCallback);
    mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
    
    DEBUG_PRINTF("Mesh initialized - SSID: %s, Port: %d\n", MESH_SSID, MESH_PORT);
    DEBUG_PRINTF("This Node ID: %u\n", mesh.getNodeId());
    
    // Master node: start color broadcast task
    if (nodeType == NODE_TYPE_MASTER) {
        DEBUG_PRINTLN("Starting master node tasks...");
        userScheduler.addTask(taskBroadcastColorTask);
        taskBroadcastColorTask.enable();
        DEBUG_PRINTF("Color broadcast task enabled (interval: %d ms)\n", COLOR_BROADCAST_INTERVAL);
    } else {
        DEBUG_PRINTLN("Client node ready - waiting for color updates...");
    }
    
    DEBUG_PRINTLN("\nSetup complete. Entering main loop...\n");
}

// ============================================================================
// MAIN LOOP
// ============================================================================

void loop() {
    // Update mesh network (handles all mesh communication)
    mesh.update();
    
    // Watchdog: Check if LED updates have stalled (client nodes)
    if (nodeType == NODE_TYPE_CLIENT) {
        unsigned long timeSinceUpdate = millis() - lastLedUpdate;
        if (timeSinceUpdate > LED_UPDATE_TIMEOUT && lastLedUpdate > 0) {
            DEBUG_PRINTLN("WARNING: No LED updates received - possible network issue");
            // Optional: Set LEDs to error state (e.g., red flash)
            // updateLEDs(CRGB::Red);
        }
    }
    
    // Periodic status output (every 30 seconds)
    static unsigned long lastStatusPrint = 0;
    if (millis() - lastStatusPrint > 30000) {
        lastStatusPrint = millis();
        
        DEBUG_PRINTLN("\n--- Status Report ---");
        DEBUG_PRINTF("Uptime: %lu seconds\n", millis() / 1000);
        DEBUG_PRINTF("Node Type: %s\n", nodeType == NODE_TYPE_MASTER ? "MASTER" : "CLIENT");
        DEBUG_PRINTF("Node ID: %u\n", mesh.getNodeId());
        DEBUG_PRINTF("Connected Nodes: %d\n", mesh.getNodeList().size());
        DEBUG_PRINTF("Messages Sent: %u\n", sentMessages);
        DEBUG_PRINTF("Messages Received: %u\n", receivedMessages);
        DEBUG_PRINTF("Current Color: R=%d G=%d B=%d\n", 
                     currentColor.r, currentColor.g, currentColor.b);
        DEBUG_PRINTF("Free Heap: %u bytes\n", ESP.getFreeHeap());
        DEBUG_PRINTLN("--------------------\n");
    }
    
    // Small delay to prevent watchdog timer issues
    // painlessMesh handles most timing, but this ensures stability
    delay(10);
}
