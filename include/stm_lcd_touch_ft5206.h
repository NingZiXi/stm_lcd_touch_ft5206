#ifndef STM_LCD_TOUCH_FT5206_H
#define STM_LCD_TOUCH_FT5206_H
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
#define STM_LCD_TOUCH_FT5206_MAX_POINTS 5
typedef struct { uint16_t x, y; uint8_t id; } stm_lcd_touch_ft5206_point_t;
typedef struct {
    /* Read len register bytes over I2C; return 0 on success. */
    int (*read_reg)(void *io, uint8_t reg, uint8_t *data, size_t len);
    void (*delay_ms)(void *io, uint32_t ms);
    void (*reset)(void *io, int high);
    void *io;
    uint16_t x_max, y_max;
    uint8_t swap_xy, mirror_x, mirror_y;
} stm_lcd_touch_ft5206_config_t;
typedef struct {
    stm_lcd_touch_ft5206_config_t config;
    stm_lcd_touch_ft5206_point_t points[STM_LCD_TOUCH_FT5206_MAX_POINTS];
    uint8_t count, created;
} stm_lcd_touch_ft5206_t;
int stm_lcd_touch_ft5206_new_i2c(stm_lcd_touch_ft5206_t *touch,
                                    const stm_lcd_touch_ft5206_config_t *config);
int stm_lcd_touch_ft5206_reset(stm_lcd_touch_ft5206_t *touch);
int stm_lcd_touch_ft5206_read_data(stm_lcd_touch_ft5206_t *touch);
int stm_lcd_touch_ft5206_get_data(const stm_lcd_touch_ft5206_t *touch,
                                     stm_lcd_touch_ft5206_point_t *points, size_t capacity,
                                     size_t *count);
#ifdef __cplusplus
}
#endif
#endif
