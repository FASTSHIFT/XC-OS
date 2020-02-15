#ifndef __GAMEDEF_H
#define __GAMEDEF_H

#define DISP_CS_SET GPIO_HIGH(GPIOB,GPIO_Pin_12)
#define DISP_CS_CLR GPIO_LOW(GPIOB,GPIO_Pin_12)

#define DISP_RS_SET GPIO_HIGH(GPIOB,GPIO_Pin_13)
#define DISP_RS_CLR GPIO_LOW(GPIOB,GPIO_Pin_13)

#define DISP_RW_SET GPIO_HIGH(GPIOB,GPIO_Pin_14)
#define DISP_RW_CLR GPIO_LOW(GPIOB,GPIO_Pin_14)

#define DISP_PORT_WRITE(data) (GPIOC->ODR = (data))

#define DISP_CMD_SET_X     0x2A
#define DISP_CMD_SET_Y     0x2B
#define DISP_CMD_WRITE_RAM 0x2C
#define DISP_CMD_READ_RAM  0x2D

#define DISP_SEND(data) \
do{\
    DISP_PORT_WRITE((data));\
    DISP_RW_CLR;\
    DISP_RW_SET;\
}while(0)

#define DISP_SET_WINDOW(x0,y0,x1,y1) \
do{\
    DISP_RS_CLR;\
    DISP_SEND(DISP_CMD_SET_X);\
    DISP_RS_SET;\
    DISP_SEND(x0 >> 8);\
    DISP_SEND(0x00FF & x0);\
    DISP_SEND(x1 >> 8);\
    DISP_SEND(0x00FF & x1);\
    DISP_RS_CLR;\
    DISP_SEND(DISP_CMD_SET_Y);\
    DISP_RS_SET;\
    DISP_SEND(y0 >> 8);\
    DISP_SEND(0x00FF & y0);\
    DISP_SEND(y1 >> 8);\
    DISP_SEND(0x00FF & y1);\
    DISP_RS_CLR;\
    DISP_SEND(DISP_CMD_WRITE_RAM);\
}while(0)

#endif
