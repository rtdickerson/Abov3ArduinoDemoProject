# Hardware Documentation

## Bill of Materials (BOM)

### Core Components

| Qty | Component | Specification | Notes |
|-----|-----------|---------------|-------|
| 1-N | ESP32 Development Board | ESP32-WROOM-32 | Any variant with at least 4MB flash |
| 1-N | WS2812B LED Ring | 16 LEDs, 5V | NeoPixel compatible |
| 1-N | Power Supply | 5V, 2A+ | Calculate: 16 LEDs × 60mA × 0.7 = ~672mA peak |
| 1-N | Capacitor | 1000µF, 16V electrolytic | Power smoothing for LEDs |
| 1-N | Resistor | 470Ω, 1/4W | Data line protection |
| N | Wire | 22-24 AWG | Power and signal connections |

### Optional Components

| Qty | Component | Purpose |
|-----|-----------|---------|
| 1 | Level Shifter | 3.3V → 5V logic (if LEDs require it) |
| 1 | Enclosure | Weatherproofing/protection |
| 1 | Status LED | Visual network status indicator |

## Wiring Diagram

```
ESP32                           WS2812B Ring
┌─────────────┐                ┌──────────────┐
│             │                │              │
│     GPIO 5  ├────[470Ω]─────►│ DIN          │
│             │                │              │
│         GND ├────────────────┤ GND          │
│             │                │              │
└─────────────┘                │              │
                               │              │
Power Supply (5V, 2A+)         │              │
┌─────────────┐                │              │
│             │                │              │
│          +5V├────┬───────────┤ 5V           │
│             │    │           │              │
│         GND ├────┼───────────┤ GND          │
│             │    │           │              │
└─────────────┘    │           └──────────────┘
                   │
                [1000µF]
                   │
                  GND
```

### Pin Connections

#### ESP32 to WS2812B
- **GPIO 5** → **470Ω resistor** → **DIN** (Data In)
- **GND** → **GND**
- **5V** (from external PSU) → **5V** on LED ring

> **Important**: Do NOT power WS2812B from ESP32's 5V pin. Use external power supply.

#### Power Distribution
- **Power Supply +5V** → **LED Ring +5V** AND **ESP32 VIN/5V**
- **Power Supply GND** → **Common ground** (ESP32 + LED Ring)
- **1000µF capacitor** across power rails near LED ring

## Hardware Setup Instructions

### Step 1: Prepare Components
1. Check ESP32 board for damage
2. Verify WS2812B LED ring (test with simple Arduino sketch)
3. Measure power supply output (should be 4.9-5.2V)

### Step 2: Solder Connections
1. **Data Line**:
   - Solder 470Ω resistor to GPIO 5
   - Connect resistor to DIN pad on LED ring
   
2. **Ground**:
   - Connect ESP32 GND to LED ring GND
   - Ensure solid connection (use thicker wire for ground)
   
3. **Power**:
   - Solder capacitor across LED ring +5V and GND (observe polarity!)
   - Connect external 5V to LED ring +5V
   - Connect external GND to common ground

### Step 3: Power-On Test
1. Connect USB to ESP32 (for serial monitoring)
2. Connect external 5V power supply
3. All LEDs should briefly flash (bootloader test)
4. Upload test sketch to verify LED control

### Step 4: Final Assembly
1. Secure components in enclosure (if using)
2. Strain-relief for all wires
3. Label each node (Master/Client 1/Client 2/etc.)

## Power Calculations

### Per Node Current Draw

| Component | Typical | Peak | Notes |
|-----------|---------|------|-------|
| ESP32 (WiFi active) | 160mA | 260mA | During transmission |
| ESP32 (WiFi idle) | 80mA | 120mA | Mesh listening |
| WS2812B (all white, full brightness) | 960mA | 960mA | 16 LEDs × 60mA |
| WS2812B (typical colors, brightness=200) | 500mA | 700mA | Realistic usage |

### Recommended Power Supply Sizing
- **Single node**: 5V @ 2A (provides margin)
- **Multiple nodes**: Calculate per above, add 20% margin

### Power Supply Quality
- Use **regulated** 5V supply (not USB charger)
- Ripple voltage < 100mV
- Short, thick power wires (voltage drop)

## Physical Considerations

### Heat Dissipation
- ESP32: Minimal heat at normal operation
- LED Ring: Can get warm at full brightness
- Ensure adequate ventilation in enclosure

### Electrical Noise
- Keep LED data wire away from power lines
- Twist power wire pairs (5V + GND)
- Capacitor placement is critical (close to LED ring)

### Mechanical Mounting
- Use standoffs for ESP32 board
- Secure LED ring to prevent wire stress
- Cable management to avoid interference

## Testing and Validation

### Hardware Checkout Procedure

1. **Continuity Test** (power off):
   - Verify GND connections
   - Check for shorts between power rails

2. **Power Test** (no ESP32):
   - Measure 5V rail voltage
   - Check capacitor polarity
   - Verify stable voltage under no load

3. **ESP32 Test** (USB power only):
   - Upload simple blink sketch
   - Verify serial communication @ 115200 baud

4. **LED Test** (external power + ESP32):
   - Upload FastLED test sketch
   - Verify all 16 LEDs light up
   - Test RGB color rendering

5. **Integration Test**:
   - Upload full mesh firmware
   - Verify network connectivity
   - Test synchronized LED updates

### Common Hardware Issues

| Symptom | Likely Cause | Solution |
|---------|--------------|----------|
| LEDs don't light | No power / wrong polarity | Check 5V rail, capacitor polarity |
| First LED works, rest don't | Data signal issue | Check data line resistor, level shifting |
| Flicker/glitches | Power supply noise | Add/improve capacitor, shorten wires |
| ESP32 won't boot | Voltage drop | Separate power for ESP32 and LEDs |
| WiFi unstable | Interference | Keep LED wires short, add ferrite beads |

## Safety Notes

⚠️ **WARNINGS**:
- Never connect/disconnect while powered
- Observe capacitor polarity (explosion risk if reversed)
- Use proper gauge wire for current requirements
- Ensure adequate ventilation for LEDs at high brightness
- Do not power LEDs from ESP32's 3.3V regulator (will damage board)

## Schematic Files

Detailed schematics are available in the `/schematics` directory:
- `esp32_led_mesh_schematic.pdf` - Full circuit diagram
- `pcb_layout.pdf` - PCB layout (if applicable)
- `wiring_diagram.svg` - Editable wiring diagram

## Firmware-Hardware Interface

### GPIO Assignments
- **GPIO 5**: WS2812B Data Out (configurable in `config.h`)
- **GPIO 2**: Built-in LED (optional status indicator)

### Hardware Abstraction
The firmware uses FastLED library for hardware abstraction:
```cpp
#define LED_PIN 5
#define LED_COUNT 16
CRGB leds[LED_COUNT];
FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, LED_COUNT);
```

This allows easy porting to different LED types or GPIO pins.
