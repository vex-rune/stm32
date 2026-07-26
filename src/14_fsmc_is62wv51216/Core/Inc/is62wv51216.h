#ifndef __IS62WV51216_H        // 头文件包含保护：如果没定义 __IS62WV51216_H
#define __IS62WV51216_H        // 就定义它，防止同一个头文件被多次包含

#include "main.h"              // 引入 main.h，里面定义了 uint16_t/uint32_t 等类型，以及 HAL 库宏

/*
 * FSMC Bank1 区域划分（STM32F103）：
 *   Bank1 NE1 -> 0x60000000
 *   Bank1 NE2 -> 0x64000000
 *   Bank1 NE3 -> 0x68000000   <-- IS62WV51216 挂在这里
 *   Bank1 NE4 -> 0x6C000000
 *
 * 为什么是 0x68000000：
 *   STM32F103 把外部存储区划成 4 个 Bank，每个 Bank 由一片选线（NE1~NE4）选通。
 *   IS62WV51216 的 CS 引脚接到 FSMC_NE3，因此它所有字节地址都必须落在
 *   0x68000000 ~ 0x680FFFFF 这 1MB 区域内。强制转换为 uint32_t 是为了避免
 *   在后续指针运算中被当成带符号数扩展成 64bit 地址而踩到高位存储区。
 */
// FSMC Bank1 NE3 起始地址
#define SRAM_BASE_ADDR  ((uint32_t)0x68000000)   // 定义宏：SRAM 字节基址 = 0x68000000（FSMC NE3 区首地址）
                                                // 外层 (uint32_t) 是强制类型转换，避免后续指针运算时符号扩展

/*
 * IS62WV51216 总容量 = 512K × 16bit = 1MByte。
 *
 * 为什么用"字数"而不是"字节数"表达长度：
 *   1) 屏蔽"每字 = 2 字节"的细节，调用者按"逻辑单元"思考更直观；
 *   2) 与 FSMC 16 位数据总线对齐，写入效率最高；
 *   3) 512*1024 = 524288 个 16bit 字恰好覆盖整片 1MB 物理空间，
 *      用于边界检查时再左移 1 位即可换算成字节末地址 0x67FFFF。
 */
// IS62WV51216 总16位字数：512*1024
#define SRAM_MAX_WORD   (512 * 1024)            // 定义宏：SRAM 总字数 = 524288（512K 个 16bit 字）

/*
 * volatile 必须加，原因有三：
 *   1) 这是 MMIO（Memory-Mapped IO），读写的是 SRAM 控制器而非普通 RAM，
 *      编译器不能把读到的值缓存到寄存器中，否则连续两次读同一地址
 *      会拿到相同的"旧值"；
 *   2) SRAM 内容可能被硬件（FSMC 总线、DMA 等）异步修改，volatile 阻止
 *      编译器做 CSE / 寄存器提升等优化；
 *   3) 写入侧：编译器可能把多次相同地址的写入合并，或者丢弃"看上去无副作用"
 *      的写，加 volatile 后每次赋值都会真正落到 FSMC 总线上。
 *
 * 用 uint16_t* 而非 uint8_t*：
 *   FSMC 数据线宽 16 位，按半字访问时总线利用率最高；
 *   IS62WV51216 的 UB#/LB#(NBL1/NBL0) 会随访问宽度自动控制，
 *   整字写时两个字节使能同时拉低，半字写时只拉低对应字节，
 *   因此不必担心写一个字节会破坏同一字内的另一字节。
 *
 * wordAddr << 1：把"字地址"换算成"字节地址"，因为每个 16bit 字占 2 字节。
 * static inline：这种单条赋值语句的函数会被编译器原地展开，
 *                省去函数调用开销，等价于直接访问内存。
 */
// 16位整字读写
static inline uint16_t SRAM_ReadWord(uint32_t wordAddr)   // 定义内联函数：读 1 个 16bit 字
{                                                          //   static inline -> 建议编译器原地展开，省去调用开销
                                                           //   uint16_t      -> 返回值类型，读到的 16bit 数据
                                                           //   SRAM_ReadWord -> 函数名
                                                           //   uint32_t wordAddr -> 入参：字地址（0 ~ SRAM_MAX_WORD-1）
    // wordAddr：0 ~ SRAM_MAX_WORD-1
    return *(volatile uint16_t *)(SRAM_BASE_ADDR + (wordAddr << 1));
    //  (wordAddr << 1)                       : 字地址 × 2 = 字节偏移
    //  SRAM_BASE_ADDR + (...)                : 得到 SRAM 中目标字的字节地址
    //  (volatile uint16_t *)                 : 把该地址强制解释成"易变的 16bit 指针"
    //  *(...)                                : 解引用，触发一次 FSMC 16 位读操作
    //  return                                : 把读到的 16bit 数据返回给调用者
}

static inline void SRAM_WriteWord(uint32_t wordAddr, uint16_t dat)  // 定义内联函数：写 1 个 16bit 字
{                                                                    //   void           -> 无返回值
                                                                     //   uint32_t wordAddr -> 目标字地址
                                                                     //   uint16_t dat   -> 要写入的 16bit 数据
    *(volatile uint16_t *)(SRAM_BASE_ADDR + (wordAddr << 1)) = dat;
    //  (wordAddr << 1)              : 字地址转字节偏移
    //  SRAM_BASE_ADDR + (...)       : 目标字节地址
    //  (volatile uint16_t *)        : 当成"易变的 16bit 指针"
    //  *... = dat                   : 通过该指针写 1 个 16bit 数据到 SRAM
}

/*
 * 单字节读写拆成 Low / High 两个函数，原因：
 *   IS62WV51216 提供独立的字节使能 NBL0（对应低字节 D[7:0]）和 NBL1（D[15:8]）。
 *   FSMC 检测到 8 位访问落在某个 16 位字的低字节地址时，会自动只拉低 NBL0、
 *   高字节保留原值；落在高字节地址时同理。
 *
 * 因此：
 *   低字节 (D[7:0])  -> 字节地址 (wordAddr << 1)
 *   高字节 (D[15:8]) -> 字节地址 (wordAddr << 1) + 1
 *
 * 直接用 uint8_t* 强制访问可让 FSMC 总线自动只选通 8 位，避免覆盖同一字
 * 内的另一字节，这就是"按字节读写"在硬件层面可行的根本原因。
 *
 * 使用前提：fsmc.c 中必须把 NE3 配置为"16 位数据宽度 + 独立 NBL 字节使能"
 * （即 SRAM/NOR 时序寄存器里的 MWID=01），否则字节访问可能无法正确选通。
 */
// 单字节读写（利用NBL0/NBL1）
static inline uint8_t SRAM_ReadByteLow(uint32_t wordAddr)  // 读某字内的低字节 (D[7:0])
{
    // wordAddr << 1                : 字地址 × 2 = 该字低字节的字节地址
    // SRAM_BASE_ADDR + (...)       : 得到 SRAM 中目标字节的真实地址
    // (volatile uint8_t *)         : 强制按 8 位访问，FSMC 会只拉低 NBL0
    // *(...)                        : 触发一次 FSMC 8 位读，返回低字节值
    return *(volatile uint8_t *)(SRAM_BASE_ADDR + (wordAddr << 1));
}

static inline void SRAM_WriteByteLow(uint32_t wordAddr, uint8_t dat)  // 写某字内的低字节 (D[7:0])
{
    // (wordAddr << 1)              : 字地址转低字节字节地址
    // SRAM_BASE_ADDR + (...)       : SRAM 真实地址
    // (volatile uint8_t *)         : 按 8 位写，FSMC 只拉低 NBL0，不会破坏高字节
    // *... = dat                    : 写入低字节
    *(volatile uint8_t *)(SRAM_BASE_ADDR + (wordAddr << 1)) = dat;
}

static inline uint8_t SRAM_ReadByteHigh(uint32_t wordAddr)  // 读某字内的高字节 (D[15:8])
{
    // (wordAddr << 1) + 1          : 字地址转高字节字节地址（比低字节多 1）
    // SRAM_BASE_ADDR + (...)       : SRAM 真实地址
    // (volatile uint8_t *)         : 按 8 位读，FSMC 只拉低 NBL1
    // *(...)                        : 触发一次 FSMC 8 位读，返回高字节值
    return *(volatile uint8_t *)(SRAM_BASE_ADDR + (wordAddr << 1) + 1);
}

static inline void SRAM_WriteByteHigh(uint32_t wordAddr, uint8_t dat)  // 写某字内的高字节 (D[15:8])
{
    // (wordAddr << 1) + 1          : 字地址转高字节字节地址
    // SRAM_BASE_ADDR + (...)       : SRAM 真实地址
    // (volatile uint8_t *)         : 按 8 位写，FSMC 只拉低 NBL1，低字节保持不变
    // *... = dat                    : 写入高字节
    *(volatile uint8_t *)(SRAM_BASE_ADDR + (wordAddr << 1) + 1) = dat;
}

/*
 * 批量读写放在 .c 中实现而非内联，原因：
 *   1) 循环体里要做循环变量与地址的计算，内联进调用点反而会让代码膨胀，
 *      占用更多 Flash 且影响 CPU 指令缓存命中率；
 *   2) 批量访问是相对低频但耗时的操作（典型 55ns/字 @72MHz 需 ~4 个 CPU 周期），
 *      单独的函数让调用方在必要时更容易用 DMA/IT 改造；
 *   3) 与单字/单字节读写分文件，便于在替换 FSMC 控制器或加锁时只动一处。
 *
 * 形参加 const（输入缓冲）和非 const（输出缓冲）是为了让编译器能基于
 * const 做只读优化，同时保证调用方不会意外修改输入数据。
 */
// 批量读写
void SRAM_WriteBurst(uint32_t startWord, const uint16_t *buf, uint32_t len);
// 函数声明：把 buf 指向的连续 len 个 16bit 数据写入 SRAM，从 startWord 字地址开始
//   uint32_t startWord        : 起始字地址（写入位置）
//   const uint16_t *buf       : 输入数据源指针（const 表示只读，防止误写）
//   uint32_t len              : 要写入的字数

void SRAM_ReadBurst(uint32_t startWord, uint16_t *buf, uint32_t len);
// 函数声明：从 SRAM 起始字地址 startWord 连续读出 len 个 16bit 数据到 buf
//   uint32_t startWord        : 起始字地址（读取位置）
//   uint16_t *buf             : 接收缓冲区指针（非 const，因为要写入结果）
//   uint32_t len              : 要读取的字数

#endif                          // 头文件包含保护结束，与开头的 #ifndef 配对