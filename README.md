# NYMD_DAP: A CMSIS-DAP based debug probe

A CMSIS-DAP based debugger that I made as my thesis work.

## Software 
The software implements both the v1 HID interface and the v2 interface with bulk endpoints + WinUSB.

You can switch between them in dap_config.h with `#define DAP_FW_V1`

Software components used:
- ST USB Device library
- ST HAL
- CMSIS-DAP

## Hardware
Custom board based on STM32F446, with:
- HS USB interface
- USB Type-C connector
- small(ish) footprint
- 10-pin debug connector
- SWO support
- Additional UART
