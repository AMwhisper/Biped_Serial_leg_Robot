# Motor Mapping

Current hardware assignment:

| Logical role | Physical role | Model | Motor ID | Command CAN ID | Feedback CAN ID | Positive torque direction |
| --- | --- | --- | --- | --- | --- | --- |
| Left hip joint | Left thigh | DM4310 | `0x01` | `0x01` | motor feedback frame / payload ID `0x01` | TBD |
| Left knee joint | Left leg-length/lower-leg joint | DM4310 | `0x02` | `0x02` | motor feedback frame / payload ID `0x02` | TBD |
| Right hip joint | Right thigh | DM4310 | `0x03` | `0x03` | motor feedback frame / payload ID `0x03` | TBD |
| Right knee joint | Right leg-length/lower-leg joint | DM4310 | `0x04` | `0x04` | motor feedback frame / payload ID `0x04` | TBD |
| Left drive wheel | Left wheel | DM6215 | `0x05` | `0x05` | motor feedback frame / payload ID `0x05` | Forward |
| Right drive wheel | Right wheel | DM6215 | `0x06` | `0x06` | motor feedback frame / payload ID `0x06` | Backward |

Code locations:

- `../../config/motor_config.h`: motor model IDs, limits, total count
- `../../config/hardware_config.h`: logical motor mapping
- `../src/motor_controller.c`: assigns model type by motor ID
- `../src/leg_controller.c`: maps joints and wheels to controllers

The first four motors are joints and are limited by `DM4310_*`. The last two motors are drive wheels and are limited by `DM6215_*`.

CAN2 is wired on PB5/PB6 in this project. Damiaotech MIT command frames use the motor ID directly as the standard CAN ID.

Wheel sign convention for high-level control:

- Positive chassis forward torque should spin both wheels forward.
- Motor `0x05` positive torque already spins the left wheel forward.
- Motor `0x06` positive torque spins the right wheel backward, so software must invert the right wheel torque command.
