/** @file stm_lcd_touch_ft5206.c @brief 触点解码、错误传递和实例管理。 */
#include "stm_lcd_touch_ft5206.h"
#include <stdlib.h>
#include <string.h>
struct lcd_touch_ft5206_context {
    lcd_touch_ft5206_config_t config;
    lcd_touch_ft5206_point_t points[LCD_TOUCH_FT5206_MAX_POINTS];
    uint8_t count;
};
stm_err_t lcd_touch_ft5206_create(const lcd_touch_ft5206_config_t *config, lcd_touch_ft5206_handle_t *out)
{
    if (!config || !out) return STM_ERR_INVALID_ARG;
    if (*out) return STM_ERR_INVALID_STATE;
    if (!config->read_reg  || !config->x_max || !config->y_max ||
        config->swap_xy > 1u || config->mirror_x > 1u || config->mirror_y > 1u ||
        (config->reset && !config->delay_ms)) return STM_ERR_INVALID_CONFIG;
    lcd_touch_ft5206_handle_t touch = calloc(1, sizeof(*touch));
    if (!touch) return STM_ERR_NO_MEM;
    touch->config = *config;
    *out = touch;
    return STM_OK;
}
stm_err_t lcd_touch_ft5206_delete(lcd_touch_ft5206_handle_t *handle)
{
    if (!handle) return STM_ERR_INVALID_ARG;
    free(*handle);
    *handle = NULL;
    return STM_OK;
}
stm_err_t lcd_touch_ft5206_reset(lcd_touch_ft5206_handle_t touch)
{
    if (!touch) return STM_ERR_INVALID_ARG;
    touch->count = 0;
    if (!touch->config.reset) return STM_OK;
    stm_err_t err = touch->config.reset(touch->config.io, 0);
    if (err != STM_OK) return err;
    touch->config.delay_ms(touch->config.io, 20);
    err = touch->config.reset(touch->config.io, 1);
    if (err != STM_OK) return err;
    touch->config.delay_ms(touch->config.io, 50);
    return STM_OK;
}
stm_err_t lcd_touch_ft5206_read_data(lcd_touch_ft5206_handle_t touch)
{
    if (!touch) return STM_ERR_INVALID_ARG;
    uint8_t header, raw[LCD_TOUCH_FT5206_MAX_POINTS * 6u];
    touch->count = 0;
    stm_err_t err = touch->config.read_reg(touch->config.io, 0x02, &header, 1);
    if (err != STM_OK) return err;
    uint8_t n = header & 0x0fu;
    if (n > LCD_TOUCH_FT5206_MAX_POINTS) return STM_ERR_VERIFY;
    if (!n) return STM_OK;
    err = touch->config.read_reg(touch->config.io, 0x03, raw, (size_t)n * 6u);
    if (err != STM_OK) return err;
    for (uint8_t i = 0; i < n; ++i) {
        const uint8_t *p = raw + (size_t)i * 6u;
        if ((p[0] >> 6) == 1u || (p[0] >> 6) == 3u) continue;
        uint16_t x = (uint16_t)(((p[0] & 0x0fu) << 8) | p[1]);
        uint16_t y = (uint16_t)(((p[2] & 0x0fu) << 8) | p[3]);
        if (touch->config.swap_xy) { uint16_t t = x; x = y; y = t; }
        if (x >= touch->config.x_max || y >= touch->config.y_max) continue;
        if (touch->config.mirror_x) x = (uint16_t)(touch->config.x_max - 1u - x);
        if (touch->config.mirror_y) y = (uint16_t)(touch->config.y_max - 1u - y);
        touch->points[touch->count].x = x;
        touch->points[touch->count].y = y;
        touch->points[touch->count].id = p[2] >> 4;
        ++touch->count;
    }
    return STM_OK;
}
stm_err_t lcd_touch_ft5206_get_data(lcd_touch_ft5206_handle_t touch, lcd_touch_ft5206_point_t *points, size_t capacity, size_t *count)
{
    if (count) *count = 0;
    if (!touch || !count || (capacity && !points)) return STM_ERR_INVALID_ARG;
    *count = touch->count < capacity ? touch->count : capacity;
    if (*count) memcpy(points, touch->points, *count * sizeof(*points));
    return STM_OK;
}
