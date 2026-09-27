#include "stm_lcd_touch_ft5206.h"
#include <assert.h>
#include <string.h>
static int fail;
static int rd(void *io, uint8_t reg, uint8_t *data, size_t n)
{
    (void)io;
    if (fail) return -1;
    if (reg==0x02) { assert(n==1); *data=2; return 0; }
    assert(reg==0x03 && n==12);
    memset(data,0,n);
    data[1]=12; data[3]=20; data[2]=0x30; /* ID 3, y=20 */
    data[6]=0xC0; /* reserved event is ignored */
    return 0;
}
int main(void)
{
    stm_lcd_touch_ft5206_t t={0};
    stm_lcd_touch_ft5206_point_t pt[2]; size_t count=9;
    stm_lcd_touch_ft5206_config_t cfg={rd,NULL,NULL,NULL,240,320,0,1,0};
    assert(stm_lcd_touch_ft5206_new_i2c(&t,&cfg)==0);
    assert(stm_lcd_touch_ft5206_read_data(&t)==0);
    assert(stm_lcd_touch_ft5206_get_data(&t,pt,2,&count)==0);
    assert(count==1 && pt[0].x==227 && pt[0].y==20 && pt[0].id==3);
    fail=1;
    assert(stm_lcd_touch_ft5206_read_data(&t)==-2);
    assert(stm_lcd_touch_ft5206_get_data(&t,pt,2,&count)==0 && count==0);
    return 0;
}
