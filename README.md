# ESP32 Mesh LED Controller

## Project Overview
ESP32-based mesh network system for synchronized LED ring control using WS2812B addressable LEDs.

### Hardware
- **Microcontroller**: ESP32
- **LED**: WS2812B RGB LED Ring (16 LEDs)
- **Communication**: WiFi Mesh Network (painlessMesh)

### Features
- Master-client mesh network topology
- Synchronized color updates across all nodes
- Real-time LED control via mesh broadcast
- Hardware abstraction for testability

## Project Structure

```
ardproject/
├── include/           # Header files
│   └── config.h       # Hardware and network configuration
├── src/               # Source code
│   └── main.cpp       # Main application entry point
├── lib/               # Custom libraries
├── test/              # Unit and integration tests
├── docs/              # Documentation
├── schematics/        # Hardware schematics and diagrams
├── data/              # SPIFFS/LittleFS filesystem data
├── backup/            # Configuration and firmware backups
└── platformio.ini     # PlatformIO configuration
```

## Building the Project

### Prerequisites
- PlatformIO Core or PlatformIO IDE
- ESP32 board support package
- USB drivers for ESP32

### Compile
```bash
pio run
```

### Upload to ESP32
```bash
pio run --target upload
```

### Monitor Serial Output
```bash
pio device monitor
```

### Debug Build
```bash
pio run -e debug
```

## Configuration

### Network Settings
Edit `include/config.h` to customize:
- Mesh network SSID and password
- Node type (master/client)
- Communication port

### LED Settings
- GPIO pin assignment
- LED count per ring
- Brightness level

### Timing Parameters
- Color broadcast interval
- Mesh initialization timeout
- LED update timeout

## Development Workflow

1. **Modify Configuration**: Edit `include/config.h`
2. **Write Code**: Update `src/main.cpp` or add modules
3. **Build**: `pio run`
4. **Test**: Run unit tests with `pio test`
5. **Upload**: Flash firmware with `pio run --target upload`
6. **Monitor**: Watch serial output with `pio device monitor`

## Memory Considerations
- **Flash**: ~1.4MB available for code (ESP32 has 4MB)
- **SRAM**: ~520KB available (ESP32 has ~520KB total)
- **PSRAM**: Not used in this project
- Monitor memory usage: `pio run --target size`

## Testing

### Unit Tests
```bash
pio test
```

### Simulation
Simulation configuration files are available in `simulation/` directory for Wokwi.

## Troubleshooting

### Common Issues
1. **Upload fails**: Check USB cable, verify ESP32 is in bootloader mode
2. **Mesh won't connect**: Verify SSID/password match across all nodes
3. **LEDs not working**: Check power supply, data pin connection, LED count

### Debug Mode
Use the debug environment for verbose serial output:
```bash
pio run -e debug --target upload
pio device monitor
```

## License
[Specify your license here]

## Contributors
[Your name/team]

## Version History
- **v0.1.0** - Initial project structure setup
