/**
* Author: Rian
* Date: 2026/5/9.
*/

#include "DBUS.h"

#define REMOTE_LENGTH 18     // DBUS数据帧长
#define REMOTE_BACK_LENGTH 1 // 增加一个字节保持稳定

enum DBusState { RemoteIdle, RemoteWorking };
typedef enum { OFF, ON } ButtonState;

typedef struct {
  ButtonState state;
  ButtonState laststate;
  ButtonState isPressed;
} Button_Type;

typedef struct
{
  union {
    struct {
      int16_t rx, ry, lx, ly;
    };
    struct {
      int16_t ch1, ch2, ch3, ch4;
    };
  };

  enum DBusState state;

  uint8_t switchLeft; // 3 value
  uint8_t switchRight;
}Remote_Type;

Remote_Type remoteHandle = {0};
uint8_t remoteBuffer[REMOTE_LENGTH + REMOTE_BACK_LENGTH] = {0};

static void remote_init()
{
  remoteHandle.state = RemoteIdle;
}
void DBUS_Init()
{
  remote_init();
  HAL_UARTEx_ReceiveToIdle_DMA(&DBUS_UART, remoteBuffer, sizeof(remoteBuffer));
}

static void remote_update()
{
  remoteHandle.state = RemoteWorking;

  remoteHandle.ch1 = (remoteBuffer[0] | remoteBuffer[1] << 8) & 0x07FF;
  remoteHandle.ch1 -= 1024;
  remoteHandle.ch2 = (remoteBuffer[1] >> 3 | remoteBuffer[2] << 5) & 0x07FF;
  remoteHandle.ch2 -= 1024;
  remoteHandle.ch3 = (remoteBuffer[2] >> 6 | remoteBuffer[3] << 2 | remoteBuffer[4] << 10) & 0x07FF;
  remoteHandle.ch3 -= 1024;
  remoteHandle.ch4 = (remoteBuffer[4] >> 1 | remoteBuffer[5] << 7) & 0x07FF;
  remoteHandle.ch4 -= 1024;

  remoteHandle.switchLeft  = ((remoteBuffer[5] >> 4) & 0x000C) >> 2;
  remoteHandle.switchRight = (remoteBuffer[5] >> 4) & 0x0003;
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
  if (huart->Instance == DBUS_UART.Instance)
  {
    if (Size == REMOTE_LENGTH) remote_update();
    HAL_UARTEx_ReceiveToIdle_DMA(huart, huart->pRxBuffPtr, (REMOTE_LENGTH + REMOTE_BACK_LENGTH));
  }
}