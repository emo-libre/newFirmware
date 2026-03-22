# EMO Hardware — ESP32 GPIO Map

All pins confirmed via Ghidra RE of the original firmware (`alexa.bin.c` + `ghidraExtracted-elfPartition1.c`),
cross-verified with UART scan on physical hardware (2026-03-22).

---

## Quick Reference Table

| GPIO | Direction | Function | Protocol | Notes |
|------|-----------|----------|----------|-------|
| **4**  | IN  | K210 TX → ESP32 RX | UART1 1 Mbaud | Confirmed by scan |
| **5**  | OUT | Left Foot servo    | RMT custom | Smart servo signal |
| **12** | OUT | ESP32 TX → K210 RX | UART1 1 Mbaud | — |
| **21** | OUT | Left Legs servo    | RMT custom | Smart servo signal |
| **22** | OUT | K210 RESETN        | GPIO | Active-LOW, pulse to reset |
| **23** | OUT | Battery / Power circuit enable | GPIO | HIGH = system powered on |
| **25** | OUT | Right Legs servo   | RMT custom | Smart servo signal |
| **26** | OUT | Right Foot servo   | RMT custom | Smart servo signal |

---

## Block Diagram

```
                        ┌─────────────────────────────────────┐
                        │            ESP32 (WROOM-32)         │
                        │                                     │
   ┌──────────────┐     │  GPIO21 ──────────────────────────► │ Left Legs servo (LL)
   │              │     │  GPIO5  ──────────────────────────► │ Left Foot servo (LF)
   │   SERVOS     │ ◄───│  GPIO25 ──────────────────────────► │ Right Legs servo (RL)
   │  (RMT bus)   │     │  GPIO26 ──────────────────────────► │ Right Foot servo (RF)
   │              │     │                                     │
   └──────────────┘     │         RMT protocol (custom)       │
                        │         Smart servo, 3-wire         │
                        │         VCC / GND / Signal          │
                        │                                     │
   ┌──────────────┐     │  GPIO4  ◄─────────────────────────  │ ← K210 TX (data in)
   │              │     │  GPIO12 ──────────────────────────► │ → K210 RX (data out)
   │    K210      │     │                                     │
   │  (AI face    │     │         UART1, 1 Mbaud, 8N1         │
   │   camera)    │     │         No flow control             │
   │              │     │                                     │
   │   RESETN ◄───┼─────│  GPIO22 ──────────────────────────► │ K210 reset
   │              │     │         Active-LOW pulse            │
   └──────────────┘     │                                     │
                        │  GPIO23 ──────────────────────────► │─────────────────────┐
                        │         HIGH = on                   │                     │
                        └─────────────────────────────────────┘            ┌────────┴───────┐
                                                                           │    Battery /   │
                                                                           │  Power circuit │
                                                                           │  (PWR_EN pin)  │
                                                                           └────────────────┘
```

---

## Servo Details

Protocol: ESP32 **RMT** peripheral → custom half-duplex pulse-width encoding
Each servo contains a **PUYA PY32F002A** MCU (ARM Cortex-M0+) inside.
Connection: 3-wire — VCC / GND / Signal (single wire, half-duplex).

### RMT Pulse Timing

| Symbol       | HIGH    | LOW    |
|--------------|---------|--------|
| Start pulse  | 256 µs  | 100 µs |
| Bit = 1      | 144 µs  | 100 µs |
| Bit = 0      |  72 µs  | 100 µs |

- Clock: `clk_div=80` → 1 µs/tick at 80 MHz APB
- Byte order: **LSB first**
- Packet: 6 bytes → 49 RMT items (1 header + 48 bits)


### Angle Encoding

```
raw = (degree × 4 + 1380) / 3
```

| Degrees | Raw value |
|---------|-----------|
| 0°      | 460       |
| 90°     | 580 (neutral) |
| 180°    | 700       |

---

## K210 Details

### UART Protocol (confirmed from RE + scan)

| Parameter   | Value        |
|-------------|--------------|
| Port        | UART1        |
| TX (ESP→K210) | GPIO 12    |
| RX (K210→ESP) | GPIO 4     |
| Baud rate   | **1,000,000** |
| Frame       | 8N1          |
| Flow ctrl   | None         |
| RX buffer   | 1024 bytes   |


### Hardware Control Pins

| Pin    | GPIO | Active | Function |
|--------|------|--------|----------|
| RESETN | 22   | LOW    | Pulse LOW to reset K210. Normal boot = 1 ms pulse. Hard reset = 100 ms pulse. |
| PWR_EN | 23   | HIGH   | Connected to **battery / power management circuit** (not directly to K210). Drive HIGH to enable system power. Drive LOW to shut down. Exact IC unknown.

### Optimal Boot Sequence (from RE of original firmware)

```
1.  UART init (GPIO12 TX, GPIO4 RX, 1 Mbaud)
2.  Send cmd 0x34 via UART  ← software reset command
3.  Wait 500 ms
7.  GPIO23 HIGH  ← battery/power circuit enable


## Notes

- **GPIO 23** controls a **battery/power management circuit** Also shared with I2S WS (audio mode 0); audio and power sequencing run in separate contexts.
- **GPIO 25 / 26** are shared between Right Legs/Foot servos and I2S BCK (audio mode 0/1). Same multiplexing applies.