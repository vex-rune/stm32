/**
  ******************************************************************************
  * @file    lcd.h
  * @brief   LCD 驱动头文件 (FSMC 8080 接口, 兼容 ILI9341 等控制器)
  *
  * @note
  *   本驱动通过 STM32F1 的 FSMC (Flexible Static Memory Controller) 总线以
  *   8080 接口时序驱动 TFT LCD。硬件连接方式:
  *     - 数据线:  FSMC_D0 ~ FSMC_D15 (PD0/PD1/PD4/PD5/PD8~PD10/PD14/PD15/PE7~PE15)
  *     - 片选:    FSMC_NE4 -> PG12  (Bank4, 基址 0x6C000000)
  *     - 读使能:  FSMC_NOE -> PD4
  *     - 写使能:  FSMC_NWE -> PD5
  *     - 命令/数据切换: FSMC_A10 -> PG0 (A10=0 写命令, A10=1 写数据)
  *     - 背光:    PB0 (输出, 默认低)
  *     - 复位:    PG15 (输出, 默认高)
  *
  *   LCD_REG 寄存器地址 0x6C000000 -> A10=0 -> 命令通道
  *   LCD_RAM 数据地址 0x6C000800 -> A10=1 -> 数据通道
  *
  *   颜色编码采用 RGB565 格式, 共 16-bit, 实际像素数据写入 GRAM。
  *
  *   默认适配: Z350IT002 (320x480, ILI9486 控制器, 16-bit 8080 并口)
  *   如更换其他控制器, 调整 lcd_init_sequence() 中的寄存器配置即可,
  *   API 层 (LCD_Clear/Draw/Show) 不受影响。
  ******************************************************************************
  */
#ifndef __LCD_H
#define __LCD_H

#include "stm32f1xx_hal.h"

/*============================================================================
 * FSMC 总线地址映射 (Bank4 区域)
 *============================================================================*/

/**
 * @brief LCD 命令通道地址 (A10=0)
 * @note  对该地址执行写操作 -> LCD 视为命令
 */
#define LCD_REG    (*(volatile uint16_t *)0x6C000000)

/**
 * @brief LCD 数据通道地址 (A10=1)
 * @note  对该地址执行写操作 -> LCD 视为显示数据
 */
#define LCD_RAM    (*(volatile uint16_t *)0x6C000800)

/*============================================================================
 * LCD 控制引脚宏 (与 gpio.c 中的 MX_GPIO_Init 配置保持一致)
 *============================================================================*/

/** @defgroup LCD_BL 背光控制引脚 (PB0) */
#define LCD_BL_GPIO_PORT      GPIOB                 /* 背光 GPIO 端口 */
#define LCD_BL_GPIO_PIN       GPIO_PIN_0            /* 背光 GPIO 引脚 */
#define LCD_BL_SET()          HAL_GPIO_WritePin(LCD_BL_GPIO_PORT, LCD_BL_GPIO_PIN, GPIO_PIN_SET)    /* 打开背光 */
#define LCD_BL_CLR()          HAL_GPIO_WritePin(LCD_BL_GPIO_PORT, LCD_BL_GPIO_PIN, GPIO_PIN_RESET)  /* 关闭背光 */

/** @defgroup LCD_RST 复位控制引脚 (PG15) */
#define LCD_RST_GPIO_PORT     GPIOG                 /* 复位 GPIO 端口 */
#define LCD_RST_GPIO_PIN      GPIO_PIN_15           /* 复位 GPIO 引脚 */
#define LCD_RST_SET()         HAL_GPIO_WritePin(LCD_RST_GPIO_PORT, LCD_RST_GPIO_PIN, GPIO_PIN_SET)    /* 释放复位 (高) */
#define LCD_RST_CLR()         HAL_GPIO_WritePin(LCD_RST_GPIO_PORT, LCD_RST_GPIO_PIN, GPIO_PIN_RESET)  /* 拉低复位 */

/*============================================================================
 * RGB565 颜色宏
 *   格式: RRRRRGGGGGGBBBBB
 *          高 5 位 -> 红色
 *          中 6 位 -> 绿色
 *          低 5 位 -> 蓝色
 *============================================================================*/
#define WHITE               0xFFFF    /* 纯白 */
#define BLACK               0x0000    /* 纯黑 */
#define BLUE                0x001F    /* 纯蓝 */
#define BRED                0xF81F    /* 蓝红 (Brown Red, 偏紫红) */
#define GRED                0xFFE0    /* 绿红 (Green Red, 偏黄) */
#define GBLUE               0x07FF    /* 绿蓝 (Cyan, 青色) */
#define RED                 0xF800    /* 纯红 */
#define MAGENTA             0xF81F    /* 紫红 (品红) */
#define GREEN               0x07E0    /* 纯绿 */
#define CYAN                0x7FFF    /* 青色 */
#define YELLOW              0xFFE0    /* 黄色 */
#define BROWN               0xBC40    /* 棕色 */
#define BRRED               0xFC07    /* 棕红色 */
#define GRAY                0x8430    /* 灰色 */
#define DARKBLUE            0x01CF    /* 深蓝 */
#define LIGHTBLUE           0x7D7C    /* 浅蓝 */
#define GRAYBLUE            0x5458    /* 灰蓝 */
#define LIGHTGREEN          0x841F    /* 浅绿 */
#define LIGHTGRAY           0xEF5B    /* 浅灰 */
#define LGRAY               0xC618    /* 浅灰 (Light Gray) */
#define DARKGRAY            0x7BEF    /* 深灰 */

/*============================================================================
 * 字体大小
 *============================================================================*/

/*============================================================================
 * 显示方向宏 (传给 LCD_DisplayDir)
 *============================================================================*/
#define LCD_DIR_HORIZONTAL  0        /* 横屏: 480 x 320 (宽 x 高) */
#define LCD_DIR_VERTICAL    1        /* 竖屏: 320 x 480 (宽 x 高) */

/*============================================================================
 * LCD 设备参数结构体
 * @note  记录当前 LCD 的尺寸与方向信息, 便于后续绘图函数自动适配
 *============================================================================*/
typedef struct
{
 uint16_t width; /*!< LCD 宽度 (像素), 随方向变化 */
 uint16_t height; /*!< LCD 高度 (像素), 随方向变化 */
 uint16_t id; /*!< LCD 控制器 ID (如 0x9486) */
 uint8_t dir; /*!< 显示方向: 0 横屏 / 1 竖屏 */
} _lcd_dev;

/** 全局 LCD 设备对象, 在 lcd.c 中定义 */
extern _lcd_dev lcddev;

/*============================================================================
 * 函数原型
 *============================================================================*/

/**
 * @brief  LCD 初始化 (FSMC 复位 + ILI9486 寄存器配置 + 清屏)
 * @note   必须先调用 MX_FSMC_Init() 完成总线初始化
 *         适配 Z350IT002 模块 (320x480, ILI9486 控制器)
 */
void LCD_Init(void);

/**
 * @brief  打开 LCD 显示
 */
void LCD_DisplayOn(void);

/**
 * @brief  关闭 LCD 显示 (进入休眠)
 */
void LCD_DisplayOff(void);

/**
 * @brief  全屏填充单一颜色 (清屏)
 * @param  color  RGB565 颜色值, 例如 WHITE / BLACK
 */
void LCD_Clear(uint16_t color);


/**
 * @brief  设置显示方向, 并自动更新 lcddev.width/height
 * @param  dir  LCD_DIR_HORIZONTAL 或 LCD_DIR_VERTICAL
 */
void LCD_DisplayDir(uint8_t dir);


uint32_t LCD_ReadID(void);


void LCD_Write_Font(char str, int size, uint16_t x, uint16_t y, uint16_t color);
void LCD_Write_Fonts(char* str, int size, int len, uint16_t x, uint16_t y, uint16_t color);
void LCD_Write_C(const uint8_t font_c[], int size, uint16_t x, uint16_t y, uint16_t color);

#endif /* __LCD_H */
