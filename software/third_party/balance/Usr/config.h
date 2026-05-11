/**
* Author: Rian
* Date: 2026/5/9.
*/

#ifndef CONFIG_H
#define CONFIG_H

#define IMU_flag (1u << 0)
#define control_flag (1u << 1)
#define init_mask (IMU_flag | control_flag)

#define accel_flag (1u << 0)
#define gyro_flag (1u << 1)
#define imu_mask (accel_flag | gyro_flag)


#define GYROSCOPE_YAW_FILTER_Coefficient 0
#define IMU_INIT_TIME 5000                      // IMU初始化时间
#define fastTriangle 1                          // 快速三角函数计算，仅用于imu解算，且下列三个角度必须为PI/2的倍数
#define angle_x 0                               // 内旋，先绕x轴旋转角度（rad）
#define angle_y 0                               // 内旋，再绕y轴旋转角度（rad）
#define angle_z 0                           // 内旋，最后绕z轴旋转角度（rad）
#if fastTriangle
  #define f_cos(para) (pow(-1, ((int)((para + PI/2)/PI*2))/2)*abs((int)((para + PI/2)/PI*2)%2))
  #define f_sin(para) (pow(-1, ((int)(para/PI*2))/2)*abs((int)(para/PI*2)%2))
#else
  #define f_cos(para) cos(para)
  #define f_sin(para) sin(para)
#endif
#define trans_matrix {f_cos(angle_y)*f_cos(angle_z), f_cos(angle_x)*f_sin(angle_z) + f_cos(angle_z)*f_sin(angle_x)*f_sin(angle_y), f_sin(angle_x)*f_sin(angle_z) - f_cos(angle_x)*f_cos(angle_z)*f_sin(angle_y), \
-f_cos(angle_y)*f_sin(angle_z), f_cos(angle_x)*f_cos(angle_z) - f_sin(angle_x)*f_sin(angle_y)*f_sin(angle_z), f_cos(angle_z)*f_sin(angle_x) + f_cos(angle_x)*f_sin(angle_y)*f_sin(angle_z),  \
f_sin(angle_y), -f_cos(angle_y)*f_sin(angle_x), f_cos(angle_x)*f_cos(angle_y)}

#endif //CONFIG_H
