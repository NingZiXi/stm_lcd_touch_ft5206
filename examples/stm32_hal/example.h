#ifndef STM_LCD_TOUCH_FT5206_EXAMPLE_H
#define STM_LCD_TOUCH_FT5206_EXAMPLE_H
#include "stm32h7xx_hal.h"
#include "stm_lcd_touch_ft5206.h"

typedef struct {
    I2C_HandleTypeDef *i2c;
    uint8_t address_7bit;
    GPIO_TypeDef *rst_port;
    uint16_t rst_pin;
    uint16_t width, height;
    uint8_t swap_xy, mirror_x, mirror_y;
} stm_lcd_touch_ft5206_example_board_t;

/* I2C/GPIO 由 CubeMX 初始化；board、touch 在后续读取期间始终有效。 */
int stm_lcd_touch_ft5206_example_start(stm_lcd_touch_ft5206_t *touch,
                                       stm_lcd_touch_ft5206_example_board_t *board);
int stm_lcd_touch_ft5206_example_poll(stm_lcd_touch_ft5206_t *touch,
                                      stm_lcd_touch_ft5206_point_t *points,
                                      size_t capacity, size_t *count);
#endif
