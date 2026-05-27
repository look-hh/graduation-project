# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Smart greenhouse environmental monitoring and control system based on **STM32F103C8T6** (Cortex-M3, 64KB Flash, 20KB SRAM) running **FreeRTOS V202212.01**. Uses the STM32F10x Standard Peripheral Library (not HAL).

## Build & Flash

- **IDE**: Keil MDK-ARM (uVision5). Open `Project.uvprojx`.
- **Build**: Project → Rebuild all target files (Ctrl+F7).
- **Flash**: Via ST-Link or JLink. Debugger configured as `STLink\ST-LINKIII-KEIL_SWO.dll` in project settings.
- **Preprocessor define**: `USE_STDPERIPH_DRIVER`
- **Include paths**: `.\Start;.\User;.\Library;.\freertos\inc;.\freertos\port;.\freertos\src;.\freertos;.\hardware;.\system`
- **Output**: `Objects\Project.hex` (hex file generated for flashing)
- There is no CI, lint, or test suite — this is bare-metal embedded firmware.

## Architecture: Task & Data Flow

### FreeRTOS Tasks (3 tasks, priorities 2–4)

| Task | Priority | Period | Role |
|------|----------|--------|------|
| `myTask3` | 4 (highest) | Semaphore-driven, poll every 2s | GY39 sensor data acquisition. Waits on `xTask3_Sem` (given from UART3 RX ISR), then parses received frame via `get_gy39_data()`, updates display and Bluetooth cache. Alternates between `GUANGZHAO` (light) and `OTHER` (temp/pressure/humidity/altitude) modes each cycle. |
| `myTask1` | 3 | 2s | Automatic control loop: `Fan_PID_ctrl()` → `WP_auto_ctrl()` → `LED_auto_ctrl()` → `Alarm_auto_ctrl()`. Sends cached sensor data over Bluetooth every 15 seconds via `send_bluetooth_data()`. |
| `myTask2` | 2 | 100ms | User interaction: `process_bluetooth_cmd()` (UART2) + `key_ctrl()` (button debounce + actions). |

### Hardware Pin Map

| Peripheral | Pin | Function |
|------------|-----|----------|
| OLED | PB6(SCL), PB7(SDA) | I2C 128×64 |
| GY39 sensor | PB10(TX), PB11(RX) | UART3, 9600 baud |
| Bluetooth HC-05 | PA2(TX), PA3(RX) | UART2, 9600 baud |
| Debug serial | PA9(TX), PA10(RX) | UART1, 9600 baud |
| Servo (water pump) | PB8 | TIM4_CH3, PWM 50Hz |
| Fan | PB9 | TIM4_CH4, PWM 50Hz (shared TIM4) |
| Soil moisture | PB0 | ADC1_IN8, analog |
| Buzzer | PA8 | GPIO, active high |
| LED0 (buzzer indicator) | PA11 | GPIO |
| LED1 (water pump indicator) | PA12 | GPIO |
| LED2 (fill light) | PA15 | GPIO |
| LED3 (fan indicator) | PB3 | GPIO |
| KEY3 (light toggle) | PA0 | EXTI0 interrupt |
| KEY2 (pump toggle) | PA1 | EXTI1 interrupt |
| KEY1 (fan toggle) | PA6 | EXTI6 interrupt |
| KEY0 (status display) | PA7 | EXTI7 interrupt |

### UART Routing

- **UART1** (PA9/PA10): Debug `printf` output (redirected via `sys.c`)
- **UART2** (PA2/PA3): Bluetooth module — commands in, periodic sensor data out
- **UART3** (PB10/PB11): GY39 multi-sensor module — sends binary frames, ISR gives semaphore to Task3

### Control Algorithms (in `system/show.c`)

- **Fan**: Positional PID with anti-windup. Kp=200.0, Ki=10.0, Kd=50.0, T=2s, deadband ±0.3°C, min PWM=3000. Manual override via `Fan_Manual_Mode` flag.
- **Water pump**: Hysteresis on soil moisture — ON at < threshold, OFF at ≥ threshold+10%. Uses servo angle (0°=closed, 180°=open).
- **Fill light**: Hysteresis on ambient light — ON at < threshold, OFF at ≥ threshold+20 lux. Manual override via `Light_Manual_Mode`.
- **Alarm**: Temp >35°C, temp <5°C, soil <30%, with hysteresis recovery thresholds. Pulsed buzzer (2s on/off). State-change notifications over Bluetooth.

### Key Directory Purposes

- `Start/` — CMSIS Cortex-M3 core, startup assembly (`startup_stm32f10x_md.s`), system init
- `Library/` — STM32F10x Standard Peripheral Library (GPIO, USART, TIM, ADC, etc.)
- `User/` — `main.c` (task creation, peripheral init), `stm32f10x_it.c` (ISRs including EXTI and UART), `stm32f10x_conf.h` (peripheral header includes)
- `system/` — Application-layer drivers: OLED display, GY39 sensor, servo, fan PWM, ADC/soil moisture, key/EXTI, LED, buzzer, Bluetooth command parser, display formatter (`show.c`)
- `freertos/` — FreeRTOS kernel source, Cortex-M3 port, heap_4, config (`FreeRTOSConfig.h`: preemptive, 5 priority levels, 17KB heap, 1000Hz tick)
- `Objects/`, `Listings/` — Build artifacts

### Naming Conventions

- FreeRTOS header includes use PascalCase: `#include "FreeRTOS.h"`, `#include "Task.h"` (not the standard lowercase). This is deliberate — the project has capitalized copies/symlinks.
- Application modules use lowercase filenames (e.g., `led.c`, `key.c`, `show.c`).
- STM32 library files follow `stm32f10x_<peripheral>.c/.h`.

### Bluetooth Command Protocol

Text-based commands over UART2 (9600 baud), newline-terminated. Commands include: `BEEPON`/`BEEPOFF`, `Servo+`/`Servo-`, `FANSET`, `SETLIGHT`, `SETWDLOW`/`SETWDHIGH`, `SETSDLOW`/`SETSDHIGH`, `SETLUXLOW`/`SETLUXHIGH`, `FANAUTOSET`, `WPAUTOSET`, `LIGHTAUTO`, `ALARMAUTO`, `GY39`, `RECVDATA`.

### Important: File Encoding

All source files are **UTF-8**. Several files (key.c, led.c, sys.c, beep.h, sys.h, PWM.c) were converted from GBK to UTF-8. Chinese comments throughout.
