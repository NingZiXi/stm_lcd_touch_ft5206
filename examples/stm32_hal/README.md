# FT5206：STM32 HAL I²C 接入示例

`example.c` / `example.h` 提供 CubeMX 工程可移植的 I²C 寄存器读取、可选复位和轮询触点代码；不是完整 CubeMX 工程，未包含 ST HAL/CMSIS。**尚未完成触摸实板测试**。其他 STM32 系列请替换 HAL 头文件。

先核对实际器件是否为 FT5206、7 位地址、供电和引脚。CubeMX 初始化对应 I²C 和可选复位 GPIO，将本组件与 `example.c` 加入 CM7 应用目标。在板级应用入口调用：

```c
#include "example.h"
#include "i2c.h"
#include "gpio.h"

static stm_lcd_touch_ft5206_t touch;
static stm_lcd_touch_ft5206_example_board_t touch_board;

void app_main(void)
{
    touch_board = (stm_lcd_touch_ft5206_example_board_t){
        .i2c = &hi2c_touch, /* 换成实际 I²C 句柄 */
        .address_7bit = 0x38, /* 须用实物确认；HAL 内部使用左移后的地址 */
        .rst_port = TOUCH_RST_GPIO_Port, .rst_pin = TOUCH_RST_Pin,
        .width = PANEL_WIDTH, .height = PANEL_HEIGHT,
        .swap_xy = 0, .mirror_x = 0, .mirror_y = 0,
    };
    int rc = stm_lcd_touch_ft5206_example_start(&touch, &touch_board);
    for (;;) {
        stm_lcd_touch_ft5206_point_t points[STM_LCD_TOUCH_FT5206_MAX_POINTS];
        size_t count = 0;
        if (rc == 0) rc = stm_lcd_touch_ft5206_example_poll(&touch, points, 5, &count);
        if (rc != 0) { /* 记录错误码，稍后重试；不可把旧数据当新触摸。 */ rc = 0; }
        /* 根据 count 和 points[0..count-1] 处理触摸；没有触摸时 count == 0。 */
        HAL_Delay(20);
    }
}
```

无复位脚时设置 `rst_port = NULL`；根据实际屏幕方向调整 `swap_xy` / `mirror_x` / `mirror_y`。轮询示例使用阻塞 HAL API，应在任务/主循环而非中断中运行；共享 I²C 时须用项目锁保护事务。`touch_board` 和 `touch` 需长期有效。读寄存器失败为 `-2`，点数超限为 `-3`，参数或状态错误为 `-1`。

在 CM7 的 `CMakeLists.txt` 把复制到 `App/` 的示例源码加入应用目标（具体目录名按工程调整）：

```cmake
target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/App/example.c)
```

中文主页：[README.md](../../README.md)；[完整接入指南](https://github.com/NingZiXi/stm32-hal-lib/blob/main/docs/display-components.md)。
