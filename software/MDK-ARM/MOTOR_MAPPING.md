# Motor Mapping

Current hardware assignment:

| Logical role | Model | Motor ID | Command CAN ID | Feedback CAN ID |
| --- | --- | --- | --- | --- |
| Left hip joint | DM4310 | `0x01` | `0x141` | `0x201` |
| Left knee joint | DM4310 | `0x02` | `0x142` | `0x202` |
| Right hip joint | DM4310 | `0x03` | `0x143` | `0x203` |
| Right knee joint | DM4310 | `0x04` | `0x144` | `0x204` |
| Left drive wheel | DM6215 | `0x05` | `0x145` | `0x205` |
| Right drive wheel | DM6215 | `0x06` | `0x146` | `0x206` |

Code locations:

- `../../config/motor_config.h`: motor model IDs, limits, total count
- `../../config/hardware_config.h`: logical motor mapping
- `../src/motor_controller.c`: assigns model type by motor ID
- `../src/leg_controller.c`: maps joints and wheels to controllers

The first four motors are joints and are limited by `DM4310_*`. The last two motors are drive wheels and are limited by `DM6215_*`.
