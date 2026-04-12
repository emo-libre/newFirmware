# EMO K210 UART Commands

This file documents the currently confirmed ESP32 <-> K210 UART link:

## GPIO

| GPIO | Direction | Function | Notes |
|------|-----------|----------|-------|
| 12 | ESP32 -> K210 | UART1 TX | 3.3 V TTL |
| 4 | K210 -> ESP32 | UART1 RX | 3.3 V TTL |
| 22 | ESP32 -> K210 | RESETN | Active LOW |
| 23 | ESP32 -> power rail | POWER_EN | HIGH = powered/enabled |

## UART Parameters

| Field | Value |
|-------|-------|
| Port | UART1 |
| Baud | 1000000 |
| Frame | 8N1 |
| Flow control | none |
| Voltage | 3.3 V TTL |

## Small Command Frame

Format:

```text
55 AA CMD D0 D1 D2 CS
```

Checksum:

```text
CS = (CMD + D0 + D1 + D2 - 1) & 0xFF
```

Equivalent form:

```text
CS = (0x55 + 0xAA + CMD + D0 + D1 + D2) & 0xFF
```

## Extended Payload Frame

Notes:

- `LEN = payload_byte_count + 1`
- the extra `+1` is the trailing `00` terminator after the ASCII payload
- `PAYLOAD_CS` comes after the terminating `00`
- `HDR_CS = sum(55 AA FF LEN_LO LEN_HI SELECTOR) & 0xFF`
- `PAYLOAD_CS` is recovered from capture for known payloads; a generic formula is still not confirmed

## Confirmed Selectors

| Selector | Meaning | Example |
|----------|---------|---------|
| `0x03` | path load | `/spiffs/json/profile.json` |
| `0x02` | animation payload | JSON animation payloads |

## Confirmed Small Commands

| Frame | Meaning | Status |
|-------|---------|--------|
| `55 AA 34 00 00 00 33` | boot/update gate | confirmed |
| `55 AA 69 01 00 00 69` | app ready/status | confirmed |
| `55 AA 2C 00 00 00 2B` | pre-profile step | confirmed from working boot |
| `55 AA 2D 00 00 00 2C` | post-profile step | confirmed from working boot |
| `55 AA 70 00 00 00 6F` | boot stage | confirmed from working boot |
| `55 AA 38 01 00 00 38` | display/state step | confirmed from working boot |
| `55 AA 06 01 00 00 06` | boot stage | confirmed from working boot |
| `55 AA 17 68 00 00 7E` | boot stage | confirmed from working boot |
| `55 AA 6A 46 00 00 AF` | boot stage | confirmed from working boot |
| `55 AA 6C 00 00 00 6B` | boot stage | confirmed from working boot |
| `55 AA 74 00 00 00 73` | boot stage | confirmed from working boot |
| `55 AA 61 00 00 00 60` | voice-wake prelude | confirmed from capture |
| `55 AA 09 00 00 00 08` | voice-wake prelude / state ping | confirmed from capture |
| `55 AA 06 03 10 00 18` | voice-wake follow-up | confirmed from capture |
| `55 AA 06 01 10 00 16` | post-animation state step | observed |
| `55 AA 06 00 10 00 15` | post-animation state step | observed |
| `55 AA 06 00 00 00 05` | post-animation state step | observed |
| `55 AA 65 EB 00 00 4F` | state/error/transition frame | observed, meaning not yet confirmed |

## Working Boot Sequence

Confirmed working sequence now replayed by firmware:

```text
55 AA 34 00 00 00 33
55 AA 69 01 00 00 69
55 AA 69 01 00 00 69
55 AA 69 01 00 00 69
55 AA 2C 00 00 00 2B
55 AA FF 1A 00 03 1B "/spiffs/json/profile.json" 00 10
55 AA 2D 00 00 00 2C
55 AA 70 00 00 00 6F
55 AA 38 01 00 00 38
55 AA 06 01 00 00 06
55 AA 17 68 00 00 7E
55 AA 6A 46 00 00 AF
55 AA 6C 00 00 00 6B
55 AA 74 00 00 00 73
55 AA FF 4C 00 02 4C [0,[["/spiffs/avi/boot_start.avi",[[0,"/spiffs/mp3/boot_start_000.mp3"]]]]] 00 C4
```

## Voice Wake Sequence

Current captured wake sequence:

```text
55 AA 61 00 00 00 60
55 AA 09 00 00 00 08
55 AA FF 52 00 02 52 [0,[["/spiffs/avi/voice_wake_up.avi",[[0,"/spiffs/mp3/voice_wake_up_000.mp3"]]]]] 00 54
55 AA 06 03 10 00 18
```

Observed follow-on animation/state frames after wake:

```text
55 AA FF 26 00 02 26 [0,[["/spiffs/avi/stay_wink_3.avi"]]] 00 81
55 AA FF 23 00 02 23 [0,[["/spiffs/avi/stay_b_c.avi"]]] 00 A2
55 AA FF 26 00 02 26 [0,[["/spiffs/avi/stay_wink_7.avi"]]] 00 92
```

## Known Extended Payloads

### Selector `0x03` path load

| Payload | `LEN_LO` | `HDR_CS` | `PAYLOAD_CS` |
|---------|----------|----------|--------------|
| `/spiffs/json/profile.json` | `0x1A` | `0x1B` | `0x10` |

### Selector `0x02` animation payloads

| Name | Payload form | `LEN_LO` | `HDR_CS` | `PAYLOAD_CS` |
|------|--------------|----------|----------|--------------|
| `boot_start` | AVI + MP3 | `0x4C` | `0x4C` | `0xC4` |
| `voice_wake_up` | AVI + MP3 | `0x52` | `0x52` | `0x54` |
| `dont_understand` | AVI + MP3 | `0x56` | `0x56` | `0x98` |
| `no_network` | AVI + MP3 | `0x4C` | `0x4C` | `0x70` |
| `stay_wink_1` | AVI only | `0x26` | `0x26` | `0x04` |
| `stay_wink_3` | AVI only | `0x26` | `0x26` | `0x81` |
| `stay_wink_4` | AVI only | `0x26` | `0x26` | `0xD9` |
| `stay_wink_7` | AVI only | `0x26` | `0x26` | `0x92` |
| `stay_a_c` | AVI only | `0x23` | `0x23` | `0x1A` |
| `stay_b_c` | AVI only | `0x23` | `0x23` | `0xA2` |

## Known Payload Text Forms

AVI only:

```text
[0,[["/spiffs/avi/stay_wink_1.avi"]]]
```

AVI + MP3:

```text
[0,[["/spiffs/avi/boot_start.avi",[[0,"/spiffs/mp3/boot_start_000.mp3"]]]]]
```

Mode 1 example:

```text
[1,[["/spiffs/avi/listen_and_think.avi",[[0,"/spiffs/mp3/listen_and_think_000.mp3"]]]]]
```

`listen_and_think` is confirmed as a valid animation name in the decompiled firmware, but its
captured `PAYLOAD_CS` byte has not been recovered yet.

## Decompiled Firmware Anchors

Useful references for further RE:

- `"/spiffs/json/profile.json"`:
  - `ghidraExtracted-elfPartition1.c:79534`
  - `ghidraExtracted-elfPartition1.c:823111`
  - `ghidraExtracted-elfPartition1.c:861842`
- animation string table:
  - `k210_fw_decompiled.c:681206` `boot_start`
  - `k210_fw_decompiled.c:681362` `dont_understand`
  - `k210_fw_decompiled.c:682656` `stay_b_c`
  - `k210_fw_decompiled.c:682736` `stay_wink_1`
  - `k210_fw_decompiled.c:682744` `stay_wink_3`
  - `k210_fw_decompiled.c:682748` `stay_wink_4`
  - `k210_fw_decompiled.c:682756` `stay_wink_7`
  - `k210_fw_decompiled.c:682976` `voice_wake_up`
  - `ghidraExtracted-elfPartition1.c:91416` `stay_a_c`
  - `ghidraExtracted-elfPartition1.c:86455` `no_network`
- ESP32 extended payload helpers:
  - `fw's/alexa.bin.c:255412` `FUN_401db848`
  - `fw's/alexa.bin.c:255452` `FUN_401db8f0`
