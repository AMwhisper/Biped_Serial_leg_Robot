# DJI NDJ6 DBus Control

## Wiring

Connect the DJI receiver DBus/SBUS output to one STM32 USART RX pin.

Typical DBus serial settings:

- Baudrate: `100000`
- Data bits: `8`
- Parity: even
- Stop bits: `1`
- Frame length: `18` bytes

Some receivers output inverted SBUS-level signals. If the UART cannot decode frames, add a hardware inverter or use a UART/peripheral path that supports RX inversion.

## Control Mapping

Current mapping in `dbus_receiver.c`:

| Remote input | Robot command |
| --- | --- |
| Right switch up/mid | Enable chassis |
| Right switch down or signal lost | Disable all motors |
| CH1, right stick horizontal | Yaw rate |
| CH2, right stick vertical | Forward velocity |
| CH3 | Leg length target |

## Firmware Flow

1. USART ISR/DMA receives DBus bytes.
2. Feed each byte into:

```c
uart_rx_isr_handler(byte);
```

3. `main.c` drains UART bytes and calls:

```c
dbus_rx_byte(byte);
```

4. Parsed commands are passed into:

```c
wheel_leg_control_set_command(&command);
```

For a CubeMX HAL USART project, use RX DMA or idle-line reception. In the callback, push all received bytes into `uart_rx_isr_handler()`.
