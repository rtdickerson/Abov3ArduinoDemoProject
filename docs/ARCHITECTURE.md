# Architecture Documentation

## System Architecture

### Overview
This project implements a distributed mesh network system where multiple ESP32 nodes communicate wirelessly to synchronize LED displays.

### Network Topology

```
Master Node (ID: 1)
    |
    ├── painlessMesh WiFi Network
    |
    ├─── Client Node 1
    ├─── Client Node 2
    ├─── Client Node 3
    └─── Client Node N
```

### Component Architecture

#### 1. Hardware Layer
- **ESP32**: WiFi-enabled microcontroller
  - Dual-core Xtensa LX6 @ 240MHz
  - 520KB SRAM
  - 4MB Flash
  - Built-in WiFi 802.11 b/g/n
  
- **WS2812B LED Ring**: Addressable RGB LEDs
  - 16 LEDs per ring
  - 5V power requirement
  - Data line on GPIO 5

#### 2. Communication Layer (painlessMesh)
- **Protocol**: Custom mesh over WiFi
- **Message Types**:
  - `MSG_TYPE_COLOR_UPDATE (1)`: Master broadcasts new color
  - `MSG_TYPE_HEARTBEAT (2)`: Keep-alive messages
  - `MSG_TYPE_NODE_DISCOVERY (3)`: Node discovery requests

#### 3. Application Layer
- **Master Node Responsibilities**:
  - Generate color patterns
  - Broadcast updates every 2 seconds
  - Manage mesh network
  
- **Client Node Responsibilities**:
  - Listen for color updates
  - Apply received colors to local LED ring
  - Maintain mesh connection

### Data Flow

```
Master Node:
  [Color Generator] → [Mesh Broadcast] → [Network]
  
Client Nodes:
  [Network] → [Mesh Receiver] → [LED Controller] → [WS2812B]
```

### Memory Map

#### Flash Layout (Approximate)
```
0x00000000 - 0x00010000 : Bootloader
0x00010000 - 0x00020000 : Partition Table
0x00020000 - 0x00320000 : Application (3MB)
0x00320000 - 0x00400000 : SPIFFS/LittleFS (896KB)
```

#### SRAM Usage Considerations
- **Stack**: ~8KB per task (FreeRTOS)
- **Heap**: Dynamic allocation for mesh, buffers
- **Static**: Global variables, LED buffer (16 LEDs × 3 bytes = 48 bytes)
- **Reserved**: WiFi/Bluetooth stack (~50KB)

### Timing Diagram

```
Time (ms)    Master Node              Client Nodes
─────────────────────────────────────────────────────
0            [Boot & Init]            [Boot & Init]
             [Connect to Mesh]        [Connect to Mesh]
10000        [Mesh Established]       [Mesh Established]
12000        [Generate Color]         [Waiting]
             [Broadcast Color]    ──► [Receive Color]
                                      [Update LEDs]
14000        [Generate Color]         [Waiting]
             [Broadcast Color]    ──► [Receive Color]
                                      [Update LEDs]
...          (repeats every 2s)       (listens)
```

### State Machine

#### Master Node States
1. **INIT**: Power-on, initialize peripherals
2. **MESH_CONNECT**: Establish mesh network
3. **IDLE**: Wait for broadcast interval
4. **BROADCAST**: Send color update to all nodes
5. **ERROR**: Handle network or hardware failures

#### Client Node States
1. **INIT**: Power-on, initialize peripherals
2. **MESH_CONNECT**: Join mesh network
3. **LISTENING**: Wait for color updates
4. **UPDATE_LED**: Apply received color
5. **ERROR**: Handle network or hardware failures

### Error Handling

#### Network Failures
- **Mesh disconnection**: Automatic reconnection via painlessMesh
- **Message loss**: No ACK required (broadcast model, updates are frequent)
- **Timeout**: LED_UPDATE_TIMEOUT ensures client LEDs don't hang

#### Hardware Failures
- **LED communication**: FastLED library handles WS2812B protocol
- **Power issues**: Monitor WiFi stability (high current draw affects WiFi)

### Performance Characteristics

#### Latency
- **Mesh propagation**: 50-200ms typical
- **LED update**: <10ms for 16 LEDs
- **Total update latency**: <250ms master to client LED change

#### Throughput
- **Color updates**: Every 2 seconds (configurable)
- **Payload size**: 7 bytes per color message
- **Network overhead**: ~100 bytes per message (mesh headers)

### Scalability

#### Current Limitations
- **Max nodes**: ~10-15 (painlessMesh practical limit)
- **Update rate**: 2-second interval (reduces mesh congestion)
- **Range**: ~30-50m line-of-sight per hop

#### Optimization Strategies
- Reduce `COLOR_BROADCAST_INTERVAL` for faster updates (increases network load)
- Implement message batching for multiple LED rings per node
- Use WiFi channel selection to avoid interference

### Future Enhancements
1. **Bi-directional communication**: Clients report status to master
2. **Pattern library**: Pre-programmed LED animations
3. **OTA updates**: Remote firmware updates via mesh
4. **Power management**: Sleep modes for battery operation
5. **Sensor integration**: Environmental triggers for LED patterns
