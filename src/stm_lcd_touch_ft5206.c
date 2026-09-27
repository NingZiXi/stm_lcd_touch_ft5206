#include "stm_lcd_touch_ft5206.h"
#include <string.h>
int stm_lcd_touch_ft5206_new_i2c(stm_lcd_touch_ft5206_t *touch,
                                    const stm_lcd_touch_ft5206_config_t *config)
{
    if (!touch || !config || !config->read_reg || !config->x_max || !config->y_max) return -1;
    memset(touch, 0, sizeof(*touch));
    touch->config = *config;
    touch->created = 1;
    return 0;
}
int stm_lcd_touch_ft5206_reset(stm_lcd_touch_ft5206_t *touch)
{
    if (!touch || !touch->created) return -1;
    touch->count = 0;
    if (!touch->config.reset) return 0;
    if (!touch->config.delay_ms) return -1;
    touch->config.reset(touch->config.io, 0);
    touch->config.delay_ms(touch->config.io, 20);
    touch->config.reset(touch->config.io, 1);
    touch->config.delay_ms(touch->config.io, 50);
    return 0;
}
int stm_lcd_touch_ft5206_read_data(stm_lcd_touch_ft5206_t *touch)
{
    uint8_t header, bytes[STM_LCD_TOUCH_FT5206_MAX_POINTS * 6];
    uint8_t n, i;
    if (!touch || !touch->created) return -1;
    touch->count = 0; /* A failed I2C read must never replay old touches. */
    if (touch->config.read_reg(touch->config.io, 0x02, &header, 1)) return -2;
    n = header & 0x0fu;
    if (n > STM_LCD_TOUCH_FT5206_MAX_POINTS) return -3;
    if (!n) return 0;
    if (touch->config.read_reg(touch->config.io, 0x03, bytes, (size_t)n * 6u)) return -2;
    for (i = 0; i < n; ++i) {
        const uint8_t *p = &bytes[i * 6u];
        uint16_t x, y, t;
        if ((p[0] >> 6) == 1u || (p[0] >> 6) == 3u) continue; /* up or reserved */
        x = (uint16_t)(((p[0] & 0x0fu) << 8) | p[1]);
        y = (uint16_t)(((p[2] & 0x0fu) << 8) | p[3]);
        if (touch->config.swap_xy) { t = x; x = y; y = t; }
        if (x >= touch->config.x_max || y >= touch->config.y_max) continue;
        if (touch->config.mirror_x) x = (uint16_t)(touch->config.x_max - 1u - x);
        if (touch->config.mirror_y) y = (uint16_t)(touch->config.y_max - 1u - y);
        touch->points[touch->count].x = x;
        touch->points[touch->count].y = y;
        touch->points[touch->count].id = p[2] >> 4;
        ++touch->count;
    }
    return 0;
}
int stm_lcd_touch_ft5206_get_data(const stm_lcd_touch_ft5206_t *touch,
                                     stm_lcd_touch_ft5206_point_t *points, size_t capacity,
                                     size_t *count)
{
    if (!touch || !touch->created || !count || (capacity && !points)) return -1;
    *count = touch->count < capacity ? touch->count : capacity;
    if (*count) memcpy(points, touch->points, *count * sizeof(*points));
    return 0;
}
