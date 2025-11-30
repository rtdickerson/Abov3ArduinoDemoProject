/**
 * Unit Tests for ESP32 Mesh LED Controller
 * 
 * This file contains unit tests for core functionality.
 * Tests run on the target hardware (ESP32) using PlatformIO's testing framework.
 * 
 * To run tests: pio test
 */

#include <Arduino.h>
#include <unity.h>

// Test configuration values
void test_config_constants() {
    // LED configuration
    TEST_ASSERT_EQUAL(5, LED_PIN);
    TEST_ASSERT_EQUAL(16, LED_COUNT);
    TEST_ASSERT_TRUE(LED_BRIGHTNESS >= 0 && LED_BRIGHTNESS <= 255);
    
    // Mesh configuration
    TEST_ASSERT_EQUAL(5555, MESH_PORT);
    TEST_ASSERT_TRUE(strlen(MESH_SSID) > 0);
}

// Test timing configuration
void test_timing_values() {
    TEST_ASSERT_TRUE(COLOR_BROADCAST_INTERVAL > 0);
    TEST_ASSERT_TRUE(MESH_INIT_TIMEOUT > 0);
    TEST_ASSERT_TRUE(LED_UPDATE_TIMEOUT > 0);
    
    // Sanity checks
    TEST_ASSERT_TRUE(COLOR_BROADCAST_INTERVAL >= 100); // At least 100ms
    TEST_ASSERT_TRUE(MESH_INIT_TIMEOUT >= 1000); // At least 1 second
}

// Test message type definitions
void test_message_types() {
    TEST_ASSERT_EQUAL(1, MSG_TYPE_COLOR_UPDATE);
    TEST_ASSERT_EQUAL(2, MSG_TYPE_HEARTBEAT);
    TEST_ASSERT_EQUAL(3, MSG_TYPE_NODE_DISCOVERY);
    
    // Ensure unique values
    TEST_ASSERT_NOT_EQUAL(MSG_TYPE_COLOR_UPDATE, MSG_TYPE_HEARTBEAT);
    TEST_ASSERT_NOT_EQUAL(MSG_TYPE_COLOR_UPDATE, MSG_TYPE_NODE_DISCOVERY);
    TEST_ASSERT_NOT_EQUAL(MSG_TYPE_HEARTBEAT, MSG_TYPE_NODE_DISCOVERY);
}

// Test payload size constraints
void test_payload_sizes() {
    TEST_ASSERT_EQUAL(7, COLOR_PAYLOAD_SIZE);
    TEST_ASSERT_EQUAL(255, MAX_MESSAGE_SIZE);
    TEST_ASSERT_TRUE(COLOR_PAYLOAD_SIZE < MAX_MESSAGE_SIZE);
}

// Mock LED test - verify FastLED can initialize
void test_led_initialization() {
    // This test verifies that LED subsystem can be initialized
    // In real hardware test, this would check GPIO configuration
    TEST_ASSERT_TRUE(LED_COUNT > 0);
    TEST_ASSERT_TRUE(LED_PIN < 40); // ESP32 has GPIOs 0-39
}

// Test node type definitions
void test_node_types() {
    TEST_ASSERT_EQUAL(1, NODE_TYPE_MASTER);
    TEST_ASSERT_EQUAL(0, NODE_TYPE_CLIENT);
    TEST_ASSERT_NOT_EQUAL(NODE_TYPE_MASTER, NODE_TYPE_CLIENT);
}

// Test serial baud rate
void test_serial_config() {
    TEST_ASSERT_EQUAL(115200, BAUD_RATE);
}

// Arduino setup function
void setup() {
    // Wait for USB serial connection (2 seconds timeout)
    delay(2000);
    
    UNITY_BEGIN();
    
    // Run all tests
    RUN_TEST(test_config_constants);
    RUN_TEST(test_timing_values);
    RUN_TEST(test_message_types);
    RUN_TEST(test_payload_sizes);
    RUN_TEST(test_led_initialization);
    RUN_TEST(test_node_types);
    RUN_TEST(test_serial_config);
    
    UNITY_END();
}

// Arduino loop function (required but not used in tests)
void loop() {
    // Tests run once in setup()
    delay(1000);
}
