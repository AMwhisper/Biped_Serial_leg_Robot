# HAL/CAN Bring-up Steps

The current firmware now includes a minimal STM32F4 HAL-compatible board layer for CAN2. It provides `hcan2`, CAN2 GPIO setup, CAN2 RX0 IRQ glue, and the small HAL subset used by `can_driver.c`.

## 1. Generate Or Import Board Support

For production bring-up, you can replace the local minimal board layer with CubeMX or an official RoboMaster C Type example. Add these files to the Keil project:

- `startup_stm32f407xx.s`
- `system_stm32f4xx.c`
- `stm32f4xx_hal.c`
- `stm32f4xx_hal_cortex.c`
- `stm32f4xx_hal_gpio.c`
- `stm32f4xx_hal_can.c`
- `stm32f4xx_hal_rcc.c`
- `stm32f4xx_hal_rcc_ex.c`
- `stm32f4xx_it.c`
- `gpio.c/.h`
- `can.c/.h` with `CAN_HandleTypeDef hcan2`

Then remove or exclude the local minimal files:

- `../board/startup_stm32f407xx.c`
- `../board/system_stm32f4xx.c`
- `../board/stm32f4xx_hal.h`
- `../board/can.c/.h`
- `../board/gpio.c/.h`
- `../board/stm32f4xx_it.c`

## 2. CAN Configuration

Create `hcan2` in `can.c` and configure CAN at 1 Mbps. The local board layer uses CAN2 on PB5/PB6. A common 42 MHz APB1 setup is:

- Prescaler: `3`
- BS1: `CAN_BS1_11TQ`
- BS2: `CAN_BS2_2TQ`
- SJW: `CAN_SJW_1TQ`
- Mode: `CAN_MODE_NORMAL`

Enable `CAN2_RX0_IRQn`.

## 3. Interrupt Glue

CubeMX normally generates this in `stm32f4xx_it.c`:

```c
void CAN1_RX0_IRQHandler(void)
{
    HAL_CAN_IRQHandler(&hcan1);
}
```

For CAN2 use:

```c
void CAN2_RX0_IRQHandler(void)
{
    HAL_CAN_IRQHandler(&hcan2);
}
```

`can_driver.c` already provides a weak `HAL_CAN_RxFifo0MsgPendingCallback()` that pushes frames into the project RX queue, so no extra callback is required unless you want custom processing.

## 4. Time Base

The temporary `HAL_GetTick()` in `../board/system_stm32f4xx.c` is weak. Once HAL is linked, the real HAL tick replaces it. If you keep a custom `SysTick_Handler`, make sure it calls:

```c
HAL_IncTick();
```

`main.c` already does this when `stm32f4xx_hal.h` is present.
