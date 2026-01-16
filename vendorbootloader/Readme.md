# Documentation from spazzymoto on the EEvblog
He disassembled the apk containing the bootloader.

# Firmware Flashing Technical Breakdown (Bootloader V2)

This document details the low-level mechanism used to flash `firmware.bin` to the printer.

## 1. Overview
The flashing process uses a custom "Bootloader V2" protocol over a serial connection. The application acts as the client, sending fixed-size frames to the printer (bootloader).

## 2. Firmware File Preparation
Before transmission, the application processes `firmware.bin`:
- **Skip Header**: The first **512 bytes** of the file are skipped (ignored).
- **Chunking**: The remaining data is split into chunks of **4080 bytes**.

## 3. Protocol Transport Layer
Communication relies on fixed-size frames sent over `ISerialConnection`.

### Frame Structure
**Total Size**: 4106 bytes
**Endianness**: Little Endian (LSB first)

| Offset | Size | Field | Description |
| :--- | :--- | :--- | :--- |
| 0 | 4 | Magic | `ZBOT` (`5A 42 4F 54`) |
| 4 | 1 | Command ID | Opcode (see below) |
| 5 | 1 | Is Response | `0` = Command, `1` = Response |
| 6 | 2 | Data Size | Header payload size (Often `0` in client commands) |
| 8 | 4096 | Payload | Command-specific data |
| 4104 | 2 | CRC | CRC16 checksum of bytes 0-4103 |

### Reliability
- **Timeout**: The client waits up to 6 seconds for a response in some cases (usually faster).
- **Retries**: Up to 7 attempts are made for each command if no valid response (valid CRC + correct opcode) is received.

## 4. Command Opcodes
Defined in `BootloaderV2CommandType`.

| ID | Name | Description |
| :--- | :--- | :--- |
| `0x01` | `CMD_GET_STATE` | Polls bootloader status. |
| `0x02` | `CMD_START_APP` | Jumps to main firmware. |
| `0x03` | `CMD_START_FLASH`| Initiates flashing session. |
| `0x04` | `CMD_FLASH` | Transmits a firmware data chunk. |
| `0x05` | `CMD_REBOOT` | Resets the device. |
| `0x06` | `CMD_END_FLASH` | Finalizes the flashing process. |

## 5. Packet Payloads
Payloads start at offset 8 within the frame.

### `CMD_START_FLASH` (ID 0x03)
| Offset (from 8) | Size | Value |
| :--- | :--- | :--- |
| 0 | 4 | **Firmware Size** (File length - 512) |

### `CMD_FLASH` (ID 0x04)
| Offset (from 8) | Size | Value |
| :--- | :--- | :--- |
| 0 | 2 | **Offset** (Lower 16-bits of absolute byte offset) |
| 2 | 4 | **Chunk Length** (Usually 4080) |
| 6 | 2 | **Chunk Index** (0-based index) |
| 8 | 4080 | **Firmware Data** |

> **Note**: The "Offset" field is only 16-bits. For firmware larger than 64KB, this value wraps around. The bootloader likely uses the **Chunk Index** to determine the correct memory address (`Address = ChunkIndex * 4080`).

## 6. Flashing Sequence
1.  **Handshake**: Client sends `CMD_GET_STATE` until it receives `ZOS_BOOT_STATE_IDLE`.
2.  **Start**: Client sends `CMD_START_FLASH` with total size.
3.  **Transfer Loop**:
    *   Client constructs `CMD_FLASH` frame with 4080 bytes of data.
    *   Sends frame.
    *   Waits for valid ACK response.
    *   Repeats for all chunks.
4.  **End**: Client sends `CMD_END_FLASH`.
5.  **Reset**: Client triggers printer reset via GPIO.

## 7. GPIO Pin Configuration
The application controls specific GPIO pins to manage the printer's state (Reset/Boot Mode). This is handled by `GpioManager` (for Resin printers).

### Pin Mappings
| Name | Pin Number | Function |
| :--- | :--- | :--- |
| `RST` | 165 | Main Reset Pin |
| `GPIOS`| 163 | **Programming Mode** Pin |
| `GPIO6`| 166 | Used for **Resetting** the printer |
| `FRONT`| 104 | Front Panel / Button ? |
| `SLP` | 118 | Sleep ? |

> **Note**: These pin numbers map to the `/sys/class/gpio/gpioXXX` interface in Android.

### Reset Sequence (`resetPrinter`)
1.  **Direction**: Set `GPIO6` (166) to `OUTPUT`.
2.  **Assert**: Set `GPIO6` to `LOW`.
3.  **Wait**: Sleep **1000ms**.
4.  **Deassert**: Set `GPIO6` to `HIGH`.

### Boot Mode Sequence (`setProgrammingMode`)
To enter/exit programming mode (often used before reset/bootloader start):
- **Enter (True)**: Set `GPIOS` (163) to `HIGH`.
- **Exit (False)**: Set `GPIOS` (163) to `LOW`.

*Note: For some printer types (likely non-Resin), `GpioManagerScriptIf` is used instead, which executes shell scripts at `/data/local/gpiomgr/` (e.g., `reboot`, `reseton`).*


### Some more documentation from the scripts

cmdProgModeOn()
{
	echo 0 > $GPIO_BOOT_DATA;
}

cmdProgModeOff()
{
	echo 1 > $GPIO_BOOT_DATA;
}

### Baudrate

```
stty -F /dev/ttyS2 115200
cat /dev/ttyS2
```

```
microcom -p /dev/ttyS2 -s 115200
```