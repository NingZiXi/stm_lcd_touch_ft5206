# stm_lcd_touch_ft5206

FT5206 I²C 触摸芯片驱动，独立于面板和 LVGL。板级代码负责配置 I²C、复位 GPIO 和实际器件地址；完整 HAL 与 LVGL 接入示例见[显示与触摸接入指南](https://github.com/NingZiXi/stm32-hal-lib/blob/main/docs/display-components.md)。

完整中文示例：[`examples/stm32_hal/README.md`](examples/stm32_hal/README.md)（含 HAL I²C 适配与轮询代码）。

```c
stm_lcd_touch_ft5206_t touch = {0};
stm_lcd_touch_ft5206_config_t cfg = {
    .read_reg = board_read_reg, .io = &hi2c_touch,
    .x_max = BOARD_LCD_WIDTH, .y_max = BOARD_LCD_HEIGHT,
    .swap_xy = 0, .mirror_x = 0, .mirror_y = 0,
};
int rc = stm_lcd_touch_ft5206_new_i2c(&touch, &cfg);
if (rc == 0) rc = stm_lcd_touch_ft5206_read_data(&touch);
if (rc == 0) {
    stm_lcd_touch_ft5206_point_t point;
    size_t count = 0;
    rc = stm_lcd_touch_ft5206_get_data(&touch, &point, 1, &count);
}
```

`read_reg(io, reg, data, len)` 应从指定寄存器连续读取 `len` 字节，0 表示成功。常见 7 位地址 `0x38` 给 STM32 HAL `HAL_I2C_Mem_Read` 使用时传入 `0x38 << 1`，以实际器件为准。只有接好复位脚并提供 `reset` 和 `delay_ms` 回调时才调用 `stm_lcd_touch_ft5206_reset`。`read_data` 从寄存器 `0x02` 读取点数，从 `0x03` 起读取最多五个触点；每次重新读取前清空缓存，避免通信失败时回报旧触点。`get_data` 只获取最新缓存，返回点数不超过 `capacity`。返回 -1 为参数/状态错误，-2 为 I²C 错误，-3 为点数超限。根据面板方向设置坐标互换和镜像，目前仅完成主机测试。

```cmake
add_subdirectory(Lib/stm_lcd_touch_ft5206)
target_link_libraries(app PRIVATE stm_lcd_touch_ft5206) # app 改为你的实际目标名
```
