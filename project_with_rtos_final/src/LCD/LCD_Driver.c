#include "LCD_Driver.h"
#include "fonts.h"
#include "hal_data.h"

static void LCD_SPI_Init(void);
static void Before_Sending_Data(void);
static void Before_Sending_Command(void);
static void LCD_Send_Data(unsigned char data);
static void LCD_Send_Command(unsigned char command);
static void LCD_Send_Burst(unsigned short color, unsigned long len);
//static void LCD_Draw_Double_Pixel(int x, int y, unsigned int color1, unsigned int color2);
static void LCD_Draw_Char(int x, int y, unsigned int color, unsigned int bgcolor, unsigned char charcode, unsigned char size);

static unsigned int X_SIZE = 240;
static unsigned int Y_SIZE = 320;

unsigned char hh;
//static SPI_HandleTypeDef lcd_spi;

void LCD_Init() {
    LCD_SPI_Init();

    /* Reset The Screen */
    LCD_Reset();
    LCD_Send_Command(0x01);
    //R_BSP_SoftwareDelay(50, BSP_DELAY_UNITS_MILLISECONDS);

    //if nothing happen delete the code above

    /* Power Control A */
    LCD_Send_Command(0xCB);
    LCD_Send_Data(0x39);
    LCD_Send_Data(0x2C);
    LCD_Send_Data(0x00);
    LCD_Send_Data(0x34);
    LCD_Send_Data(0x02);

    /* Power Control B */
    LCD_Send_Command(0xCF);
    LCD_Send_Data(0x00);
    LCD_Send_Data(0xC1);
    LCD_Send_Data(0x30);

    /* Driver timing control A */
    LCD_Send_Command(0xE8);
    LCD_Send_Data(0x85);
    LCD_Send_Data(0x00);
    LCD_Send_Data(0x78);

    /* Driver timing control B */
    LCD_Send_Command(0xEA);
    LCD_Send_Data(0x00);
    LCD_Send_Data(0x00);

    /* Power on Sequence control */
    LCD_Send_Command(0xED);
    LCD_Send_Data(0x64);
    LCD_Send_Data(0x03);
    LCD_Send_Data(0x12);
    LCD_Send_Data(0x81);

    /* Pump ratio control */
    LCD_Send_Command(0xF7);
    LCD_Send_Data(0x20);

    /* Power Control 1 */
    LCD_Send_Command(0xC0);
    LCD_Send_Data(0x23);

    /* Power Control 2 */
    LCD_Send_Command(0xC1);
    LCD_Send_Data(0x10);

    /* VCOM Control 1 */
    LCD_Send_Command(0xC5);
    LCD_Send_Data(0x3E);
    LCD_Send_Data(0x28);

    /* VCOM Control 2 */
    LCD_Send_Command(0xC7);
    LCD_Send_Data(0x86);

    /* VCOM Control 2 */
//  LCD_Send_Command(0x36);
//  LCD_Send_Data(0x48);

    LCD_Send_Command(0x36);
    LCD_Send_Data(0x28);  // Correct orientation, no mirror

    /* Pixel Format Set */
    LCD_Send_Command(0x3A);
    LCD_Send_Data(0x55);    //16bit

    LCD_Send_Command(0xB1);
    LCD_Send_Data(0x00);
    LCD_Send_Data(0x18);

    /* Display Function Control */
    LCD_Send_Command(0xB6);
    LCD_Send_Data(0x08);
    LCD_Send_Data(0x82);
    LCD_Send_Data(0x27);

    /* 3GAMMA FUNCTION DISABLE */
    LCD_Send_Command(0xF2);
    LCD_Send_Data(0x00);

    /* GAMMA CURVE SELECTED */
    LCD_Send_Command(0x26); //Gamma set
    LCD_Send_Data(0x01);    //Gamma Curve (G2.2)

    //Positive Gamma  Correction
    LCD_Send_Command(0xE0);
    LCD_Send_Data(0x0F);
    LCD_Send_Data(0x31);
    LCD_Send_Data(0x2B);
    LCD_Send_Data(0x0C);
    LCD_Send_Data(0x0E);
    LCD_Send_Data(0x08);
    LCD_Send_Data(0x4E);
    LCD_Send_Data(0xF1);
    LCD_Send_Data(0x37);
    LCD_Send_Data(0x07);
    LCD_Send_Data(0x10);
    LCD_Send_Data(0x03);
    LCD_Send_Data(0x0E);
    LCD_Send_Data(0x09);
    LCD_Send_Data(0x00);

    //Negative Gamma  Correction
    LCD_Send_Command(0xE1);
    LCD_Send_Data(0x00);
    LCD_Send_Data(0x0E);
    LCD_Send_Data(0x14);
    LCD_Send_Data(0x03);
    LCD_Send_Data(0x11);
    LCD_Send_Data(0x07);
    LCD_Send_Data(0x31);
    LCD_Send_Data(0xC1);
    LCD_Send_Data(0x48);
    LCD_Send_Data(0x08);
    LCD_Send_Data(0x0F);
    LCD_Send_Data(0x0C);
    LCD_Send_Data(0x31);
    LCD_Send_Data(0x36);
    LCD_Send_Data(0x0F);

    //EXIT SLEEP
    LCD_Send_Command(0x11);

    //TURN ON DISPLAY
    LCD_Send_Command(0x29);
    LCD_Send_Data(0x2C);
}

/**
 * @brief SPI1 Initialization Function
 * @param None
 * @retval None
 */
static void LCD_SPI_Init() {
    fsp_err_t status =  R_SCI_SPI_Open(&g_spi0_ctrl, &g_spi0_cfg);
    if(status != FSP_SUCCESS)
           SEGGER_RTT_printf(0,"ko khoi tao dc LCD");
    else
           SEGGER_RTT_printf(0,"success LCD");
}

static void Before_Sending_Data() {
    R_IOPORT_PinWrite(&g_ioport_ctrl, DC_Pin, 1);
    R_IOPORT_PinWrite(&g_ioport_ctrl, CS_Pin, 0);

}

static void Before_Sending_Command() {
    R_IOPORT_PinWrite(&g_ioport_ctrl, DC_Pin, 0);
    R_IOPORT_PinWrite(&g_ioport_ctrl, CS_Pin, 0);
}

void LCD_SPI_Send(unsigned char data)
{
    R_SCI_SPI_Write(&g_spi0_ctrl, &data, 1, SPI_BIT_WIDTH_8_BITS);

}

void LCD_SPI_Send_Multiple(unsigned char data, int size)
{
    R_SCI_SPI_Write(&g_spi0_ctrl, &data, 10, SPI_BIT_WIDTH_8_BITS);

}

void LCD_SPI_Send_32(unsigned char command, unsigned long data) {
    CS_ON;
    DC_COMMAND;
    LCD_SPI_Send(command);

    DC_DATA;
    LCD_SPI_Send(data >> 24);
    LCD_SPI_Send(data >> 16);
    LCD_SPI_Send(data >> 8);
    LCD_SPI_Send(data);

}

void LCD_Reset() {
    RESET_ON;
    R_BSP_SoftwareDelay(50, BSP_DELAY_UNITS_MILLISECONDS);
    RESET_OFF;
    CS_ON;
    R_BSP_SoftwareDelay(50, BSP_DELAY_UNITS_MILLISECONDS);
    LCD_Send_Command(0x01);
    CS_OFF;
}

void LCD_Set_Rotation(unsigned char rotation) {
    LCD_Send_Command(0x36);
    switch (rotation) {
    case 0:
        LCD_Send_Data(0x40);
        X_SIZE = 240;
        Y_SIZE = 320;
        break;
    case 1:
        LCD_Send_Data(0x00);
        X_SIZE = 320;
        Y_SIZE = 240;
        break;
    case 2:
        LCD_Send_Data(0x80);
        X_SIZE = 240;
        Y_SIZE = 320;
        break;
    case 3:
        LCD_Send_Data(0xB0); //0xE0
        X_SIZE = 320;
        Y_SIZE = 240;
        break;
    }
}

void LCD_Set_Address(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2) {
     unsigned long t;
     t = x1;
     t <<= 16;
     t |= x2;
     LCD_SPI_Send_32(0x2A, t); //Column Addres Set
     t = y1;
     t <<= 16;
     t |= y2;
     LCD_SPI_Send_32(0x2B, t); //Page Addres Set
}

void LCD_Fill_Screen(unsigned int color)
{
    LCD_Set_Address(0, 0, X_SIZE-1, Y_SIZE-1);
    LCD_Send_Burst(color, (long)X_SIZE * (long)Y_SIZE);
}

void LCD_Draw_Pixel(int x, int y, unsigned int color)
{
    if((x < 0) || (y < 0) || (x >= X_SIZE) || (y >= Y_SIZE))
    {
        return;
    }
    LCD_Set_Address(x, y, X_SIZE-1, Y_SIZE-1);
    DC_COMMAND;
    LCD_SPI_Send(0x2C);
    DC_DATA;
    LCD_SPI_Send(color>>8);
    LCD_SPI_Send(color);
}

void LCD_Draw_Filled_Rectangle(unsigned int color,unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2)
{
    LCD_Set_Address(x1, y1, x2, y2);
    LCD_Send_Burst(color, (long)(x2-x1+1) * (long)(y2-y1+1));
}

void LCD_Draw_Empty_Rectangle(unsigned int color,unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2)
{
    LCD_Draw_Line(color, x1, y1, x2, y1);
    LCD_Draw_Line(color, x2, y1, x2, y2);
    LCD_Draw_Line(color, x1, y1, x1, y2);
    LCD_Draw_Line(color, x1, y2, x2, y2);
}

void LCD_Draw_Line(unsigned int color, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2)
{
    int steep = abs(y2-y1) > abs(x2-x1);

    if (steep)
    {
        swap(x1,y1);
        swap(x2,y2);
    }

    if(x1>x2)
    {
        swap(x1,x2);
        swap(y1,y2);
    }

    int dx,dy;
    dx = (x2 - x1);
    dy = abs(y2 - y1);
    int err = dx / 2;
    int ystep;
    if(y1 < y2)
    {
        ystep = 1;
    }
    else
    {
        ystep = -1;
    }
    for (; x1 <= x2; x1++)
    {
        if (steep)
        {
            LCD_Draw_Pixel(y1, x1, color);
        }
        else
        {
            LCD_Draw_Pixel(x1, y1, color);
        }
        err -= dy;
        if (err < 0)
        {
            y1 += ystep;
            err = dx;
        }
    }
}

void LCD_Draw_Circle(unsigned int x0, unsigned int y0, int r, unsigned int color, unsigned char flood) {
    int f = 1 - r;
    int ddF_x = 1;
    int ddF_y = -2 * r;
    int x = 0;
    int y = r;
    if (flood == 0) {
        LCD_Draw_Pixel(x0, y0 + r, color);
        LCD_Draw_Pixel(x0, y0 - r, color);
        LCD_Draw_Pixel(x0 + r, y0, color);
        LCD_Draw_Pixel(x0 - r, y0, color);
        while (x < y) {
            if (f >= 0) {
                y--;
                ddF_y += 2;
                f += ddF_y;
            }
            x++;
            ddF_x += 2;
            f += ddF_x;
            LCD_Draw_Pixel(x0 + x, y0 + y, color);
            LCD_Draw_Pixel(x0 - x, y0 + y, color);
            LCD_Draw_Pixel(x0 + x, y0 - y, color);
            LCD_Draw_Pixel(x0 - x, y0 - y, color);
            LCD_Draw_Pixel(x0 + y, y0 + x, color);
            LCD_Draw_Pixel(x0 - y, y0 + x, color);
            LCD_Draw_Pixel(x0 + y, y0 - x, color);
            LCD_Draw_Pixel(x0 - y, y0 - x, color);
        }
    } else {
        LCD_Draw_Pixel(x0, y0 + r, color);
        LCD_Draw_Pixel(x0, y0 - r, color);
        LCD_Set_Address(x0 - r, y0, x0 + r, y0);
        DC_COMMAND;
        LCD_SPI_Send(0x2C);
        DC_DATA;
        for (uint32_t fff = 0; fff < r * 2 + 1; fff++) {
            LCD_SPI_Send(color >> 8);
            LCD_SPI_Send(color);
        }
        while (x < y) {
            if (f >= 0) {
                y--;
                ddF_y += 2;
                f += ddF_y;
            }
            x++;
            ddF_x += 2;
            f += ddF_x;
            LCD_Set_Address(x0 - x, y0 + y, x0 + x, y0 + y);
            DC_COMMAND;
            LCD_SPI_Send(0x2C);
            DC_DATA;
            for (uint32_t fff = 0; fff < x * 2 + 1; fff++) {
                LCD_SPI_Send(color >> 8);
                LCD_SPI_Send(color);
            }
            LCD_Set_Address(x0 - x, y0 - y, x0 + x, y0 - y);
            DC_COMMAND;
            LCD_SPI_Send(0x2C);
            DC_DATA;
            for (uint32_t fff = 0; fff < x * 2 + 1; fff++) {
                LCD_SPI_Send(color >> 8);
                LCD_SPI_Send(color);
            }
            LCD_Set_Address(x0 - y, y0 + x, x0 + y, y0 + x);
            DC_COMMAND;
            LCD_SPI_Send(0x2C);
            DC_DATA;
            for (uint32_t fff = 0; fff < y * 2 + 1; fff++) {
                LCD_SPI_Send(color >> 8);
                LCD_SPI_Send(color);
            }
            LCD_Set_Address(x0 - y, y0 - x, x0 + y, y0 - x);
            DC_COMMAND;
            LCD_SPI_Send(0x2C);
            DC_DATA;
            for (uint32_t fff = 0; fff < y * 2 + 1; fff++) {
                LCD_SPI_Send(color >> 8);
                LCD_SPI_Send(color);
            }
        }
    }
}

static void LCD_Send_Command(unsigned char command) {
    Before_Sending_Command();
    LCD_SPI_Send(command);
}

static void LCD_Send_Data(unsigned char data) {
    Before_Sending_Data();
    LCD_SPI_Send(data);
}

static void LCD_Send_Burst(unsigned short color, unsigned long len) {
    unsigned short blocks;
    unsigned char i, high_bit = color >> 8, low_bit = color;
    CS_ON;
    DC_COMMAND;
    LCD_SPI_Send(0x2C);
    DC_DATA;
    LCD_SPI_Send(high_bit);
    LCD_SPI_Send(low_bit);
    len--;
    blocks = (unsigned short) (len / 64); //64 pixels/block
    if (high_bit == low_bit)
    {
        while (blocks--) {
            i = 16;
            do {
                LCD_SPI_Send(low_bit);
                LCD_SPI_Send(low_bit);
                LCD_SPI_Send(low_bit);
                LCD_SPI_Send(low_bit);
                LCD_SPI_Send(low_bit);
                LCD_SPI_Send(low_bit);
                LCD_SPI_Send(low_bit);
                LCD_SPI_Send(low_bit);
            } while (--i);
        }
        //Fill any remaining pixels(1 to 64)
        for (i = (unsigned char) len & 63; i--;) {
            LCD_SPI_Send(low_bit);
            LCD_SPI_Send(low_bit);
        }
    }
    else
    {
        while (blocks--) {
            i = 16;
            do {
                LCD_SPI_Send(high_bit);
                LCD_SPI_Send(low_bit);
                LCD_SPI_Send(high_bit);
                LCD_SPI_Send(low_bit);
                LCD_SPI_Send(high_bit);
                LCD_SPI_Send(low_bit);
                LCD_SPI_Send(high_bit);
                LCD_SPI_Send(low_bit);
            } while (--i);
        }
        //Fill any remaining pixels(1 to 64)
        for (i = (unsigned char) len & 63; i--;) {
            LCD_SPI_Send(high_bit);
            LCD_SPI_Send(low_bit);
        }
    }
    CS_OFF;
}

void LCD_Draw_Double_Pixel(int x, int y, unsigned int color1, unsigned int color2)
{
    if((x < 0) || (y < 0) || (x >= X_SIZE) || (y >= Y_SIZE))
    {
        return;
    }
    LCD_Set_Address(x, y, X_SIZE-1, Y_SIZE-1);
    DC_COMMAND;
    LCD_SPI_Send(0x2C);
    DC_DATA;
    LCD_SPI_Send(color1);
    LCD_SPI_Send(color2);
}

static void LCD_Draw_Char(int x, int y, unsigned int color, unsigned int bgcolor, unsigned char charcode, unsigned char size)
{
    int i,h;
    switch(size)
    {
        case 1:
            LCD_Set_Address(x, y, x+5, y+7);
            DC_COMMAND;
            LCD_SPI_Send(0x2C);
            DC_DATA;
            for(h=0;h<8;h++)
            {
                for(i=2;i<8;i++)
                {
                    if ((font6x8[charcode-0x20][h]>>(7-i))&0x01)
                    {
                        LCD_SPI_Send(color>>8);
                        LCD_SPI_Send(color);
                    }
                    else
                    {
                        LCD_SPI_Send(bgcolor>>8);
                        LCD_SPI_Send(bgcolor);
                    }
                }
            }
            break;
        case 2:
            LCD_Set_Address(x, y, x+7, y+15);
            DC_COMMAND;
            LCD_SPI_Send(0x2C);
            DC_DATA;
            for(h=0;h<16;h++)
            {
                for(i=0;i<8;i++)
                {
                    if ((font8x16[charcode-0x20][h]>>(7-i))&0x01)
                    {
                        LCD_SPI_Send(color>>8);
                        LCD_SPI_Send(color);
                    }
                    else
                    {
                        LCD_SPI_Send(bgcolor>>8);
                        LCD_SPI_Send(bgcolor);
                    }
                }
            }
            break;
        case 3:
            LCD_Set_Address(x, y, x+10, y+17);
            DC_COMMAND;
            LCD_SPI_Send(0x2C);
            DC_DATA;
            for(h=0;h<18;h++)
            {
                for(i=0;i<11;i++)
                {
                    if ((font11x18[charcode-0x20][h]>>(16-i))&0x01)
                    {
                        LCD_SPI_Send(color>>8);
                        LCD_SPI_Send(color);
                    }
                    else
                    {
                        LCD_SPI_Send(bgcolor>>8);
                        LCD_SPI_Send(bgcolor);
                    }
                }
            }
            break;
        case 4:
            LCD_Set_Address(x, y, x+15, y+25);
            DC_COMMAND;
            LCD_SPI_Send(0x2C);
            DC_DATA;
            for(h=0;h<26;h++)
            {
                for(i=0;i<16;i++)
                {
                    if ((font16x26[charcode-0x20][h]>>(15-i))&0x01)
                    {
                        LCD_SPI_Send(color>>8);
                        LCD_SPI_Send(color);
                    }
                    else
                    {
                        LCD_SPI_Send(bgcolor>>8);
                        LCD_SPI_Send(bgcolor);
                    }
                }
            }
            break;
        default:
            break;
    }
}

void LCD_Draw_String(unsigned int x, unsigned int y, unsigned int color, unsigned int bgcolor, char *str, unsigned char size)
{
    switch (size)
    {
    case 1:
        while (*str)
        {
            if ((x+(size*8))>X_SIZE)
            {
                x = 1;
                y = y + (size*8);
            }
            LCD_Draw_Char(x, y, color, bgcolor, *str, size);
            x += size*8-2;
            str++;
        }
        break;
    case 2:
        hh=1;
        while (*str)
        {
            if ((x+(size*8))>X_SIZE)
            {
                x = 1;
                y = y + (size*8);
            }
            LCD_Draw_Char(x,y,color,bgcolor,*str,size);
            x += hh*8;
            str++;
        }
        break;
    case 3:
        while (*str)
        {
            if ((x+11)>X_SIZE)
            {
                x = 1;
                y = y + 18;
            }
            LCD_Draw_Char(x,y,color,bgcolor,*str,size);
            x += 11;
            str++;
        }
        break;
    case 4:
        while (*str)
        {
            if ((x+16)>X_SIZE)
            {
                x = 1;
                y = y + 26;
            }
            LCD_Draw_Char(x,y,color,bgcolor,*str,size);
            x += 16;
            str++;
        }
        break;
    default:
        break;
    }
}

void LCD_Draw_Icon(unsigned int x0, unsigned int y0, unsigned int x_size, unsigned int y_size, const unsigned int icon[])
{
    uint64_t k = 0;
    for(uint32_t j = y0; j <  y0 + y_size; j++)
    {
        for(uint32_t i = x0; i <  x0 + x_size; i++)
        {
            LCD_Draw_Pixel(i, j, icon[k]);
            k++;
        }
    }
}
