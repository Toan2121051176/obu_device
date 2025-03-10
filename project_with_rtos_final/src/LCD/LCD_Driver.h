#include "hal_data.h"
#include "stdlib.h"

#define  RESET_OFF      R_IOPORT_PinWrite(&g_ioport_ctrl, RESET_Pin, 1)
#define  RESET_ON       R_IOPORT_PinWrite(&g_ioport_ctrl, RESET_Pin, 0)
#define  CS_OFF         R_IOPORT_PinWrite(&g_ioport_ctrl, CS_Pin, 1)
#define  CS_ON          R_IOPORT_PinWrite(&g_ioport_ctrl, CS_Pin, 0)
#define  DC_DATA        R_IOPORT_PinWrite(&g_ioport_ctrl, DC_Pin, 1)
#define  DC_COMMAND     R_IOPORT_PinWrite(&g_ioport_ctrl, DC_Pin, 0)

#define BLACK           0x0000
#define NAVY            0x000F
#define DARKGREEN       0x03E0
#define DARKCYAN        0x03EF
#define MAROON          0x7800
#define PURPLE          0x780F
#define OLIVE           0x7BE0
#define LIGHTGREY       0xC618
#define DARKGREY        0x7BEF
#define BLUE            0x001F
#define GREEN           0x07E0
#define CYAN            0x07FF
#define RED             0xF800
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0
#define WHITE           0xFFFF
#define ORANGE          0xFD20
#define GREENYELLOW     0xAFE5
#define PINK            0xF81F

#define swap(a,b)       {int16_t t=a;a=b;b=t;}
#define BURST_MAX_SIZE  500

void Show_Menu(void);
void LCD_Init(void);
void LCD_Reset(void);
void LCD_Set_Rotation(unsigned char rotation);
void LCD_Fill_Screen(unsigned int color);

void LCD_Draw_Pixel(int x, int y, unsigned int color);
void LCD_Draw_Double_Pixel(int x, int y, unsigned int color1, unsigned int color2);
void LCD_Draw_Circle(unsigned int x0, unsigned int y0, int r, unsigned int color, unsigned char flood);
void LCD_Draw_Line(unsigned int color, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
void LCD_Draw_Filled_Rectangle(unsigned int color,unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
void LCD_Draw_Empty_Rectangle(unsigned int color,unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);


