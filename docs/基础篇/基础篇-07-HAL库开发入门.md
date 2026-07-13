# HAL库开发入门


## HAL介绍


##### 为什么要学习HAL库开发

到目前我们已经学会1种开发SMT32程序的姿势：基于寄存器开发。

寄存器开发效率比较低，在工作中如果想快速开发还是要会用到HAL库开发。

HAL库是目前ST主力推广的开发方式，所以你如果还要用ST的芯片，会使用HAL库开发是势在必行的。而且最最关键的是，使用HAL开发有诸多的好处。

目前，HAL库已经支持STM32全线产品。


##### 什么是HAL库

HAL库的全称是Hardware Abstraction Layer，翻译成硬件抽象层。HAL库是ST为STM32最新推出的抽象层嵌入式软件，可以更好的确保跨STM32产品的最大可移植性。该库提供了一整套一致的中间件组件，如RTOS，USB，TCP/IP和图形等。

HAL库是基于一个非限制性的BSD许可协议（Berkeley Software Distribution）而发布的开源代码。 ST制作的中间件堆栈（USB主机和设备库，STemWin）带有允许轻松重用的许可模式，只要是在ST公司的MCU 芯片上使用，库中的中间件(USB 主机/设备库，STemWin)协议栈即被允许随便修改，并可以反复使用。至于基于其它著名的开源解决方案商的中间件（FreeRTOS，FatFs，LwIP和PolarSSL）也都具有友好的用户许可条款。


##### CMSIS和HAL库的关系

CMSIS（Cortex Microcontroller Software Interface Standard）是 Cortex-M 处理器系列的与供应商无关的硬件抽象层。它是ARM公司制定的一个标准。它可以为处理器和外设实现一致且简单的软件接口，从而简化软件的重用、缩短微控制器新开发人员的学习过程，并缩短新设备的上市时间。简单来说，就是ARM公司制定标准，芯片厂商按照此标准编写相应的程序，实现统一的接口，方便开发人员的使用。

![image133.png](images/image133.png)


## HAL库开发环境搭建 


### 安装Java运行环境

STM32CubeMX 要求java8的运行环境。下载地址：https://www.oracle.com/java/technologies/javase/javase8u211-later-archive-downloads.html

我给大家准备的是：jre-8u381-windows-x64.exe。 双击安装即可。


### 安装STM32CubeMX

是ST公司提供的一个工具，非常方便的用图形化的方式帮我们创建工程，和完成各种配置。

下载地址：https://www.st.com/zh/development-tools/stm32cubemx.html#get-software

双击安装即可。

![image134.png](images/image134.png)

![image135.png](images/image135.png)

![image136.png](images/image136.png)

![image137.png](images/image137.png)

![image138.png](images/image138.png)

![image139.png](images/image139.png)

![image140.png](images/image140.png)

![image141.png](images/image141.png)

![image142.png](images/image142.png)


### 在线下载芯片支持包

双击安装好的CubeMx。桌面会有快捷方式。

![image143.png](images/image143.png)

进入如下界面。

![image144.png](images/image144.png)

先安装下软件支持包：

![image145.png](images/image145.png)

![image146.png](images/image146.png)

点击安装之后，需要先登录你的ST账号，没有就先注册一个。

![image147.png](images/image147.png)


### 离线安装芯片支持包

如果网速不好，下载会比较慢。也可以选择离线安装。


##### 下载离线芯片支持包

下载地址：https://www.st.com/zh/development-tools/stm32cubemx.html#tools-software

 

![image148.png](images/image148.png)

![image149.png](images/image149.png)

![image150.png](images/image150.png)


##### 导入芯片支持包

![image151.png](images/image151.png)

![image152.png](images/image152.png)

![image153.png](images/image153.png)

注意:


###### 上面的离线安装方式只能安装基础包(en.stm32cubef1.zip),升级包(en.stm32cubef1-v1-8-5.zip)无法安装.


###### 把en.stm32cubef1-v1-8-5.zip拷贝到stm32cube的仓库中,直接解压就行了.仓库位置: C:\Users\你的用户名\STM32Cube\Repository. 用解压的文件覆盖原来的基础包.


## LED流水灯案例（HAL库）


### 需求描述

实现LED流水灯。用的GPIOA的 0、1、8口。


### 硬件电路设计

![image154.png](images/image154.png)

![image155.png](images/image155.png)


### 软件设计


#### 创建HAL库工程 


##### 操作页面介绍

![image156.png](images/image156.png)

![image157.png](images/image157.png)

![image158.png](images/image158.png)

![image159.png](images/image159.png)

![image160.png](images/image160.png)

![image161.png](images/image161.png)

![image162.png](images/image162.png)

![image163.png](images/image163.png)


##### 根据需求设置参数


###### Debug参数选择

默认debug选项没有设置，需要开启。选择串口即可。这个地方务必设置好，否则只能下载一次程序到MCU，以后就无法下载了。

![image164.png](images/image164.png)


###### RCC时钟选择

![image165.png](images/image165.png)

![image166.png](images/image166.png)

![image167.png](images/image167.png)


###### GPIO配置

![image168.png](images/image168.png)


###### 配置PA0引脚。

![image169.png](images/image169.png)


###### 同样的方式配置PA1和PA8，然后可以看到这3个引脚。

![image170.png](images/image170.png)


###### 分别对他们进行设置。

![image171.png](images/image171.png)

![image172.png](images/image172.png)

![image173.png](images/image173.png)


###### 工程配置

![image174.png](images/image174.png)

![image175.png](images/image175.png)

![image176.png](images/image176.png)


###### 生成代码

![image177.png](images/image177.png)

![image178.png](images/image178.png)

![image179.png](images/image179.png)


##### 生成的工程目录介绍

![image180.png](images/image180.png)


#### 自动生成的代码解读

main函数中已经替我们调用了HAL的初始化，时钟的初始化，GPIO的初始化。

```c
int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    while (1)
    {
    }
}
```

下面是GPIO初始化的代码。

```c
void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOA, LED1_Pin | LED2_Pin | LED3_Pin, GPIO_PIN_SET);

    /*Configure GPIO pins : PAPin PAPin PAPin */
    GPIO_InitStruct.Pin = LED1_Pin | LED2_Pin | LED3_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}
```


#### 构建LED驱动

为了方便管理，我们把LED驱动放入单独的文件夹管理。在项目根目录下创建Hardware/led目录。

然后在Hardware/led目录下创建2个文件led.h和led.c。

并在Keil中添加如下配置。

![image181.png](images/image181.png)

![image182.png](images/image182.png)

![image183.png](images/image183.png)


##### led.h

```c
#ifndef __led_h
#define __led_h

#include "gpio.h"

#define LED uint16_t
void LED_On(LED led);
void LED_Off(LED led);
void LED_Toggle(LED led);

void LED_OffAll(LED led[], uint8_t size);

#endif
```


##### led.c

```c
#include "led.h"

/**
 * @description: 点亮LED
 * @param {LED} led
 */
void LED_On(LED led)
{
    HAL_GPIO_WritePin(GPIOA, led, GPIO_PIN_RESET);
}

/**
 * @description: 熄灭LED
 * @param {LED} led
 */
void LED_Off(LED led)
{
    HAL_GPIO_WritePin(GPIOA, led, GPIO_PIN_SET);
}

/**
 * @description: 翻转LED的状态
 * @param {LED} led
 */
void LED_Toggle(LED led)
{
    HAL_GPIO_TogglePin(GPIOA, led);
}

/**
 * @description: 关闭所有LED
 * @param {LED} led
 * @param {uint8_t} len
 * @return {*}
 */
void LED_OffAll(LED led[], uint8_t size)
{
    uint8_t i;
    for (i = 0; i < size; i++)
    {
        LED_Off(led[i]);
    }
}
```


#### main函数

```c
#include "LED.h"
int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    LED leds[] = {LED1_Pin, LED2_Pin, LED3_Pin};
    uint8_t i;

    while (1)
    {
        for (i = 0; i < 3; i++)
        {
            LED_OffAll(leds, 3);
            LED_On(leds[i]);
            HAL_Delay(200);
        }

        for (i = 0; i < 3; i++)
        {
            LED_OffAll(leds, 3);
            LED_On(leds[2 - i]);
            HAL_Delay(200);
        }
    }
}
```


#### 注意

每新建一个工程，都需要重新配置Include Path和Debug参数。

