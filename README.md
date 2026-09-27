# stm_lcd_touch_ft5206

FT5206 I2C touch chip component. The BSP owns I2C, reset GPIO and the device address (vendor example uses 8-bit 0x70 write / 0x71 read; check your HAL API's address convention). Call `new_i2c`, optionally `reset`, then `read_data` and `get_data`. `read_data` polls register 0x02 and reads up to five points starting at 0x03; it clears cached points before I2C reads so failures cannot report stale touches. Coordinates, swap and mirror are configured at construction. It does not depend on a display library or LVGL. Based on the H757 vendor example experiment 24; hardware validation is still required.

```cmake
add_subdirectory(Lib/stm_lcd_touch_ft5206)
target_link_libraries(app PRIVATE stm_lcd_touch_ft5206)
```
