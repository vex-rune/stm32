# 点亮LED灯案例（寄存器）

我们首先使用基于寄存器开发方式点亮第一个LED灯。

在代码期间会涉及到一些寄存器知识，先不要深究，后面再详细介绍。现在要做的就是点亮第一个LED灯。


## 需求描述

快速体验STM32开发：点亮LED1。


## 硬件电路设计

LED1-LED3是普通LED灯，LED4为电源指示灯。

![image34.png](images/image34.png)

![image35.png](images/image35.png)

说明：


###### LED1连接的是端口PA0


###### 只要让PA0引脚输出低电平就可以点亮LED1。


## 创建项目工程


### 创建工程准备


##### 创建需要的目录

在磁盘上创建一个目录 atguigu，然后再在atguigu目录下创建我们的第一个工程目录 led_register。

![image36.png](images/image36.png)


##### 准备启动文件

STM32程序需要启动文件，我们需要提前准备好。先去ST官网下载官方提供的外设标准库，里面有提供标准的启动文件，我们从里面copy一份出来。

标准外设库下载地址：https://www.st.com/zh/embedded-software/stm32-standard-peripheral-libraries/products.html

![image37.png](images/image37.png)

![image38.png](images/image38.png)

下载完成之后解压，后面我们需要从里面copy文件出来。


##### 创建目录放入启动文件和其他核心文件

为了方便管理，我们把启动文件放入专门目录中。在刚才创建的工程目录中创建一个目录：Start（名字没有强制要求，随意，但是最好不要有中文）。

![image39.png](images/image39.png)

去到我们刚才下载的标准外设库目录，找到我们需要的启动文件和其他核心文件copy到Start目录。

![image40.png](images/image40.png)

启动文件选择标准：

![image41.png](images/image41.png)

![image42.png](images/image42.png)

![image43.png](images/image43.png)

Copy完之后：

![image44.png](images/image44.png)


##### 创建工程

打开Keil MDK创建工程。

![image45.png](images/image45.png)

![image46.png](images/image46.png)

![image47.png](images/image47.png)

![image48.png](images/image48.png)


### 工程配置


##### 添加两个Project Group方便管理代码文件

![image49.png](images/image49.png)

先删除默认的Source Group 1，再添加两个：Start（启动相关的文件），User（我们自己写的代码）。

![image50.png](images/image50.png)

![image51.png](images/image51.png)

![image52.png](images/image52.png)

![image53.png](images/image53.png)


##### 创建main.c 文件

![image54.png](images/image54.png)

![image55.png](images/image55.png)

User路径不存在，让它帮我们创建。

![image56.png](images/image56.png)


### 编译配置


##### 编译器版本改为5

目前最新的Keil ARM用的是 Compiler version 6，与前面的core_cm3.c不兼容，所以需要提前准备好Compiler version 5。

下载地址：https://developer.arm.com/downloads/view/ACOMP5

解压之后，把解压的后文件夹放入到Keil MDK的安装目录下：

![image57.png](images/image57.png)

![image58.png](images/image58.png)

![image59.png](images/image59.png)

![image60.png](images/image60.png)

![image61.png](images/image61.png)

![image62.png](images/image62.png)

![image63.png](images/image63.png)

![image64.png](images/image64.png)

![image65.png](images/image65.png)


##### 一些其他配置

![image66.png](images/image66.png)

![image67.png](images/image67.png)

![image68.png](images/image68.png)

![image69.png](images/image69.png)


## 软件设计


### main.c

今天的体验代码大家只需要知道最终能点亮LED1就行，具体的每行代码的为什么这么写到GPIO这章学完自会明白。

根据前面硬件电路设计，我们只要让GPIOA的0口输出低电平就行了。代码需要按照下面的步骤来实现。对于其中一些概念，大家先有个了解，后面的课程会细讲。


##### 开启时钟

在STM32中，让IO口工作，必须先开启对应的时钟。所以需要先查找到开启时钟的寄存器，然后通过该寄存器操作时钟的开启或关闭。我们要打开的是GPIOA的时钟。

![image70.png](images/image70.png)

![image71.png](images/image71.png)

![image72.png](images/image72.png)

我们需要知道RCC_APB2ENR这个寄存器的地址。如何查找呢？先知道RCC这个外设的基地址，然后加上这个寄存器的偏移地址就行了。

![image73.png](images/image73.png)

从上面可以看出来，RCC的基地址是0x4002 1000，APB2ENR的偏移量是0x18，所以APB2ENR的地址值是0x4002 1000 + 0x18

有了地址，在这个地址写入一个数据，这个数据的二进制第2位是1就行了。其他位暂时不管。我们写入4。这样就开启了GPIOA的时钟。

在代码中，我们需要把地址强转成指针才能给这个地址赋值。

```c
*(uint32_t *)(0x40021000 + 0x18) = 4;
```


##### 给IO口设置输出模式

在STM32中，如果要让IO口输出低电平或高电平，必须给要使用的IO设置为输出模式。

根据前面的思路，需要先找到GPIOA的基地址，再根据偏移地址找到要使用的寄存器的地址。GPIOA的基地址是0x4001 0800。

配置PA0口的输出模式的寄存器是GPIOA_CRL。

![image74.png](images/image74.png)

只需要让这个寄存器的最后4位是 0011，就是最大速度的推挽输出。

```c
*(uint32_t *)(0x40010800 + 0x00) = 3;
```


##### 给PA0口输出0

给指定PA0口输出0就可以点亮LED1了。用到的寄存器是ODR数据输出寄存器。

![image75.png](images/image75.png)

ODR寄存器的地址是 0x40010800 + 0x0c 。给这个地址的第0位写0，其他位写1。

```c
*(uint32_t *)((0x40010800 + 0x0c)) = 0xfffe;
```

main.c具体代码清单。

```c
#include "stdint.h"

int main(void)
{
    /* 开启GPIOA的时钟 */
    *(uint32_t *)(0x40021000 + 0x18) = 4;

    /* 给PA0设置为通用推挽输出 */
    *(uint32_t *)(0x40010800 + 0x00) = 3;

    /* 给输出寄存器赋值 */
    *(uint32_t *)(0x40010800 + 0x0c) = 0xfffe;
    while (1)
    {
    }
}
```


### 编译工程

![image76.png](images/image76.png)


### 安装ST-LINK驱动


###### 我们使用ST-LINK仿真器下载程序。

![image77.png](images/image77.png)


###### Keil的安装目录下自带了ST-LINK的USB驱动，双击安装即可。

![image78.png](images/image78.png)


###### 升级STLink固件

![image79.png](images/image79.png)

![image80.png](images/image80.png)

![image81.png](images/image81.png)

![image82.png](images/image82.png)

![image83.png](images/image83.png)


### Keil中配置ST-LINK 

还需要在Keil软件中，对仿真器做一些必要的配置。

![image84.png](images/image84.png)

![image85.png](images/image85.png)

![image86.png](images/image86.png)

![image87.png](images/image87.png)


### 下载程序

 

![image88.png](images/image88.png)


### 操作寄存器方式的“进化”


#### “进化”1

在操作寄存器的时候，如果每次都查手册计算地址，是相当麻烦且无聊。ST公司早就考虑到了这个问题，已经提前把每个外设寄存器的地址提前给我们用宏定义的方式给算好了，我们只需要直接使用即可。比如下面是定义的RCC各个寄存器地址。（stm32f10x.h中定义）

```c
#define PERIPH_BASE           ((uint32_t)0x40000000) 
#define AHBPERIPH_BASE        (PERIPH_BASE + 0x20000)
#define RCC_BASE              (AHBPERIPH_BASE + 0x1000)
#define RCC                 ((RCC_TypeDef *) RCC_BASE)
typedef struct
{
  __IO uint32_t CR;
  __IO uint32_t CFGR;
  __IO uint32_t CIR;
  __IO uint32_t APB2RSTR;
  __IO uint32_t APB1RSTR;
  __IO uint32_t AHBENR;
  __IO uint32_t APB2ENR;
  __IO uint32_t APB1ENR;
  __IO uint32_t BDCR;
  __IO uint32_t CSR;
} RCC_TypeDef;
```

这里还巧妙的运用了结构体中各个成员地址是连续的特征。CR寄存器是RCC第0个32位寄存器，所以它相对于基地址的偏移是0。CFGR相对于基地址的偏移是4，…

APB2ENR的相对于基地址的偏移是6*4=24=0x18，和我们前面查找手册的结果是一致的。

```c
#include "stm32f10x.h"

int main(void)
{
    RCC->APB2ENR = 4;
    GPIOA->CRL = 3;
    GPIOA->ODR = 0xfffe;
    while (1)
    {
    }
}
```

这样写起来是不是可读性就好很多了，而且也简单了很多。


#### “进化”2

其实在上面的代码中还有一些问题。在STM32中一个寄存器是32位的，我们在编写代码的时候只是需要给某位或某几位赋值。由于STM32不支持位寻址，所以在前面的操作中，我们其实是修改了所有位。这是非常不合理的，也许其他位在其他地方有赋值，我们重新赋值势必会覆盖了其他值，带来的后果也是很严重的。

如何只修改特定的位的值，而不影响其他位呢？我们需要先回顾下一些常见的位操作，再来继续进化上面的代码。


##### 常用的一些位操作回顾

```c
#include "stdio.h"
#include "stdlib.h"


void printfBinary(char * str, uint32_t num)
{
char buffer[33];
    itoa(num, buffer, 2); // 把result转成2进制字符串
    printf("%s = (%s)2 \n", str, buffer);
}

int main()
{
    /* 左移 8<<1 = 1000<<1 = 10000*/
    printfBinary("8 << 1", 8 << 1);

    /* 右移 8>>1 = 1000>>1 = 100*/
    printfBinary("8 >> 1", 8 >> 1);

    /* 按位或 8|7 = 1000|0111 = 1111 */
    printfBinary("8 | 7", 8 | 7);

    /* 按位或 8&7 = 1000&0111 = 0000 */
    printfBinary("8 & 7", 8 & 7);

    /* 按位取反 ~8 = ~1000 = 0111 */
    printfBinary("~8", ~8);

    /*
        把某位置 1  (0 位 1位 ...)
            比如把 num 的第 2 位置 1
                1. 得到一个数第 2 位是 1 其他都为 0
                   a =  0000 0100  是由 1<<2 得到
                2. 让 num | a
     */
    printfBinary("8置第 2 位为 1 ", 8 | (1 << 2));

    /*
        把连续的多位同时置 1  (0 位 1位 ...)
            比如把 num 的第 1和2 位置 1
                1  a =  3 << 1
                2. num | a
     */
    printfBinary("8置第 1和2 位为 1 ", 8 | (3 << 1));

    /*
        把某位置 0  (0位 1位 ...)
            比如把 num 的第 2 位置 0
                1. 得到一个数第 2 位是 0 其他都为 1
                   a =  1111 1011  是由 ~(1<<2) 得到
                2. 让 num & a
     */
    printfBinary("7置第 2 位为 0 ", 7 & ~(1 << 2));

    /*
        把连续多位同时置 0  (0位 1位 ...)
            比如把 num 的第 1和2 位置 0
                1. a = ~(3<<1)
                2. 让 num & a
     */
    printfBinary("7置第 1和2 位为 0 ", 7 & ~(3 << 1));

    /*
        把连续的多位同时置位  101 (二进制)
            比如把 num 的第 1,2,3 位置为 101
            1. num的 1,2,3位置为0
                num &= ~(7<<1)
            2. num |= (5 << 1);    (5 = 101)
     */

    unsigned char num = 13;
    num &= ~(7 << 1);
    num |= 5 << 1;
    printfBinary("13", 13);
    printfBinary("13的123位置为101 ", num);
}
```


##### 继续进化

根据前面基本位操作的回顾，我们综合使用这些位操作，可以把代码进化为下面的形式。

```c
#include "stm32f10x.h"

int main(void)
{
    /* 开启GPIOA的时钟 第2位置1*/
    RCC->APB2ENR |= 0x1 << 2;

    /* GPIOA_CRL的最后4位置 0011 */
    GPIOA->CRL &= ~(0x1 << 3);
    GPIOA->CRL &= ~(0x1 << 2);
    GPIOA->CRL |= 0x1 << 1;
    GPIOA->CRL |= 0x1 << 0;

    /* GPIOA_ODR的第0位置0 */
    GPIOA->ODR &= ~(0x1 << 0);

    while (1)
    {
    }
}
```


#### “进化”3

在上次的进化中，我们是给寄存器“或等”和“与等”了一些值，这些值都是通过相应的“移位”操作得到的。比如要操作第2位，就需要把0x1左移2位得到。我们需要查找手册才能知道要移位几。也是很不方便。

其实ST公司也把我们需要的移位后的值给提前计算好了，用宏定义的方式供我们使用。

比如前面的开启时钟，已经定义了好了这个值。正好就是1<<2

```c
#define  RCC_APB2ENR_IOPAEN                  ((uint32_t)0x00000004)
```

利用ST公司提前预定义的这些值，可以进一步进化代码为下面的形式。

```c
#include "stm32f10x.h"

int main(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    GPIOA->CRL &= ~GPIO_CRL_CNF0_1;
    GPIOA->CRL &= ~GPIO_CRL_CNF0_0;
    GPIOA->CRL |= GPIO_CRL_MODE0_1;
    GPIOA->CRL |= GPIO_CRL_MODE0_0;

    GPIOA->ODR &= ~GPIO_ODR_ODR0;

    while (1)
    {
    }
}
```

这样可读性就好多了，等操作熟练之后，很多操作不查找寄存器手册也能直接操作了。

