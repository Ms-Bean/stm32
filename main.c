#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_spi.h>

#define LCD_RS GPIO_Pin_1
#define LCD_RS_PORT GPIOA

#define LCD_RW GPIO_Pin_2
#define LCD_RW_PORT GPIOA

#define LCD_E GPIO_Pin_3
#define LCD_E_PORT GPIOA

#define LCD_D4 GPIO_Pin_6
#define LCD_D4_PORT GPIOC

#define LCD_D5 GPIO_Pin_7
#define LCD_D5_PORT GPIOC

#define LCD_D6 GPIO_Pin_8
#define LCD_D6_PORT GPIOC

#define LCD_D7 GPIO_Pin_9
#define LCD_D7_PORT GPIOC


static __IO uint32_t delay_remaining = 0;
void delay_milliseconds(uint32_t milliseconds)
{
    delay_remaining = milliseconds;
    while(delay_remaining != 0);
}

void lcd_gpio_init(void);
void lcd_write_nibble(uint8_t nibble, int is_data);
void lcd_init(void);

int main(void)
{
    SystemInit();
    lcd_gpio_init();

    if(SysTick_Config(SystemCoreClock / 1000))
    {
        for(;;);
    }

    lcd_gpio_init();
    lcd_init();
    for(;;);

    return 0;
}

void lcd_gpio_init(void)
{
    GPIO_InitTypeDef gpio_init;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio_init.GPIO_Pin = GPIO_Pin_1;
    gpio_init.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &gpio_init);
    
    gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio_init.GPIO_Pin = GPIO_Pin_2;
    gpio_init.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &gpio_init);
    
    gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio_init.GPIO_Pin = GPIO_Pin_3;
    gpio_init.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &gpio_init);

    gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio_init.GPIO_Pin = GPIO_Pin_6;
    gpio_init.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOC, &gpio_init);

    gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio_init.GPIO_Pin = GPIO_Pin_7;
    gpio_init.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOC, &gpio_init);
    
    gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio_init.GPIO_Pin = GPIO_Pin_8;
    gpio_init.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOC, &gpio_init);

    gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio_init.GPIO_Pin = GPIO_Pin_9;
    gpio_init.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOC, &gpio_init);
}

void lcd_write_nibble(uint8_t nibble, int is_data)
{
    /*See page 6 of TC1602A-09T*/

    /* Set RS to low to indicate a command or high to indicate data */
    GPIO_WriteBit(LCD_RS_PORT, LCD_RS, is_data ? 1 : 0);

    /*Set R/W to low (indicate write operation) */
    GPIO_WriteBit(LCD_RW_PORT, LCD_RW, 0);
    
    /*Set data pins DB 4-7*/
    GPIO_WriteBit(LCD_D4_PORT, LCD_D4, nibble & 0x1 ? 1 : 0);
    GPIO_WriteBit(LCD_D5_PORT, LCD_D5, nibble & 0x2 ? 1 : 0);
    GPIO_WriteBit(LCD_D6_PORT, LCD_D6, nibble & 0x4 ? 1 : 0);
    GPIO_WriteBit(LCD_D7_PORT, LCD_D7, nibble & 0x8 ? 1 : 0);

    /*Wait a minimum of 30 nanoseconds (t[SP1]) before setting E*/
    delay_milliseconds(1);

    /*Set E to high begin transmission */
    GPIO_WriteBit(LCD_E_PORT, LCD_E, 1);

    /*Wait a minimum of 150 nanoseconds (pulse width) to complete transmission*/
    delay_milliseconds(1);

    /*End transmission*/
    GPIO_WriteBit(LCD_E_PORT, LCD_E, 0);
    
    /*Wait a minimum of 10 + 400 nanoseconds (address hold time + E cycle time) */
    delay_milliseconds(1);
}
void lcd_write(uint8_t instruction, int is_data)
{
    lcd_write_nibble(instruction >> 4, is_data);
    lcd_write_nibble(instruction & 0x0F, is_data);
}
void lcd_init(void)
{
    /* See page 12 of TC1602A-09T */
    
    delay_milliseconds(40); 
    lcd_write(0x03, 0); /* Wake up */
    delay_milliseconds(5); 
    lcd_write(0x03, 0); /* Wake up */
    delay_milliseconds(1); 
    lcd_write(0x03, 0); /* Wake up */

    /*
        function set
        4 bit mode (DL=0)
        2 lines (N=1)
        5x10 dots (F=1)
    */
    lcd_write(0x2C, 0);

    /*display off*/
    lcd_write(0x08, 0);

    /*display clear*/
    lcd_write(0x01, 0);

    /*
        entry mode set
        increment cursor position (I/D = 1)
        no shift (S = 0)
    */
    lcd_write(0x06, 0);
} 
void assert_failed(uint8_t *file, uint32_t line)
{
        for(;;);
}

void SysTick_Handler(void)
{
    if(delay_remaining != 0)
    {
        delay_remaining--;
    }
}
