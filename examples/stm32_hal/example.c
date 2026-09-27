#include "example.h"
#include <limits.h>

static int read_reg(void *io, uint8_t reg, uint8_t *data, size_t len)
{
    stm_lcd_touch_ft5206_example_board_t *board = (stm_lcd_touch_ft5206_example_board_t *)io;
    if (len > UINT16_MAX) return -1;
    return HAL_I2C_Mem_Read(board->i2c, (uint16_t)(board->address_7bit << 1),
                            reg, I2C_MEMADD_SIZE_8BIT, data, (uint16_t)len, 100u) == HAL_OK ? 0 : -1;
}

static void delay_ms(void *io, uint32_t ms) { (void)io; HAL_Delay(ms); }
static void reset(void *io, int high)
{
    stm_lcd_touch_ft5206_example_board_t *board = (stm_lcd_touch_ft5206_example_board_t *)io;
    HAL_GPIO_WritePin(board->rst_port, board->rst_pin, high ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

int stm_lcd_touch_ft5206_example_start(stm_lcd_touch_ft5206_t *touch,
                                       stm_lcd_touch_ft5206_example_board_t *board)
{
    stm_lcd_touch_ft5206_config_t cfg;
    int result;
    if (!touch || !board || !board->i2c || !board->address_7bit || board->address_7bit > 0x7fu ||
        !board->width || !board->height) return -1;
    cfg = (stm_lcd_touch_ft5206_config_t){
        .read_reg = read_reg, .delay_ms = delay_ms,
        .reset = board->rst_port ? reset : NULL, .io = board,
        .x_max = board->width, .y_max = board->height,
        .swap_xy = board->swap_xy, .mirror_x = board->mirror_x, .mirror_y = board->mirror_y,
    };
    result = stm_lcd_touch_ft5206_new_i2c(touch, &cfg);
    if (result == 0) result = stm_lcd_touch_ft5206_reset(touch);
    return result;
}

int stm_lcd_touch_ft5206_example_poll(stm_lcd_touch_ft5206_t *touch,
                                      stm_lcd_touch_ft5206_point_t *points,
                                      size_t capacity, size_t *count)
{
    int result = stm_lcd_touch_ft5206_read_data(touch);
    if (result == 0) result = stm_lcd_touch_ft5206_get_data(touch, points, capacity, count);
    if (result != 0 && count) *count = 0;
    return result;
}
