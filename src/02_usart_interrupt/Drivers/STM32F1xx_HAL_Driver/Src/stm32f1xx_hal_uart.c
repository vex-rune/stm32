/**
  ******************************************************************************
  * @file    stm32f1xx_hal_uart.c
  * @author  MCD Application Team
  * @brief   UART HAL 模块驱动。
  *          本文件提供固件函数来管理通用异步收发器 (UART) 外设的以下功能：
  *           + 初始化和反初始化函数
  *           + IO 操作函数
  *           + 外设控制函数
  *           + 外设状态和错误函数
  *
  ******************************************************************************
  * @attention
  *
  * 版权所有 (c) 2016 STMicroelectronics。
  * 保留所有权利。
  *
  * 本软件根据可在软件组件根目录的 LICENSE 文件中找到的条款进行许可。
  * 如果本软件未随附 LICENSE 文件，则按"原样"提供。
  *
  ******************************************************************************
  @verbatim
  ==============================================================================
                        ##### 如何使用本驱动 #####
  ==============================================================================
  [..]
    UART HAL 驱动可按如下方式使用：

    (#) 声明一个 UART_HandleTypeDef 句柄结构体（例如 UART_HandleTypeDef huart）。
    (#) 通过实现 HAL_UART_MspInit() API 初始化 UART 低层资源：
        (##) 使能 USARTx 接口时钟。
        (##) UART 引脚配置：
            (+++) 使能 UART GPIO 的时钟。
            (+++) 将 UART TX/RX 引脚配置为复用功能上拉模式。
        (##) 如果需要使用中断处理（HAL_UART_Transmit_IT()
             和 HAL_UART_Receive_IT() API），则进行 NVIC 配置：
            (+++) 配置 USARTx 中断优先级。
            (+++) 使能 NVIC 中的 USART IRQ 处理。
        (##) 如果需要使用 DMA 处理（HAL_UART_Transmit_DMA()
             和 HAL_UART_Receive_DMA() API），则进行 DMA 配置：
            (+++) 为 Tx/Rx 通道声明一个 DMA 句柄结构体。
            (+++) 使能 DMAx 接口时钟。
            (+++) 使用所需的 Tx/Rx 参数配置已声明的 DMA 句柄结构体。
            (+++) 配置 DMA Tx/Rx 通道。
            (+++) 将初始化好的 DMA 句柄关联到 UART 的 DMA Tx/Rx 句柄。
            (+++) 配置 DMA Tx/Rx 通道传输完成中断的优先级并使能 NVIC。
            (+++) 配置 USARTx 中断优先级并使能 NVIC 中的 USART IRQ 处理
                  (用于 DMA 非循环模式下最后一个字节发送完成检测)

    (#) 在 huart Init 结构体中编程设置波特率、数据位长度、停止位、校验位、
        硬件流控制和模式（接收器/发送器）。

    (#) 对于 UART 异步模式，调用 HAL_UART_Init() API 初始化 UART 寄存器。

    (#) 对于 UART 半双工模式，调用 HAL_HalfDuplex_Init() API 初始化 UART 寄存器。

    (#) 对于 LIN 模式，调用 HAL_LIN_Init() API 初始化 UART 寄存器。

    (#) 对于多处理器模式，调用 HAL_MultiProcessor_Init() API 初始化 UART 寄存器。

     [..]
       (@) 特定的 UART 中断（发送完成中断、RXNE 中断和错误中断）将在发送
            和接收过程中使用宏 __HAL_UART_ENABLE_IT() 和 __HAL_UART_DISABLE_IT()
            进行管理。

     [..]
       (@) 这些 API（HAL_UART_Init() 和 HAL_HalfDuplex_Init()）还会通过调用
            用户自定义的 HAL_UART_MspInit() API 来配置低层硬件（GPIO、CLOCK、
            CORTEX 等）。

    ##### 回调注册 #####
    ==================================

    [..]
    当编译选项 USE_HAL_UART_REGISTER_CALLBACKS 设置为 1 时，
    允许用户动态配置驱动回调。

    [..]
    使用函数 HAL_UART_RegisterCallback() 来注册用户回调。
    函数 HAL_UART_RegisterCallback() 允许注册以下回调：
    (+) TxHalfCpltCallback        : Tx 半传输完成回调。
    (+) TxCpltCallback            : Tx 传输完成回调。
    (+) RxHalfCpltCallback        : Rx 半传输完成回调。
    (+) RxCpltCallback            : Rx 传输完成回调。
    (+) ErrorCallback             : 错误回调。
    (+) AbortCpltCallback         : 终止完成回调。
    (+) AbortTransmitCpltCallback : 发送终止完成回调。
    (+) AbortReceiveCpltCallback  : 接收终止完成回调。
    (+) MspInitCallback           : UART MspInit。
    (+) MspDeInitCallback         : UART MspDeInit。
    此函数接受 HAL 外设句柄、回调 ID 和指向用户回调函数的指针作为参数。

    [..]
    使用函数 HAL_UART_UnRegisterCallback() 将回调重置为默认的
    weak（可重载）函数。
    HAL_UART_UnRegisterCallback() 接受 HAL 外设句柄和回调 ID 作为参数。
    此函数允许重置以下回调：
    (+) TxHalfCpltCallback        : Tx 半传输完成回调。
    (+) TxCpltCallback            : Tx 传输完成回调。
    (+) RxHalfCpltCallback        : Rx 半传输完成回调。
    (+) RxCpltCallback            : Rx 传输完成回调。
    (+) ErrorCallback             : 错误回调。
    (+) AbortCpltCallback         : 终止完成回调。
    (+) AbortTransmitCpltCallback : 发送终止完成回调。
    (+) AbortReceiveCpltCallback  : 接收终止完成回调。
    (+) MspInitCallback           : UART MspInit。
    (+) MspDeInitCallback         : UART MspDeInit。

    [..]
    对于特定的回调 RxEventCallback，请使用专用的注册/重置函数：
    分别为 HAL_UART_RegisterRxEventCallback() 和 HAL_UART_UnRegisterRxEventCallback()。

    [..]
    默认情况下，在 HAL_UART_Init() 之后且状态为 HAL_UART_STATE_RESET 时，
    所有回调都设置为对应的 weak（可重载）函数：
    例如 HAL_UART_TxCpltCallback()、HAL_UART_RxHalfCpltCallback()。
    MspInit 和 MspDeInit 函数为例外情况，它们分别在 HAL_UART_Init()
    和 HAL_UART_DeInit() 中仅在这些回调为空（之前未注册）时才被重置为
    旧版 weak（可重载）函数。
    如果 MspInit 或 MspDeInit 不为空，HAL_UART_Init() 和 HAL_UART_DeInit()
    将保留并使用用户的 MspInit/MspDeInit 回调（之前已注册）。

    [..]
    回调只能在 HAL_UART_STATE_READY 状态下注册/注销。
    MspInit/MspDeInit 为例外，可以在 HAL_UART_STATE_READY
    或 HAL_UART_STATE_RESET 状态下注册/注销，因此已注册（用户）的
    MspInit/DeInit 回调可以在 Init/DeInit 期间使用。
    在这种情况下，需要先在调用 HAL_UART_DeInit()
    或 HAL_UART_Init() 函数之前，使用 HAL_UART_RegisterCallback()
    注册 MspInit/MspDeInit 用户回调。

    [..]
    当编译选项 USE_HAL_UART_REGISTER_CALLBACKS 设置为 0 或
    未定义时，回调注册功能不可用，
    使用 weak（可重载）回调。

     [..]
        本驱动提供三种操作模式：

     *** 轮询模式 IO 操作 ***
     =================================
     [..]
       (+) 使用 HAL_UART_Transmit() 在阻塞模式下发送一定数量的数据
       (+) 使用 HAL_UART_Receive() 在阻塞模式下接收一定数量的数据

     *** 中断模式 IO 操作 ***
     ===================================
     [..]
       (+) 使用 HAL_UART_Transmit_IT() 在非阻塞模式下发送一定数量的数据
       (+) 发送结束时执行 HAL_UART_TxCpltCallback，用户可以通过
            自定义函数指针 HAL_UART_TxCpltCallback 添加自己的代码
       (+) 使用 HAL_UART_Receive_IT() 在非阻塞模式下接收一定数量的数据
       (+) 接收结束时执行 HAL_UART_RxCpltCallback，用户可以通过
            自定义函数指针 HAL_UART_RxCpltCallback 添加自己的代码
       (+) 发生传输错误时，执行 HAL_UART_ErrorCallback() 函数，用户可以
            通过自定义函数指针 HAL_UART_ErrorCallback 添加自己的代码

     *** DMA 模式 IO 操作 ***
     ==============================
     [..]
       (+) 使用 HAL_UART_Transmit_DMA() 在非阻塞模式（DMA）下发送一定数量的数据
       (+) 半传输结束时执行 HAL_UART_TxHalfCpltCallback，用户可以通过
            自定义函数指针 HAL_UART_TxHalfCpltCallback 添加自己的代码
       (+) 传输结束时执行 HAL_UART_TxCpltCallback，用户可以通过
            自定义函数指针 HAL_UART_TxCpltCallback 添加自己的代码
       (+) 使用 HAL_UART_Receive_DMA() 在非阻塞模式（DMA）下接收一定数量的数据
       (+) 半传输结束时执行 HAL_UART_RxHalfCpltCallback，用户可以通过
            自定义函数指针 HAL_UART_RxHalfCpltCallback 添加自己的代码
       (+) 传输结束时执行 HAL_UART_RxCpltCallback，用户可以通过
            自定义函数指针 HAL_UART_RxCpltCallback 添加自己的代码
       (+) 发生传输错误时，执行 HAL_UART_ErrorCallback() 函数，用户可以
            通过自定义函数指针 HAL_UART_ErrorCallback 添加自己的代码
       (+) 使用 HAL_UART_DMAPause() 暂停 DMA 传输
       (+) 使用 HAL_UART_DMAResume() 恢复 DMA 传输
       (+) 使用 HAL_UART_DMAStop() 停止 DMA 传输


    [..] 本小节还提供了一组为用户提供增强型接收服务的附加函数。
        (例如，这些函数允许应用程序处理要接收的数据数量未知的场景)。

    (#) 与仅将已接收数据元素数量作为接收完成条件的标准接收服务相比，
        这些函数还考虑将其他事件作为更新接收状态至调用方的触发器：
       (+) 空闲时段检测（RX 线在给定时段内没有活动）。
          (++) 通过 IDLE 事件检测 RX 空闲，即在最后一个接收字节之后，
               RX 线在空闲状态（通常为高电平）保持 1 帧时间。

    (#) 有两种传输模式：
       (+) 阻塞模式：接收以轮询模式执行，直到接收到预期数量的数据，
           或直到发生 IDLE 事件。接收仅在函数执行期间进行处理。
           当函数退出时，不会进行数据接收。HAL 状态和实际接收的数据元素数量，
           在传输结束后由函数返回。
       (+) 非阻塞模式：接收使用中断或 DMA 执行。
           这些 API 返回 HAL 状态。
           数据处理结束将通过专用 UART IRQ（中断模式）或 DMA IRQ（DMA 模式）指示。
           HAL_UARTEx_RxEventCallback() 用户回调将在接收过程中执行
           当检测到接收错误时，将执行 HAL_UART_ErrorCallback() 用户回调。

    (#) 阻塞模式 API：
        (+) HAL_UARTEx_ReceiveToIdle()

    (#) 非阻塞模式 API（中断）：
        (+) HAL_UARTEx_ReceiveToIdle_IT()

    (#) 非阻塞模式 API（DMA）：
        (+) HAL_UARTEx_ReceiveToIdle_DMA()


     *** UART HAL 驱动宏列表 ***
     =============================================
     [..]
       以下是 UART HAL 驱动中最常用的宏列表。

      (+) __HAL_UART_ENABLE: 使能 UART 外设
      (+) __HAL_UART_DISABLE: 禁用 UART 外设
      (+) __HAL_UART_GET_FLAG : 检查指定 UART 标志是否置位
      (+) __HAL_UART_CLEAR_FLAG : 清除指定的 UART 待处理标志
      (+) __HAL_UART_ENABLE_IT: 使能指定的 UART 中断
      (+) __HAL_UART_DISABLE_IT: 禁用指定的 UART 中断
      (+) __HAL_UART_GET_IT_SOURCE: 检查指定的 UART 中断是否已发生

     [..]
       (@) 可以参考 UART HAL 驱动头文件获取更多有用的宏

  @endverbatim
     [..]
       (@) 附加说明：如果使能了校验位，那么写入数据寄存器的数据的最高位
           （MSB）会被发送，但会被校验位所改变。
           根据 M 位（8 位或 9 位）定义的帧长度，
           可能的 UART 帧格式如下表所示：
    +-------------------------------------------------------------+
    |   M 位  |  PCE 位  |            UART 帧                    |
    |---------------------|---------------------------------------|
    |    0    |    0      |    | SB | 8 位数据 | STB |            |
    |---------|-----------|---------------------------------------|
    |    0    |    1      |    | SB | 7 位数据 | PB | STB |       |
    |---------|-----------|---------------------------------------|
    |    1    |    0      |    | SB | 9 位数据 | STB |            |
    |---------|-----------|---------------------------------------|
    |    1    |    1      |    | SB | 8 位数据 | PB | STB |       |
    +-------------------------------------------------------------+
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/** @addtogroup STM32F1xx_HAL_Driver
  * @{
  */

/** @defgroup UART UART
  * @brief HAL UART module driver
  * @{
  */
#ifdef HAL_UART_MODULE_ENABLED

/* 私有类型定义 -------------------------------------------------------------*/
/* 私有宏定义 ----------------------------------------------------------------*/
/** @addtogroup UART_Private_Constants
  * @{
  */
/**
  * @}
  */
/* 私有宏 -------------------------------------------------------------------*/
/* 私有变量 -----------------------------------------------------------------*/
/* 私有函数原型 -------------------------------------------------------------*/
/** @addtogroup UART_Private_Functions  UART 私有函数
  * @{
  */

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
void UART_InitCallbacksToDefault(UART_HandleTypeDef *huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
static void UART_EndTxTransfer(UART_HandleTypeDef *huart);
static void UART_EndRxTransfer(UART_HandleTypeDef *huart);
static void UART_DMATransmitCplt(DMA_HandleTypeDef *hdma);
static void UART_DMAReceiveCplt(DMA_HandleTypeDef *hdma);
static void UART_DMATxHalfCplt(DMA_HandleTypeDef *hdma);
static void UART_DMARxHalfCplt(DMA_HandleTypeDef *hdma);
static void UART_DMAError(DMA_HandleTypeDef *hdma);
static void UART_DMAAbortOnError(DMA_HandleTypeDef *hdma);
static void UART_DMATxAbortCallback(DMA_HandleTypeDef *hdma);
static void UART_DMARxAbortCallback(DMA_HandleTypeDef *hdma);
static void UART_DMATxOnlyAbortCallback(DMA_HandleTypeDef *hdma);
static void UART_DMARxOnlyAbortCallback(DMA_HandleTypeDef *hdma);
static HAL_StatusTypeDef UART_Transmit_IT(UART_HandleTypeDef *huart);
static HAL_StatusTypeDef UART_EndTransmit_IT(UART_HandleTypeDef *huart);
static HAL_StatusTypeDef UART_Receive_IT(UART_HandleTypeDef *huart);
static HAL_StatusTypeDef UART_WaitOnFlagUntilTimeout(UART_HandleTypeDef *huart, uint32_t Flag, FlagStatus Status,
                                                     uint32_t Tickstart, uint32_t Timeout);
static void UART_SetConfig(UART_HandleTypeDef *huart);

/**
  * @}
  */

/* 导出函数 -----------------------------------------------------------------*/
/** @defgroup UART_Exported_Functions UART 导出函数
  * @{
  */

/** @defgroup UART_Exported_Functions_Group1 初始化和反初始化函数
  *  @brief    初始化和配置函数
  *
@verbatim
 ===============================================================================
            ##### 初始化和配置函数 #####
 ===============================================================================
    [..]
    本小节提供了一组函数，允许在异步模式下初始化 USARTx 或 UARTy。
      (+) 对于异步模式，只能配置以下参数：
        (++) 波特率
        (++) 数据位长度
        (++) 停止位
        (++) 校验位：如果使能了校验位，那么写入数据寄存器的数据的最高位
             （MSB）会被发送，但会被校验位所改变。
             根据 M 位（8 位或 9 位）定义的帧长度，
             请参考参考手册获取可能的 UART 帧格式。
        (++) 硬件流控制
        (++) 接收器/发送器模式
        (++) 过采样方法
    [..]
    HAL_UART_Init()、HAL_HalfDuplex_Init()、HAL_LIN_Init() 和 HAL_MultiProcessor_Init()
    API 分别遵循 UART 异步、UART 半双工、LIN 和多处理器配置流程
    （流程详情可在参考手册中获得：STM32F10Xxx MCU 为 RM0008，
     STM32F100xx MCU 为 RM0041）。

@endverbatim
  * @{
  */

/**
  * @brief  根据 UART_InitTypeDef 中指定的参数初始化 UART 模式，并创建关联句柄。
  * @param  huart  指向 UART_HandleTypeDef 结构体的指针，其中包含指定 UART 模块的配置信息。
  * @retval HAL 状态
  */
HAL_StatusTypeDef HAL_UART_Init(UART_HandleTypeDef *huart)
{
  /* 检查 UART 句柄分配 */
  if (huart == NULL)
  {
    return HAL_ERROR;
  }

  /* 检查参数 */
  if (huart->Init.HwFlowCtl != UART_HWCONTROL_NONE)
  {
    /* 硬件流控制仅在 USART1、USART2 和 USART3 上可用 */
    assert_param(IS_UART_HWFLOW_INSTANCE(huart->Instance));
    assert_param(IS_UART_HARDWARE_FLOW_CONTROL(huart->Init.HwFlowCtl));
  }
  else
  {
    assert_param(IS_UART_INSTANCE(huart->Instance));
  }
  assert_param(IS_UART_WORD_LENGTH(huart->Init.WordLength));
#if defined(USART_CR1_OVER8)
  assert_param(IS_UART_OVERSAMPLING(huart->Init.OverSampling));
#endif /* USART_CR1_OVER8 */

  if (huart->gState == HAL_UART_STATE_RESET)
  {
    /* 分配锁资源并初始化 */
    huart->Lock = HAL_UNLOCKED;

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
    UART_InitCallbacksToDefault(huart);

    if (huart->MspInitCallback == NULL)
    {
      huart->MspInitCallback = HAL_UART_MspInit;
    }

    /* 初始化低层硬件 */
    huart->MspInitCallback(huart);
#else
    /* 初始化低层硬件：GPIO、CLOCK */
    HAL_UART_MspInit(huart);
#endif /* (USE_HAL_UART_REGISTER_CALLBACKS) */
  }

  huart->gState = HAL_UART_STATE_BUSY;

  /* 禁用外设 */
  __HAL_UART_DISABLE(huart);

  /* 设置 UART 通信参数 */
  UART_SetConfig(huart);

  /* 在异步模式下，以下位必须保持清零：
     - USART_CR2 寄存器中的 LINEN 和 CLKEN 位，
     - USART_CR3 寄存器中的 SCEN、HDSEL 和 IREN 位。*/
  CLEAR_BIT(huart->Instance->CR2, (USART_CR2_LINEN | USART_CR2_CLKEN));
  CLEAR_BIT(huart->Instance->CR3, (USART_CR3_SCEN | USART_CR3_HDSEL | USART_CR3_IREN));

  /* 使能外设 */
  __HAL_UART_ENABLE(huart);

  /* 初始化 UART 状态 */
  huart->ErrorCode = HAL_UART_ERROR_NONE;
  huart->gState = HAL_UART_STATE_READY;
  huart->RxState = HAL_UART_STATE_READY;
  huart->RxEventType = HAL_UART_RXEVENT_TC;

  return HAL_OK;
}

/**
  * @brief  Initializes the half-duplex mode according to the specified
  *         parameters in the UART_InitTypeDef and create the associated handle.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HalfDuplex_Init(UART_HandleTypeDef *huart)
{
  /* Check the UART handle allocation */
  if (huart == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_UART_HALFDUPLEX_INSTANCE(huart->Instance));
  assert_param(IS_UART_WORD_LENGTH(huart->Init.WordLength));
#if defined(USART_CR1_OVER8)
  assert_param(IS_UART_OVERSAMPLING(huart->Init.OverSampling));
#endif /* USART_CR1_OVER8 */

  if (huart->gState == HAL_UART_STATE_RESET)
  {
    /* Allocate lock resource and initialize it */
    huart->Lock = HAL_UNLOCKED;

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
    UART_InitCallbacksToDefault(huart);

    if (huart->MspInitCallback == NULL)
    {
      huart->MspInitCallback = HAL_UART_MspInit;
    }

    /* Init the low level hardware */
    huart->MspInitCallback(huart);
#else
    /* Init the low level hardware : GPIO, CLOCK */
    HAL_UART_MspInit(huart);
#endif /* (USE_HAL_UART_REGISTER_CALLBACKS) */
  }

  huart->gState = HAL_UART_STATE_BUSY;

  /* Disable the peripheral */
  __HAL_UART_DISABLE(huart);

  /* Set the UART Communication parameters */
  UART_SetConfig(huart);

  /* In half-duplex mode, the following bits must be kept cleared:
     - LINEN and CLKEN bits in the USART_CR2 register,
     - SCEN and IREN bits in the USART_CR3 register.*/
  CLEAR_BIT(huart->Instance->CR2, (USART_CR2_LINEN | USART_CR2_CLKEN));
  CLEAR_BIT(huart->Instance->CR3, (USART_CR3_IREN | USART_CR3_SCEN));

  /* Enable the Half-Duplex mode by setting the HDSEL bit in the CR3 register */
  SET_BIT(huart->Instance->CR3, USART_CR3_HDSEL);

  /* Enable the peripheral */
  __HAL_UART_ENABLE(huart);

  /* Initialize the UART state*/
  huart->ErrorCode = HAL_UART_ERROR_NONE;
  huart->gState = HAL_UART_STATE_READY;
  huart->RxState = HAL_UART_STATE_READY;
  huart->RxEventType = HAL_UART_RXEVENT_TC;

  return HAL_OK;
}

/**
  * @brief  Initializes the LIN mode according to the specified
  *         parameters in the UART_InitTypeDef and create the associated handle.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @param  BreakDetectLength Specifies the LIN break detection length.
  *         This parameter can be one of the following values:
  *            @arg UART_LINBREAKDETECTLENGTH_10B: 10-bit break detection
  *            @arg UART_LINBREAKDETECTLENGTH_11B: 11-bit break detection
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_LIN_Init(UART_HandleTypeDef *huart, uint32_t BreakDetectLength)
{
  /* Check the UART handle allocation */
  if (huart == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the LIN UART instance */
  assert_param(IS_UART_LIN_INSTANCE(huart->Instance));

  /* Check the Break detection length parameter */
  assert_param(IS_UART_LIN_BREAK_DETECT_LENGTH(BreakDetectLength));
  assert_param(IS_UART_LIN_WORD_LENGTH(huart->Init.WordLength));
#if defined(USART_CR1_OVER8)
  assert_param(IS_UART_LIN_OVERSAMPLING(huart->Init.OverSampling));
#endif /* USART_CR1_OVER8 */

  if (huart->gState == HAL_UART_STATE_RESET)
  {
    /* Allocate lock resource and initialize it */
    huart->Lock = HAL_UNLOCKED;

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
    UART_InitCallbacksToDefault(huart);

    if (huart->MspInitCallback == NULL)
    {
      huart->MspInitCallback = HAL_UART_MspInit;
    }

    /* Init the low level hardware */
    huart->MspInitCallback(huart);
#else
    /* Init the low level hardware : GPIO, CLOCK */
    HAL_UART_MspInit(huart);
#endif /* (USE_HAL_UART_REGISTER_CALLBACKS) */
  }

  huart->gState = HAL_UART_STATE_BUSY;

  /* Disable the peripheral */
  __HAL_UART_DISABLE(huart);

  /* Set the UART Communication parameters */
  UART_SetConfig(huart);

  /* In LIN mode, the following bits must be kept cleared:
     - CLKEN bits in the USART_CR2 register,
     - SCEN, HDSEL and IREN bits in the USART_CR3 register.*/
  CLEAR_BIT(huart->Instance->CR2, (USART_CR2_CLKEN));
  CLEAR_BIT(huart->Instance->CR3, (USART_CR3_HDSEL | USART_CR3_IREN | USART_CR3_SCEN));

  /* Enable the LIN mode by setting the LINEN bit in the CR2 register */
  SET_BIT(huart->Instance->CR2, USART_CR2_LINEN);

  /* Set the USART LIN Break detection length. */
  CLEAR_BIT(huart->Instance->CR2, USART_CR2_LBDL);
  SET_BIT(huart->Instance->CR2, BreakDetectLength);

  /* Enable the peripheral */
  __HAL_UART_ENABLE(huart);

  /* Initialize the UART state*/
  huart->ErrorCode = HAL_UART_ERROR_NONE;
  huart->gState = HAL_UART_STATE_READY;
  huart->RxState = HAL_UART_STATE_READY;
  huart->RxEventType = HAL_UART_RXEVENT_TC;

  return HAL_OK;
}

/**
  * @brief  Initializes the Multi-Processor mode according to the specified
  *         parameters in the UART_InitTypeDef and create the associated handle.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @param  Address USART address
  * @param  WakeUpMethod specifies the USART wake-up method.
  *         This parameter can be one of the following values:
  *            @arg UART_WAKEUPMETHOD_IDLELINE: Wake-up by an idle line detection
  *            @arg UART_WAKEUPMETHOD_ADDRESSMARK: Wake-up by an address mark
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_MultiProcessor_Init(UART_HandleTypeDef *huart, uint8_t Address, uint32_t WakeUpMethod)
{
  /* Check the UART handle allocation */
  if (huart == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_UART_INSTANCE(huart->Instance));

  /* Check the Address & wake up method parameters */
  assert_param(IS_UART_WAKEUPMETHOD(WakeUpMethod));
  assert_param(IS_UART_ADDRESS(Address));
  assert_param(IS_UART_WORD_LENGTH(huart->Init.WordLength));
#if defined(USART_CR1_OVER8)
  assert_param(IS_UART_OVERSAMPLING(huart->Init.OverSampling));
#endif /* USART_CR1_OVER8 */

  if (huart->gState == HAL_UART_STATE_RESET)
  {
    /* Allocate lock resource and initialize it */
    huart->Lock = HAL_UNLOCKED;

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
    UART_InitCallbacksToDefault(huart);

    if (huart->MspInitCallback == NULL)
    {
      huart->MspInitCallback = HAL_UART_MspInit;
    }

    /* Init the low level hardware */
    huart->MspInitCallback(huart);
#else
    /* Init the low level hardware : GPIO, CLOCK */
    HAL_UART_MspInit(huart);
#endif /* (USE_HAL_UART_REGISTER_CALLBACKS) */
  }

  huart->gState = HAL_UART_STATE_BUSY;

  /* Disable the peripheral */
  __HAL_UART_DISABLE(huart);

  /* Set the UART Communication parameters */
  UART_SetConfig(huart);

  /* In Multi-Processor mode, the following bits must be kept cleared:
     - LINEN and CLKEN bits in the USART_CR2 register,
     - SCEN, HDSEL and IREN  bits in the USART_CR3 register */
  CLEAR_BIT(huart->Instance->CR2, (USART_CR2_LINEN | USART_CR2_CLKEN));
  CLEAR_BIT(huart->Instance->CR3, (USART_CR3_SCEN | USART_CR3_HDSEL | USART_CR3_IREN));

  /* Set the USART address node */
  CLEAR_BIT(huart->Instance->CR2, USART_CR2_ADD);
  SET_BIT(huart->Instance->CR2, Address);

  /* Set the wake up method by setting the WAKE bit in the CR1 register */
  CLEAR_BIT(huart->Instance->CR1, USART_CR1_WAKE);
  SET_BIT(huart->Instance->CR1, WakeUpMethod);

  /* Enable the peripheral */
  __HAL_UART_ENABLE(huart);

  /* Initialize the UART state */
  huart->ErrorCode = HAL_UART_ERROR_NONE;
  huart->gState = HAL_UART_STATE_READY;
  huart->RxState = HAL_UART_STATE_READY;
  huart->RxEventType = HAL_UART_RXEVENT_TC;

  return HAL_OK;
}

/**
  * @brief  DeInitializes the UART peripheral.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_UART_DeInit(UART_HandleTypeDef *huart)
{
  /* Check the UART handle allocation */
  if (huart == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_UART_INSTANCE(huart->Instance));

  huart->gState = HAL_UART_STATE_BUSY;

  /* Disable the Peripheral */
  __HAL_UART_DISABLE(huart);

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
  if (huart->MspDeInitCallback == NULL)
  {
    huart->MspDeInitCallback = HAL_UART_MspDeInit;
  }
  /* DeInit the low level hardware */
  huart->MspDeInitCallback(huart);
#else
  /* DeInit the low level hardware */
  HAL_UART_MspDeInit(huart);
#endif /* (USE_HAL_UART_REGISTER_CALLBACKS) */

  huart->ErrorCode = HAL_UART_ERROR_NONE;
  huart->gState = HAL_UART_STATE_RESET;
  huart->RxState = HAL_UART_STATE_RESET;
  huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;
  huart->RxEventType = HAL_UART_RXEVENT_TC;

  /* Process Unlock */
  __HAL_UNLOCK(huart);

  return HAL_OK;
}

/**
  * @brief  UART MSP Init.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */
__weak void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);
  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_UART_MspInit could be implemented in the user file
   */
}

/**
  * @brief  UART MSP DeInit.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */
__weak void HAL_UART_MspDeInit(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);
  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_UART_MspDeInit could be implemented in the user file
   */
}

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
/**
  * @brief  注册用户 UART 回调
  *         用于替代 weak 预定义回调
  * @note   HAL_UART_RegisterCallback() 可在 HAL_UART_Init()、HAL_HalfDuplex_Init()、HAL_LIN_Init() 之前调用，
  *         HAL_MultiProcessor_Init() 用于为 HAL_UART_MSPINIT_CB_ID 和 HAL_UART_MSPDEINIT_CB_ID 注册回调
  * @param  huart uart 句柄
  * @param  CallbackID 要注册的回调 ID
  *         此参数可以是以下值之一：
  *           @arg @ref HAL_UART_TX_HALFCOMPLETE_CB_ID Tx 半完成回调 ID
  *           @arg @ref HAL_UART_TX_COMPLETE_CB_ID Tx 完成回调 ID
  *           @arg @ref HAL_UART_RX_HALFCOMPLETE_CB_ID Rx 半完成回调 ID
  *           @arg @ref HAL_UART_RX_COMPLETE_CB_ID Rx 完成回调 ID
  *           @arg @ref HAL_UART_ERROR_CB_ID 错误回调 ID
  *           @arg @ref HAL_UART_ABORT_COMPLETE_CB_ID 终止完成回调 ID
  *           @arg @ref HAL_UART_ABORT_TRANSMIT_COMPLETE_CB_ID 发送终止完成回调 ID
  *           @arg @ref HAL_UART_ABORT_RECEIVE_COMPLETE_CB_ID 接收终止完成回调 ID
  *           @arg @ref HAL_UART_MSPINIT_CB_ID MspInit 回调 ID
  *           @arg @ref HAL_UART_MSPDEINIT_CB_ID MspDeInit 回调 ID
  * @param  pCallback 指向回调函数的指针
  * @retval HAL 状态
  */
HAL_StatusTypeDef HAL_UART_RegisterCallback(UART_HandleTypeDef *huart, HAL_UART_CallbackIDTypeDef CallbackID,
                                            pUART_CallbackTypeDef pCallback)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (pCallback == NULL)
  {
    /* 更新错误代码 */
    huart->ErrorCode |= HAL_UART_ERROR_INVALID_CALLBACK;

    return HAL_ERROR;
  }

  if (huart->gState == HAL_UART_STATE_READY)
  {
    switch (CallbackID)
    {
      case HAL_UART_TX_HALFCOMPLETE_CB_ID :
        huart->TxHalfCpltCallback = pCallback;
        break;

      case HAL_UART_TX_COMPLETE_CB_ID :
        huart->TxCpltCallback = pCallback;
        break;

      case HAL_UART_RX_HALFCOMPLETE_CB_ID :
        huart->RxHalfCpltCallback = pCallback;
        break;

      case HAL_UART_RX_COMPLETE_CB_ID :
        huart->RxCpltCallback = pCallback;
        break;

      case HAL_UART_ERROR_CB_ID :
        huart->ErrorCallback = pCallback;
        break;

      case HAL_UART_ABORT_COMPLETE_CB_ID :
        huart->AbortCpltCallback = pCallback;
        break;

      case HAL_UART_ABORT_TRANSMIT_COMPLETE_CB_ID :
        huart->AbortTransmitCpltCallback = pCallback;
        break;

      case HAL_UART_ABORT_RECEIVE_COMPLETE_CB_ID :
        huart->AbortReceiveCpltCallback = pCallback;
        break;

      case HAL_UART_MSPINIT_CB_ID :
        huart->MspInitCallback = pCallback;
        break;

      case HAL_UART_MSPDEINIT_CB_ID :
        huart->MspDeInitCallback = pCallback;
        break;

      default :
        /* 更新错误代码 */
        huart->ErrorCode |= HAL_UART_ERROR_INVALID_CALLBACK;

        /* 返回错误状态 */
        status =  HAL_ERROR;
        break;
    }
  }
  else if (huart->gState == HAL_UART_STATE_RESET)
  {
    switch (CallbackID)
    {
      case HAL_UART_MSPINIT_CB_ID :
        huart->MspInitCallback = pCallback;
        break;

      case HAL_UART_MSPDEINIT_CB_ID :
        huart->MspDeInitCallback = pCallback;
        break;

      default :
        /* 更新错误代码 */
        huart->ErrorCode |= HAL_UART_ERROR_INVALID_CALLBACK;

        /* 返回错误状态 */
        status =  HAL_ERROR;
        break;
    }
  }
  else
  {
    /* 更新错误代码 */
    huart->ErrorCode |= HAL_UART_ERROR_INVALID_CALLBACK;

    /* 返回错误状态 */
    status =  HAL_ERROR;
  }

  return status;
}

/**
  * @brief  注销 UART 回调
  *         UART 回调重定向到 weak 预定义回调
  * @note   HAL_UART_UnRegisterCallback() 可在 HAL_UART_Init()、HAL_HalfDuplex_Init() 之前调用，
  *         HAL_LIN_Init()、HAL_MultiProcessor_Init() 用于注销 HAL_UART_MSPINIT_CB_ID 的回调
  *         和 HAL_UART_MSPDEINIT_CB_ID
  * @param  huart uart 句柄
  * @param  CallbackID 要注销的回调 ID
  *         此参数可以是以下值之一：
  *           @arg @ref HAL_UART_TX_HALFCOMPLETE_CB_ID Tx 半完成回调 ID
  *           @arg @ref HAL_UART_TX_COMPLETE_CB_ID Tx 完成回调 ID
  *           @arg @ref HAL_UART_RX_HALFCOMPLETE_CB_ID Rx 半完成回调 ID
  *           @arg @ref HAL_UART_RX_COMPLETE_CB_ID Rx 完成回调 ID
  *           @arg @ref HAL_UART_ERROR_CB_ID 错误回调 ID
  *           @arg @ref HAL_UART_ABORT_COMPLETE_CB_ID 终止完成回调 ID
  *           @arg @ref HAL_UART_ABORT_TRANSMIT_COMPLETE_CB_ID 发送终止完成回调 ID
  *           @arg @ref HAL_UART_ABORT_RECEIVE_COMPLETE_CB_ID 接收终止完成回调 ID
  *           @arg @ref HAL_UART_MSPINIT_CB_ID MspInit 回调 ID
  *           @arg @ref HAL_UART_MSPDEINIT_CB_ID MspDeInit 回调 ID
  * @retval HAL 状态
  */
HAL_StatusTypeDef HAL_UART_UnRegisterCallback(UART_HandleTypeDef *huart, HAL_UART_CallbackIDTypeDef CallbackID)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (HAL_UART_STATE_READY == huart->gState)
  {
    switch (CallbackID)
    {
      case HAL_UART_TX_HALFCOMPLETE_CB_ID :
        huart->TxHalfCpltCallback = HAL_UART_TxHalfCpltCallback;               /* 旧版 weak TxHalfCpltCallback       */
        break;

      case HAL_UART_TX_COMPLETE_CB_ID :
        huart->TxCpltCallback = HAL_UART_TxCpltCallback;                       /* 旧版 weak TxCpltCallback            */
        break;

      case HAL_UART_RX_HALFCOMPLETE_CB_ID :
        huart->RxHalfCpltCallback = HAL_UART_RxHalfCpltCallback;               /* 旧版 weak RxHalfCpltCallback        */
        break;

      case HAL_UART_RX_COMPLETE_CB_ID :
        huart->RxCpltCallback = HAL_UART_RxCpltCallback;                       /* 旧版 weak RxCpltCallback            */
        break;

      case HAL_UART_ERROR_CB_ID :
        huart->ErrorCallback = HAL_UART_ErrorCallback;                         /* 旧版 weak ErrorCallback             */
        break;

      case HAL_UART_ABORT_COMPLETE_CB_ID :
        huart->AbortCpltCallback = HAL_UART_AbortCpltCallback;                 /* 旧版 weak AbortCpltCallback         */
        break;

      case HAL_UART_ABORT_TRANSMIT_COMPLETE_CB_ID :
        huart->AbortTransmitCpltCallback = HAL_UART_AbortTransmitCpltCallback; /* 旧版 weak AbortTransmitCpltCallback */
        break;

      case HAL_UART_ABORT_RECEIVE_COMPLETE_CB_ID :
        huart->AbortReceiveCpltCallback = HAL_UART_AbortReceiveCpltCallback;   /* 旧版 weak AbortReceiveCpltCallback  */
        break;

      case HAL_UART_MSPINIT_CB_ID :
        huart->MspInitCallback = HAL_UART_MspInit;                             /* 旧版 weak MspInitCallback           */
        break;

      case HAL_UART_MSPDEINIT_CB_ID :
        huart->MspDeInitCallback = HAL_UART_MspDeInit;                         /* 旧版 weak MspDeInitCallback         */
        break;

      default :
        /* 更新错误代码 */
        huart->ErrorCode |= HAL_UART_ERROR_INVALID_CALLBACK;

        /* 返回错误状态 */
        status =  HAL_ERROR;
        break;
    }
  }
  else if (HAL_UART_STATE_RESET == huart->gState)
  {
    switch (CallbackID)
    {
      case HAL_UART_MSPINIT_CB_ID :
        huart->MspInitCallback = HAL_UART_MspInit;
        break;

      case HAL_UART_MSPDEINIT_CB_ID :
        huart->MspDeInitCallback = HAL_UART_MspDeInit;
        break;

      default :
        /* 更新错误代码 */
        huart->ErrorCode |= HAL_UART_ERROR_INVALID_CALLBACK;

        /* 返回错误状态 */
        status =  HAL_ERROR;
        break;
    }
  }
  else
  {
    /* 更新错误代码 */
    huart->ErrorCode |= HAL_UART_ERROR_INVALID_CALLBACK;

    /* 返回错误状态 */
    status =  HAL_ERROR;
  }

  return status;
}

/**
  * @brief  注册用户 UART Rx 事件回调
  *         用于替代 weak 预定义回调
  * @param  huart     Uart 句柄
  * @param  pCallback 指向 Rx 事件回调函数的指针
  * @retval HAL 状态
  */
HAL_StatusTypeDef HAL_UART_RegisterRxEventCallback(UART_HandleTypeDef *huart, pUART_RxEventCallbackTypeDef pCallback)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (pCallback == NULL)
  {
    huart->ErrorCode |= HAL_UART_ERROR_INVALID_CALLBACK;

    return HAL_ERROR;
  }

  /* 进程锁定 */
  __HAL_LOCK(huart);

  if (huart->gState == HAL_UART_STATE_READY)
  {
    huart->RxEventCallback = pCallback;
  }
  else
  {
    huart->ErrorCode |= HAL_UART_ERROR_INVALID_CALLBACK;

    status =  HAL_ERROR;
  }

  /* 释放锁 */
  __HAL_UNLOCK(huart);

  return status;
}

/**
  * @brief  注销 UART Rx 事件回调
  *         UART Rx 事件回调重定向到 weak HAL_UARTEx_RxEventCallback() 预定义回调
  * @param  huart     Uart 句柄
  * @retval HAL 状态
  */
HAL_StatusTypeDef HAL_UART_UnRegisterRxEventCallback(UART_HandleTypeDef *huart)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* 进程锁定 */
  __HAL_LOCK(huart);

  if (huart->gState == HAL_UART_STATE_READY)
  {
    huart->RxEventCallback = HAL_UARTEx_RxEventCallback; /* 旧版 weak UART Rx 事件回调  */
  }
  else
  {
    huart->ErrorCode |= HAL_UART_ERROR_INVALID_CALLBACK;

    status =  HAL_ERROR;
  }

  /* 释放锁 */
  __HAL_UNLOCK(huart);
  return status;
}
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */

/**
  * @}
  */

/** @defgroup UART_Exported_Functions_Group2 IO 操作函数
  *  @brief UART 发送和接收函数
  *
@verbatim
 ===============================================================================
                      ##### IO 操作函数 #####
 ===============================================================================
    本小节提供了一组函数，允许管理 UART 异步
    和半双工数据传输。

    (#) 有两种传输模式：
       (+) 阻塞模式：通信以轮询模式执行。
           所有数据处理的 HAL 状态由同一函数在传输完成后返回。
       (+) 非阻塞模式：通信使用中断或 DMA 执行，这些 API 返回 HAL 状态。
           数据处理结束将通过专用 UART IRQ（中断模式）或 DMA IRQ（DMA 模式）指示。
           HAL_UART_TxCpltCallback()、HAL_UART_RxCpltCallback() 用户回调
           将分别在发送或接收过程结束时执行
           当检测到通信错误时，将执行 HAL_UART_ErrorCallback() 用户回调。

    (#) 阻塞模式 API 有：
        (+) HAL_UART_Transmit()
        (+) HAL_UART_Receive()

    (#) 非阻塞模式（中断）API 有：
        (+) HAL_UART_Transmit_IT()
        (+) HAL_UART_Receive_IT()
        (+) HAL_UART_IRQHandler()

    (#) 非阻塞模式（DMA）API 有：
        (+) HAL_UART_Transmit_DMA()
        (+) HAL_UART_Receive_DMA()
        (+) HAL_UART_DMAPause()
        (+) HAL_UART_DMAResume()
        (+) HAL_UART_DMAStop()

    (#) 在非阻塞模式下提供了一组传输完成回调：
        (+) HAL_UART_TxHalfCpltCallback()
        (+) HAL_UART_TxCpltCallback()
        (+) HAL_UART_RxHalfCpltCallback()
        (+) HAL_UART_RxCpltCallback()
        (+) HAL_UART_ErrorCallback()

    (#) 可以使用终止 API 中止非阻塞模式传输：
        (+) HAL_UART_Abort()
        (+) HAL_UART_AbortTransmit()
        (+) HAL_UART_AbortReceive()
        (+) HAL_UART_Abort_IT()
        (+) HAL_UART_AbortTransmit_IT()
        (+) HAL_UART_AbortReceive_IT()

    (#) 对于基于中断的终止服务（HAL_UART_Abortxxx_IT），提供了一组终止完成回调：
        (+) HAL_UART_AbortCpltCallback()
        (+) HAL_UART_AbortTransmitCpltCallback()
        (+) HAL_UART_AbortReceiveCpltCallback()

    (#) 对于增强接收服务的非阻塞模式，提供了 Rx 事件接收回调（Rx 事件通知）：
        (+) HAL_UARTEx_RxEventCallback()

    (#) 在非阻塞模式传输中，可能的错误分为两类。
        错误处理如下：
       (+) 错误被视为可恢复且非阻塞：传输可以继续到结束，但错误严重性
           需要由用户评估：这涉及中断模式接收中的帧错误、校验错误或噪声错误。
           接收到的字符然后被检索并存储在 Rx 缓冲区中，设置错误代码以允许用户识别错误类型，
           并执行 HAL_UART_ErrorCallback() 用户回调。UART 端传输保持运行。
           如果用户想要中止它，应调用中止服务。
       (+) 错误被视为阻塞：传输无法正确完成并被中止。
           这涉及中断模式接收中的溢出错误和 DMA 模式中的所有错误。
           设置错误代码以允许用户识别错误类型，并执行 HAL_UART_ErrorCallback() 用户回调。

    -@- 在半双工通信中，禁止并行运行发送和接收过程，
        UART 状态 HAL_UART_STATE_BUSY_TX_RX 无法使用。

@endverbatim
  * @{
  */

/**
  * @brief  在阻塞模式下发送一定数量的数据。
  * @note   当 UART 校验未启用（PCE = 0），且字长配置为 9 位（M1-M0 = 01）时，
  *         发送的数据作为一组 u16 处理。在这种情况下，Size 必须指示
  *         通过 pData 提供的 u16 数量。
  * @param  huart 指向 UART_HandleTypeDef 结构体的指针，其中包含
  *               指定 UART 模块的配置信息。
  * @param  pData 指向数据缓冲区（u8 或 u16 数据元素）的指针。
  * @param  Size  要发送的数据元素数量（u8 或 u16）
  * @param  Timeout 超时持续时间
  * @retval HAL 状态
  */
HAL_StatusTypeDef HAL_UART_Transmit(UART_HandleTypeDef *huart, const uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
  const uint8_t  *pdata8bits;
  const uint16_t *pdata16bits;
  uint32_t tickstart = 0U;

  /* 检查 Tx 过程是否尚未进行 */
  if (huart->gState == HAL_UART_STATE_READY)
  {
    if ((pData == NULL) || (Size == 0U))
    {
      return  HAL_ERROR;
    }

    huart->ErrorCode = HAL_UART_ERROR_NONE;
    huart->gState = HAL_UART_STATE_BUSY_TX;

    /* 初始化 tickstart 用于超时管理 */
    tickstart = HAL_GetTick();

    huart->TxXferSize = Size;
    huart->TxXferCount = Size;

    /* 对于 9 位/无校验传输，pData 需要作为 uint16_t 指针处理 */
    if ((huart->Init.WordLength == UART_WORDLENGTH_9B) && (huart->Init.Parity == UART_PARITY_NONE))
    {
      pdata8bits  = NULL;
      pdata16bits = (const uint16_t *) pData;
    }
    else
    {
      pdata8bits  = pData;
      pdata16bits = NULL;
    }

    while (huart->TxXferCount > 0U)
    {
      if (UART_WaitOnFlagUntilTimeout(huart, UART_FLAG_TXE, RESET, tickstart, Timeout) != HAL_OK)
      {
        huart->gState = HAL_UART_STATE_READY;

        return HAL_TIMEOUT;
      }
      if (pdata8bits == NULL)
      {
        huart->Instance->DR = (uint16_t)(*pdata16bits & 0x01FFU);
        pdata16bits++;
      }
      else
      {
        huart->Instance->DR = (uint8_t)(*pdata8bits & 0xFFU);
        pdata8bits++;
      }
      huart->TxXferCount--;
    }

    if (UART_WaitOnFlagUntilTimeout(huart, UART_FLAG_TC, RESET, tickstart, Timeout) != HAL_OK)
    {
      huart->gState = HAL_UART_STATE_READY;

      return HAL_TIMEOUT;
    }

    /* 在 Tx 过程结束时，将 huart->gState 恢复为 Ready */
    huart->gState = HAL_UART_STATE_READY;

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  在阻塞模式下接收一定数量的数据。
  * @note   当 UART 校验未启用（PCE = 0），且字长配置为 9 位（M1-M0 = 01）时，
  *         接收的数据作为一组 u16 处理。在这种情况下，Size 必须指示
  *         通过 pData 可用的 u16 数量。
  * @param  huart 指向 UART_HandleTypeDef 结构体的指针，其中包含
  *               指定 UART 模块的配置信息。
  * @param  pData 指向数据缓冲区（u8 或 u16 数据元素）的指针。
  * @param  Size  要接收的数据元素数量（u8 或 u16）。
  * @param  Timeout 超时持续时间
  * @retval HAL 状态
  */
HAL_StatusTypeDef HAL_UART_Receive(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
  uint8_t  *pdata8bits;
  uint16_t *pdata16bits;
  uint32_t tickstart = 0U;

  /* 检查 Rx 过程是否尚未进行 */
  if (huart->RxState == HAL_UART_STATE_READY)
  {
    if ((pData == NULL) || (Size == 0U))
    {
      return  HAL_ERROR;
    }

    huart->ErrorCode = HAL_UART_ERROR_NONE;
    huart->RxState = HAL_UART_STATE_BUSY_RX;
    huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;

    /* 初始化 tickstart 用于超时管理 */
    tickstart = HAL_GetTick();

    huart->RxXferSize = Size;
    huart->RxXferCount = Size;

    /* 对于 9 位/无校验传输，pRxData 需要作为 uint16_t 指针处理 */
    if ((huart->Init.WordLength == UART_WORDLENGTH_9B) && (huart->Init.Parity == UART_PARITY_NONE))
    {
      pdata8bits  = NULL;
      pdata16bits = (uint16_t *) pData;
    }
    else
    {
      pdata8bits  = pData;
      pdata16bits = NULL;
    }

    /* 检查剩余要接收的数据 */
    while (huart->RxXferCount > 0U)
    {
      if (UART_WaitOnFlagUntilTimeout(huart, UART_FLAG_RXNE, RESET, tickstart, Timeout) != HAL_OK)
      {
        huart->RxState = HAL_UART_STATE_READY;

        return HAL_TIMEOUT;
      }
      if (pdata8bits == NULL)
      {
        *pdata16bits = (uint16_t)(huart->Instance->DR & 0x01FF);
        pdata16bits++;
      }
      else
      {
        if ((huart->Init.WordLength == UART_WORDLENGTH_9B) || ((huart->Init.WordLength == UART_WORDLENGTH_8B) && (huart->Init.Parity == UART_PARITY_NONE)))
        {
          *pdata8bits = (uint8_t)(huart->Instance->DR & (uint8_t)0x00FF);
        }
        else
        {
          *pdata8bits = (uint8_t)(huart->Instance->DR & (uint8_t)0x007F);
        }
        pdata8bits++;
      }
      huart->RxXferCount--;
    }

    /* 在 Rx 过程结束时，将 huart->RxState 恢复为 Ready */
    huart->RxState = HAL_UART_STATE_READY;

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  在非阻塞模式下发送一定数量的数据。
  * @note   当 UART 校验未启用（PCE = 0），且字长配置为 9 位（M1-M0 = 01）时，
  *         发送的数据作为一组 u16 处理。在这种情况下，Size 必须指示
  *         通过 pData 提供的 u16 数量。
  * @param  huart 指向 UART_HandleTypeDef 结构体的指针，其中包含
  *               指定 UART 模块的配置信息。
  * @param  pData 指向数据缓冲区（u8 或 u16 数据元素）的指针。
  * @param  Size  要发送的数据元素数量（u8 或 u16）
  * @retval HAL 状态
  */
HAL_StatusTypeDef HAL_UART_Transmit_IT(UART_HandleTypeDef *huart, const uint8_t *pData, uint16_t Size)
{
  /* 检查 Tx 过程是否尚未进行 */
  if (huart->gState == HAL_UART_STATE_READY)
  {
    if ((pData == NULL) || (Size == 0U))
    {
      return HAL_ERROR;
    }

    huart->pTxBuffPtr = pData;
    huart->TxXferSize = Size;
    huart->TxXferCount = Size;

    huart->ErrorCode = HAL_UART_ERROR_NONE;
    huart->gState = HAL_UART_STATE_BUSY_TX;

    /* 使能 UART 发送数据寄存器空中断 */
    __HAL_UART_ENABLE_IT(huart, UART_IT_TXE);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  在非阻塞模式下接收一定数量的数据。
  * @note   当 UART 校验未启用（PCE = 0），且字长配置为 9 位（M1-M0 = 01）时，
  *         接收的数据作为一组 u16 处理。在这种情况下，Size 必须指示
  *         通过 pData 可用的 u16 数量。
  * @param  huart 指向 UART_HandleTypeDef 结构体的指针，其中包含
  *               指定 UART 模块的配置信息。
  * @param  pData 指向数据缓冲区（u8 或 u16 数据元素）的指针。
  * @param  Size  要接收的数据元素数量（u8 或 u16）。
  * @retval HAL 状态
  */
HAL_StatusTypeDef HAL_UART_Receive_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
  /* 检查 Rx 过程是否尚未进行 */
  if (huart->RxState == HAL_UART_STATE_READY)
  {
    if ((pData == NULL) || (Size == 0U))
    {
      return HAL_ERROR;
    }

    /* 将接收类型设置为标准接收 */
    huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;

    return (UART_Start_Receive_IT(huart, pData, Size));
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  在 DMA 模式下发送一定数量的数据。
  * @note   当 UART 校验未启用（PCE = 0），且字长配置为 9 位（M1-M0 = 01）时，
  *         发送的数据作为一组 u16 处理。在这种情况下，Size 必须指示
  *         通过 pData 提供的 u16 数量。
  * @param  huart  指向 UART_HandleTypeDef 结构体的指针，其中包含
  *                指定 UART 模块的配置信息。
  * @param  pData 指向数据缓冲区（u8 或 u16 数据元素）的指针。
  * @param  Size  要发送的数据元素数量（u8 或 u16）
  * @retval HAL 状态
  */
HAL_StatusTypeDef HAL_UART_Transmit_DMA(UART_HandleTypeDef *huart, const uint8_t *pData, uint16_t Size)
{
  const uint32_t *tmp;

  /* 检查 Tx 过程是否尚未进行 */
  if (huart->gState == HAL_UART_STATE_READY)
  {
    if ((pData == NULL) || (Size == 0U))
    {
      return HAL_ERROR;
    }

    huart->pTxBuffPtr = pData;
    huart->TxXferSize = Size;
    huart->TxXferCount = Size;

    huart->ErrorCode = HAL_UART_ERROR_NONE;
    huart->gState = HAL_UART_STATE_BUSY_TX;

    /* 设置 UART DMA 传输完成回调 */
    huart->hdmatx->XferCpltCallback = UART_DMATransmitCplt;

    /* 设置 UART DMA 半传输完成回调 */
    huart->hdmatx->XferHalfCpltCallback = UART_DMATxHalfCplt;

    /* 设置 DMA 错误回调 */
    huart->hdmatx->XferErrorCallback = UART_DMAError;

    /* 设置 DMA 终止回调 */
    huart->hdmatx->XferAbortCallback = NULL;

    /* 使能 UART 发送 DMA 通道 */
    tmp = (const uint32_t *)&pData;
    HAL_DMA_Start_IT(huart->hdmatx, *(const uint32_t *)tmp, (uint32_t)&huart->Instance->DR, Size);

    /* 通过写入 0 清除 SR 寄存器中的 TC 标志 */
    __HAL_UART_CLEAR_FLAG(huart, UART_FLAG_TC);

    /* 通过在 UART CR3 寄存器中设置 DMAT 位
       使能发送请求的 DMA 传输 */
    ATOMIC_SET_BIT(huart->Instance->CR3, USART_CR3_DMAT);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  在 DMA 模式下接收一定数量的数据。
  * @note   当 UART 校验未启用（PCE = 0），且字长配置为 9 位（M1-M0 = 01）时，
  *         接收的数据作为一组 u16 处理。在这种情况下，Size 必须指示
  *         通过 pData 可用的 u16 数量。
  * @param  huart 指向 UART_HandleTypeDef 结构体的指针，其中包含
  *               指定 UART 模块的配置信息。
  * @param  pData 指向数据缓冲区（u8 或 u16 数据元素）的指针。
  * @param  Size  要接收的数据元素数量（u8 或 u16）。
  * @note   当 UART 校验启用（PCE = 1）时，接收的数据包含校验位。
  * @retval HAL 状态
  */
HAL_StatusTypeDef HAL_UART_Receive_DMA(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
  /* 检查 Rx 过程是否尚未进行 */
  if (huart->RxState == HAL_UART_STATE_READY)
  {
    if ((pData == NULL) || (Size == 0U))
    {
      return HAL_ERROR;
    }

    /* 将接收类型设置为标准接收 */
    huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;

    return (UART_Start_Receive_DMA(huart, pData, Size));
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief 暂停 DMA 传输。
  * @param  huart  指向 UART_HandleTypeDef 结构体的指针，其中包含
  *                指定 UART 模块的配置信息。
  * @retval HAL 状态
  */
HAL_StatusTypeDef HAL_UART_DMAPause(UART_HandleTypeDef *huart)
{
  uint32_t dmarequest = 0x00U;

  dmarequest = HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAT);
  if ((huart->gState == HAL_UART_STATE_BUSY_TX) && dmarequest)
  {
    /* 禁用 UART DMA Tx 请求 */
    ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAT);
  }

  dmarequest = HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAR);
  if ((huart->RxState == HAL_UART_STATE_BUSY_RX) && dmarequest)
  {
    /* 禁用 RXNE、PE 和 ERR（帧错误、噪声错误、溢出错误）中断 */
    ATOMIC_CLEAR_BIT(huart->Instance->CR1, USART_CR1_PEIE);
    ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE);

    /* 禁用 UART DMA Rx 请求 */
    ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAR);
  }

  return HAL_OK;
}

/**
  * @brief 恢复 DMA 传输。
  * @param  huart  指向 UART_HandleTypeDef 结构体的指针，其中包含
  *                指定 UART 模块的配置信息。
  * @retval HAL 状态
  */
HAL_StatusTypeDef HAL_UART_DMAResume(UART_HandleTypeDef *huart)
{

  if (huart->gState == HAL_UART_STATE_BUSY_TX)
  {
    /* 使能 UART DMA Tx 请求 */
    ATOMIC_SET_BIT(huart->Instance->CR3, USART_CR3_DMAT);
  }

  if (huart->RxState == HAL_UART_STATE_BUSY_RX)
  {
    /* 在恢复 Rx 传输之前清除溢出标志 */
    __HAL_UART_CLEAR_OREFLAG(huart);

    /* 重新使能 PE 和 ERR（帧错误、噪声错误、溢出错误）中断 */
    if (huart->Init.Parity != UART_PARITY_NONE)
    {
      ATOMIC_SET_BIT(huart->Instance->CR1, USART_CR1_PEIE);
    }
    ATOMIC_SET_BIT(huart->Instance->CR3, USART_CR3_EIE);

    /* 使能 UART DMA Rx 请求 */
    ATOMIC_SET_BIT(huart->Instance->CR3, USART_CR3_DMAR);
  }

  return HAL_OK;
}

/**
  * @brief 停止 DMA 传输。
  * @param  huart  指向 UART_HandleTypeDef 结构体的指针，其中包含
  *                指定 UART 模块的配置信息。
  * @retval HAL 状态
  */
HAL_StatusTypeDef HAL_UART_DMAStop(UART_HandleTypeDef *huart)
{
  uint32_t dmarequest = 0x00U;
  /* 此 API 未实现锁，以允许用户应用程序
     在回调 HAL_UART_TxCpltCallback() / HAL_UART_RxCpltCallback() 下调用 HAL UART API：
     调用 HAL_DMA_Abort() API 时会生成 DMA TX/RX 传输完成中断
     并执行相应的回调 HAL_UART_TxCpltCallback() / HAL_UART_RxCpltCallback()
     */

  /* 如果 UART DMA Tx 请求正在进行，则停止 */
  dmarequest = HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAT);
  if ((huart->gState == HAL_UART_STATE_BUSY_TX) && dmarequest)
  {
    ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAT);

    /* 中止 UART DMA Tx 通道 */
    if (huart->hdmatx != NULL)
    {
      HAL_DMA_Abort(huart->hdmatx);
    }
    UART_EndTxTransfer(huart);
  }

  /* 如果 UART DMA Rx 请求正在进行，则停止 */
  dmarequest = HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAR);
  if ((huart->RxState == HAL_UART_STATE_BUSY_RX) && dmarequest)
  {
    ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAR);

    /* 中止 UART DMA Rx 通道 */
    if (huart->hdmarx != NULL)
    {
      HAL_DMA_Abort(huart->hdmarx);
    }
    UART_EndRxTransfer(huart);
  }

  return HAL_OK;
}

/**
  * @brief 在阻塞模式下接收一定数量的数据，直到接收到预期数量的数据或发生 IDLE 事件。
  * @note   如果接收完成（已接收到预期数量的数据），
  *         或者在 IDLE 事件后停止接收（接收到的数据少于预期数量），
  *         则返回 HAL_OK。在这种情况下，RxLen 输出参数指示接收缓冲区中可用的数据数量。
  * @note   当 UART 校验未启用（PCE = 0），且字长配置为 9 位（M = 01）时，
  *         接收的数据作为一组 uint16_t 处理。在这种情况下，Size 必须指示
  *         通过 pData 可用的 uint16_t 数量。
  * @param huart   UART 句柄。
  * @param pData   指向数据缓冲区（uint8_t 或 uint16_t 数据元素）的指针。
  * @param Size    要接收的数据元素数量（uint8_t 或 uint16_t）。
  * @param RxLen   最终接收到的数据元素数量（在 IDLE 事件结束接收的情况下可能小于 Size）
  * @param Timeout 以 ms 表示的超时持续时间（涵盖整个接收序列）。
  * @retval HAL 状态
  */
HAL_StatusTypeDef HAL_UARTEx_ReceiveToIdle(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint16_t *RxLen,
                                           uint32_t Timeout)
{
  uint8_t  *pdata8bits;
  uint16_t *pdata16bits;
  uint32_t tickstart;

  /* 检查 Rx 过程是否尚未进行 */
  if (huart->RxState == HAL_UART_STATE_READY)
  {
    if ((pData == NULL) || (Size == 0U))
    {
      return  HAL_ERROR;
    }

    huart->ErrorCode = HAL_UART_ERROR_NONE;
    huart->RxState = HAL_UART_STATE_BUSY_RX;
    huart->ReceptionType = HAL_UART_RECEPTION_TOIDLE;
    huart->RxEventType = HAL_UART_RXEVENT_TC;

    /* 初始化 tickstart 用于超时管理 */
    tickstart = HAL_GetTick();

    huart->RxXferSize  = Size;
    huart->RxXferCount = Size;

    /* 对于 9 位/无校验传输，pRxData 需要作为 uint16_t 指针处理 */
    if ((huart->Init.WordLength == UART_WORDLENGTH_9B) && (huart->Init.Parity == UART_PARITY_NONE))
    {
      pdata8bits  = NULL;
      pdata16bits = (uint16_t *) pData;
    }
    else
    {
      pdata8bits  = pData;
      pdata16bits = NULL;
    }

    /* 初始化已接收元素的输出数量 */
    *RxLen = 0U;

    /* 当还有数据要接收时 */
    while (huart->RxXferCount > 0U)
    {
      /* 检查 IDLE 标志是否置位 */
      if (__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE))
      {
        /* 在 ISR 中清除 IDLE 标志 */
        __HAL_UART_CLEAR_IDLEFLAG(huart);

        /* 如果置位，但从未接收到数据，则清除标志而不退出循环 */
        /* 如果置位，且已经接收到数据，这意味着空闲事件有效：结束接收 */
        if (*RxLen > 0U)
        {
          huart->RxEventType = HAL_UART_RXEVENT_IDLE;
          huart->RxState = HAL_UART_STATE_READY;

          return HAL_OK;
        }
      }

      /* 检查 RXNE 标志是否置位 */
      if (__HAL_UART_GET_FLAG(huart, UART_FLAG_RXNE))
      {
        if (pdata8bits == NULL)
        {
          *pdata16bits = (uint16_t)(huart->Instance->DR & (uint16_t)0x01FF);
          pdata16bits++;
        }
        else
        {
          if ((huart->Init.WordLength == UART_WORDLENGTH_9B) || ((huart->Init.WordLength == UART_WORDLENGTH_8B) && (huart->Init.Parity == UART_PARITY_NONE)))
          {
            *pdata8bits = (uint8_t)(huart->Instance->DR & (uint8_t)0x00FF);
          }
          else
          {
            *pdata8bits = (uint8_t)(huart->Instance->DR & (uint8_t)0x007F);
          }

          pdata8bits++;
        }
        /* 增加已接收元素的数量 */
        *RxLen += 1U;
        huart->RxXferCount--;
      }

      /* 检查超时 */
      if (Timeout != HAL_MAX_DELAY)
      {
        if (((HAL_GetTick() - tickstart) > Timeout) || (Timeout == 0U))
        {
          huart->RxState = HAL_UART_STATE_READY;

          return HAL_TIMEOUT;
        }
      }
    }

    /* 在输出参数中设置已接收元素的数量：RxLen */
    *RxLen = huart->RxXferSize - huart->RxXferCount;
    /* 在 Rx 过程结束时，将 huart->RxState 恢复为 Ready */
    huart->RxState = HAL_UART_STATE_READY;

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief 在中断模式下接收一定数量的数据，直到接收到预期数量的数据或发生 IDLE 事件。
  * @note   接收由此次函数调用启动。接收的进一步进展通过
  *         RXNE 和 IDLE 事件引发的 UART 中断实现。回调在接收结束时被调用，指示
  *         接收到的数据元素数量。
  * @note   当 UART 校验未启用（PCE = 0），且字长配置为 9 位（M = 01）时，
  *         接收的数据作为一组 uint16_t 处理。在这种情况下，Size 必须指示
  *         通过 pData 可用的 uint16_t 数量。
  * @param huart UART 句柄。
  * @param pData 指向数据缓冲区（uint8_t 或 uint16_t 数据元素）的指针。
  * @param Size  要接收的数据元素数量（uint8_t 或 uint16_t）。
  * @retval HAL 状态
  */
HAL_StatusTypeDef HAL_UARTEx_ReceiveToIdle_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
  HAL_StatusTypeDef status;

  /* 检查 Rx 过程是否尚未进行 */
  if (huart->RxState == HAL_UART_STATE_READY)
  {
    if ((pData == NULL) || (Size == 0U))
    {
      return HAL_ERROR;
    }

    /* 将接收类型设置为直到 IDLE 事件的接收 */
    huart->ReceptionType = HAL_UART_RECEPTION_TOIDLE;
    huart->RxEventType = HAL_UART_RXEVENT_TC;

    status =  UART_Start_Receive_IT(huart, pData, Size);

    /* 检查 Rx 进程是否已成功启动 */
    if (status == HAL_OK)
    {
      if (huart->ReceptionType == HAL_UART_RECEPTION_TOIDLE)
      {
        __HAL_UART_CLEAR_IDLEFLAG(huart);
        ATOMIC_SET_BIT(huart->Instance->CR1, USART_CR1_IDLEIE);
      }
      else
      {
        /* 如果在启动接收时已有错误挂起，
           中断可能已经触发并导致接收中止。
           （例如溢出错误）。
           在这种情况下，接收类型已重置为 HAL_UART_RECEPTION_STANDARD。 */
        status = HAL_ERROR;
      }
    }

    return status;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief 在 DMA 模式下接收一定数量的数据，直到接收到预期数量的数据或发生 IDLE 事件。
  * @note   接收由此次函数调用启动。接收的进一步进展通过
  *         DMA 服务实现，自动将接收到的数据元素传输到用户接收缓冲区，
  *         并在接收半/结束时调用已注册的回调。UART IDLE 事件也用于将
  *         接收阶段视为结束。在所有情况下，回调执行将指示接收到的数据元素数量。
  * @note   当 UART 校验启用（PCE = 1）时，接收的数据包含
  *         校验位（MSB 位置）。
  * @note   当 UART 校验未启用（PCE = 0），且字长配置为 9 位（M = 01）时，
  *         接收的数据作为一组 uint16_t 处理。在这种情况下，Size 必须指示
  *         通过 pData 可用的 uint16_t 数量。
  * @param huart UART 句柄。
  * @param pData 指向数据缓冲区（uint8_t 或 uint16_t 数据元素）的指针。
  * @param Size  要接收的数据元素数量（uint8_t 或 uint16_t）。
  * @retval HAL 状态
  */
HAL_StatusTypeDef HAL_UARTEx_ReceiveToIdle_DMA(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
  HAL_StatusTypeDef status;

  /* 检查 Rx 过程是否尚未进行 */
  if (huart->RxState == HAL_UART_STATE_READY)
  {
    if ((pData == NULL) || (Size == 0U))
    {
      return HAL_ERROR;
    }

    /* 将接收类型设置为直到 IDLE 事件的接收 */
    huart->ReceptionType = HAL_UART_RECEPTION_TOIDLE;
    huart->RxEventType = HAL_UART_RXEVENT_TC;

    status =  UART_Start_Receive_DMA(huart, pData, Size);

    /* 检查 Rx 进程是否已成功启动 */
    if (huart->ReceptionType == HAL_UART_RECEPTION_TOIDLE)
    {
      __HAL_UART_CLEAR_IDLEFLAG(huart);
      ATOMIC_SET_BIT(huart->Instance->CR1, USART_CR1_IDLEIE);
    }
    else
    {
      /* 如果在启动接收时已有错误挂起，
         中断可能已经触发并导致接收中止。
         （例如溢出错误）。
         在这种情况下，接收类型已重置为 HAL_UART_RECEPTION_STANDARD。 */
      status = HAL_ERROR;
    }

    return status;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief 提供导致 RxEvent 回调执行的 Rx 事件类型。
  * @note  当调用 HAL_UARTEx_ReceiveToIdle_IT() 或 HAL_UARTEx_ReceiveToIdle_DMA() API 时，
  *        接收过程的进度通过 Rx 事件回调的调用提供给应用程序（默认的
  *        HAL_UARTEx_RxEventCallback() 或用户注册的）。由于可能发生多种类型的事件（IDLE 事件、
  *        半传输或传输完成），此函数允许检索导致 Rx 事件回调执行的 Rx 事件类型。
  * @note  此函数应在用户的 Rx 事件回调实现中调用，
  *        以提供准确的值：
  *        在中断模式下：
  *           - HAL_UART_RXEVENT_TC : 接收已完成（已接收到预期数量的数据）
  *           - HAL_UART_RXEVENT_IDLE : 在接收完成前发生空闲事件（接收到的
  *             数据数量少于预期）
  *        在 DMA 模式下：
  *           - HAL_UART_RXEVENT_TC : 接收已完成（已接收到预期数量的数据）
  *           - HAL_UART_RXEVENT_HT : 已接收到预期数据数量的一半
  *           - HAL_UART_RXEVENT_IDLE : 在接收完成前发生空闲事件（接收到的
  *             数据数量少于预期）。
  *        在 DMA 模式下，RxEvent 回调可能被调用多次；
  *        当 DMA 配置为正常模式时，HT 事件不会停止接收过程；
  *        当 DMA 配置为循环模式时，HT、TC 或 IDLE 事件不会停止接收过程；
  * @param  huart UART 句柄。
  * @retval Rx 事件类型（返回值将是 @ref UART_RxEvent_Type_Values 的值）
  */
HAL_UART_RxEventTypeTypeDef HAL_UARTEx_GetRxEventType(UART_HandleTypeDef *huart)
{
  /* 返回 Rx 事件类型值，如 UART 句柄中所存储 */
  return(huart->RxEventType);
}

/**
  * @brief  中止正在进行的传输（阻塞模式）。
  * @param  huart UART 句柄。
  * @note   此过程可用于中止以中断或 DMA 模式启动的任何正在进行的传输。
  *         此过程执行以下操作：
  *           - 禁用 UART 中断（Tx 和 Rx）
  *           - 禁用外设寄存器中的 DMA 传输（如果启用）
  *           - 通过调用 HAL_DMA_Abort 中止 DMA 传输（DMA 模式下的传输）
  *           - 将句柄状态设置为 READY
  * @note   此过程在阻塞模式下执行：退出函数时，中止被视为已完成。
  * @retval HAL 状态
  */
HAL_StatusTypeDef HAL_UART_Abort(UART_HandleTypeDef *huart)
{
  /* 禁用 TXEIE、TCIE、RXNE、PE 和 ERR（帧错误、噪声错误、溢出错误）中断 */
  ATOMIC_CLEAR_BIT(huart->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE | USART_CR1_TXEIE | USART_CR1_TCIE));
  ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE);

  /* 如果正在进行直到 IDLE 事件的接收，则禁用 IDLEIE 中断 */
  if (huart->ReceptionType == HAL_UART_RECEPTION_TOIDLE)
  {
    ATOMIC_CLEAR_BIT(huart->Instance->CR1, (USART_CR1_IDLEIE));
  }

  /* 如果启用，则禁用 UART DMA Tx 请求 */
  if (HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAT))
  {
    ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAT);

    /* 中止 UART DMA Tx 通道：使用阻塞 DMA Abort API（无回调） */
    if (huart->hdmatx != NULL)
    {
      /* 将 UART DMA Abort 回调设置为 Null。
         DMA 中止过程结束时无回调执行 */
      huart->hdmatx->XferAbortCallback = NULL;

      if (HAL_DMA_Abort(huart->hdmatx) != HAL_OK)
      {
        if (HAL_DMA_GetError(huart->hdmatx) == HAL_DMA_ERROR_TIMEOUT)
        {
          /* 将错误代码设置为 DMA */
          huart->ErrorCode = HAL_UART_ERROR_DMA;

          return HAL_TIMEOUT;
        }
      }
    }
  }

  /* 如果启用，则禁用 UART DMA Rx 请求 */
  if (HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAR))
  {
    ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAR);

    /* 中止 UART DMA Rx 通道：使用阻塞 DMA Abort API（无回调） */
    if (huart->hdmarx != NULL)
    {
      /* 将 UART DMA Abort 回调设置为 Null。
         DMA 中止过程结束时无回调执行 */
      huart->hdmarx->XferAbortCallback = NULL;

      if (HAL_DMA_Abort(huart->hdmarx) != HAL_OK)
      {
        if (HAL_DMA_GetError(huart->hdmarx) == HAL_DMA_ERROR_TIMEOUT)
        {
          /* 将错误代码设置为 DMA */
          huart->ErrorCode = HAL_UART_ERROR_DMA;

          return HAL_TIMEOUT;
        }
      }
    }
  }

  /* 重置 Tx 和 Rx 传输计数器 */
  huart->TxXferCount = 0x00U;
  huart->RxXferCount = 0x00U;

  /* 重置 ErrorCode */
  huart->ErrorCode = HAL_UART_ERROR_NONE;

  /* 将 huart->RxState 和 huart->gState 恢复为 Ready */
  huart->RxState = HAL_UART_STATE_READY;
  huart->gState = HAL_UART_STATE_READY;
  huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;

  return HAL_OK;
}

/**
  * @brief  中止正在进行的发送传输（阻塞模式）。
  * @param  huart UART 句柄。
  * @note   此过程可用于中止以中断或 DMA 模式启动的任何正在进行的 Tx 传输。
  *         此过程执行以下操作：
  *           - 禁用 UART 中断（Tx）
  *           - 禁用外设寄存器中的 DMA 传输（如果启用）
  *           - 通过调用 HAL_DMA_Abort 中止 DMA 传输（DMA 模式下的传输）
  *           - 将句柄状态设置为 READY
  * @note   此过程在阻塞模式下执行：退出函数时，中止被视为已完成。
  * @retval HAL 状态
  */
HAL_StatusTypeDef HAL_UART_AbortTransmit(UART_HandleTypeDef *huart)
{
  /* 禁用 TXEIE 和 TCIE 中断 */
  ATOMIC_CLEAR_BIT(huart->Instance->CR1, (USART_CR1_TXEIE | USART_CR1_TCIE));

  /* 如果启用，则禁用 UART DMA Tx 请求 */
  if (HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAT))
  {
    ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAT);

    /* 中止 UART DMA Tx 通道：使用阻塞 DMA Abort API（无回调） */
    if (huart->hdmatx != NULL)
    {
      /* 将 UART DMA Abort 回调设置为 Null。
         DMA 中止过程结束时无回调执行 */
      huart->hdmatx->XferAbortCallback = NULL;

      if (HAL_DMA_Abort(huart->hdmatx) != HAL_OK)
      {
        if (HAL_DMA_GetError(huart->hdmatx) == HAL_DMA_ERROR_TIMEOUT)
        {
          /* 将错误代码设置为 DMA */
          huart->ErrorCode = HAL_UART_ERROR_DMA;

          return HAL_TIMEOUT;
        }
      }
    }
  }

  /* 重置 Tx 传输计数器 */
  huart->TxXferCount = 0x00U;

  /* 将 huart->gState 恢复为 Ready */
  huart->gState = HAL_UART_STATE_READY;

  return HAL_OK;
}

/**
  * @brief  中止正在进行的接收传输（阻塞模式）。
  * @param  huart UART 句柄。
  * @note   此过程可用于中止以中断或 DMA 模式启动的任何正在进行的 Rx 传输。
  *         此过程执行以下操作：
  *           - 禁用 UART 中断（Rx）
  *           - 禁用外设寄存器中的 DMA 传输（如果启用）
  *           - 通过调用 HAL_DMA_Abort 中止 DMA 传输（DMA 模式下的传输）
  *           - 将句柄状态设置为 READY
  * @note   此过程在阻塞模式下执行：退出函数时，中止被视为已完成。
  * @retval HAL 状态
  */
HAL_StatusTypeDef HAL_UART_AbortReceive(UART_HandleTypeDef *huart)
{
  /* 禁用 RXNE、PE 和 ERR（帧错误、噪声错误、溢出错误）中断 */
  ATOMIC_CLEAR_BIT(huart->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE));
  ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE);

  /* 如果正在进行直到 IDLE 事件的接收，则禁用 IDLEIE 中断 */
  if (huart->ReceptionType == HAL_UART_RECEPTION_TOIDLE)
  {
    ATOMIC_CLEAR_BIT(huart->Instance->CR1, (USART_CR1_IDLEIE));
  }

  /* 如果启用，则禁用 UART DMA Rx 请求 */
  if (HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAR))
  {
    ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAR);

    /* 中止 UART DMA Rx 通道：使用阻塞 DMA Abort API（无回调） */
    if (huart->hdmarx != NULL)
    {
      /* 将 UART DMA Abort 回调设置为 Null。
         DMA 中止过程结束时无回调执行 */
      huart->hdmarx->XferAbortCallback = NULL;

      if (HAL_DMA_Abort(huart->hdmarx) != HAL_OK)
      {
        if (HAL_DMA_GetError(huart->hdmarx) == HAL_DMA_ERROR_TIMEOUT)
        {
          /* 将错误代码设置为 DMA */
          huart->ErrorCode = HAL_UART_ERROR_DMA;

          return HAL_TIMEOUT;
        }
      }
    }
  }

  /* 重置 Rx 传输计数器 */
  huart->RxXferCount = 0x00U;

  /* 将 huart->RxState 恢复为 Ready */
  huart->RxState = HAL_UART_STATE_READY;
  huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;

  return HAL_OK;
}

/**
  * @brief  中止正在进行的传输（中断模式）。
  * @param  huart UART 句柄。
  * @note   此过程可用于中止以中断或 DMA 模式启动的任何正在进行的传输。
  *         此过程执行以下操作：
  *           - 禁用 UART 中断（Tx 和 Rx）
  *           - 禁用外设寄存器中的 DMA 传输（如果启用）
  *           - 通过调用 HAL_DMA_Abort_IT 中止 DMA 传输（DMA 模式下的传输）
  *           - 将句柄状态设置为 READY
  *           - 在中止完成时，调用用户中止完成回调
  * @note   此过程在中断模式下执行，意味着中止过程只能
  *         在执行用户中止完成回调时被视为完成（而不是在退出函数时）。
  * @retval HAL 状态
  */
HAL_StatusTypeDef HAL_UART_Abort_IT(UART_HandleTypeDef *huart)
{
  uint32_t AbortCplt = 0x01U;

  /* 禁用 TXEIE、TCIE、RXNE、PE 和 ERR（帧错误、噪声错误、溢出错误）中断 */
  ATOMIC_CLEAR_BIT(huart->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE | USART_CR1_TXEIE | USART_CR1_TCIE));
  ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE);

  /* 如果正在进行直到 IDLE 事件的接收，则禁用 IDLEIE 中断 */
  if (huart->ReceptionType == HAL_UART_RECEPTION_TOIDLE)
  {
    ATOMIC_CLEAR_BIT(huart->Instance->CR1, (USART_CR1_IDLEIE));
  }

  /* 如果 DMA Tx 和/或 DMA Rx 句柄与 UART 句柄关联，则在任何调用 DMA Abort 函数之前
     应初始化 DMA Abort 完成回调 */
  /* DMA Tx 句柄有效 */
  if (huart->hdmatx != NULL)
  {
    /* 如果启用了 UART DMA Tx 请求，则设置 DMA Abort 完成回调。
       否则，将其设置为 NULL */
    if (HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAT))
    {
      huart->hdmatx->XferAbortCallback = UART_DMATxAbortCallback;
    }
    else
    {
      huart->hdmatx->XferAbortCallback = NULL;
    }
  }
  /* DMA Rx 句柄有效 */
  if (huart->hdmarx != NULL)
  {
    /* 如果启用了 UART DMA Rx 请求，则设置 DMA Abort 完成回调。
       否则，将其设置为 NULL */
    if (HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAR))
    {
      huart->hdmarx->XferAbortCallback = UART_DMARxAbortCallback;
    }
    else
    {
      huart->hdmarx->XferAbortCallback = NULL;
    }
  }

  /* 如果启用，则禁用 UART DMA Tx 请求 */
  if (HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAT))
  {
    /* 在 UART 级别禁用 DMA Tx */
    ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAT);

    /* 中止 UART DMA Tx 通道：使用非阻塞 DMA Abort API（回调） */
    if (huart->hdmatx != NULL)
    {
      /* UART Tx DMA Abort 回调已被初始化：
         将导致在 DMA 中止过程结束时调用 HAL_UART_AbortCpltCallback() */

      /* 中止 DMA TX */
      if (HAL_DMA_Abort_IT(huart->hdmatx) != HAL_OK)
      {
        huart->hdmatx->XferAbortCallback = NULL;
      }
      else
      {
        AbortCplt = 0x00U;
      }
    }
  }

  /* 如果启用，则禁用 UART DMA Rx 请求 */
  if (HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAR))
  {
    ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAR);

    /* 中止 UART DMA Rx 通道：使用非阻塞 DMA Abort API（回调） */
    if (huart->hdmarx != NULL)
    {
      /* UART Rx DMA Abort 回调已被初始化：
         将导致在 DMA 中止过程结束时调用 HAL_UART_AbortCpltCallback() */

      /* 中止 DMA RX */
      if (HAL_DMA_Abort_IT(huart->hdmarx) != HAL_OK)
      {
        huart->hdmarx->XferAbortCallback = NULL;
        AbortCplt = 0x01U;
      }
      else
      {
        AbortCplt = 0x00U;
      }
    }
  }

  /* 如果不需要执行 DMA 中止完成回调 => 调用用户 Abort Complete 回调 */
  if (AbortCplt == 0x01U)
  {
    /* 重置 Tx 和 Rx 传输计数器 */
    huart->TxXferCount = 0x00U;
    huart->RxXferCount = 0x00U;

    /* 重置 ErrorCode */
    huart->ErrorCode = HAL_UART_ERROR_NONE;

    /* 将 huart->gState 和 huart->RxState 恢复为 Ready */
    huart->gState  = HAL_UART_STATE_READY;
    huart->RxState = HAL_UART_STATE_READY;
    huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;

    /* 由于没有 DMA 被中止，直接调用用户 Abort complete 回调 */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
    /* 调用已注册的 Abort complete 回调 */
    huart->AbortCpltCallback(huart);
#else
    /* 调用旧版 weak Abort complete 回调 */
    HAL_UART_AbortCpltCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
  }

  return HAL_OK;
}

/**
  * @brief  中止正在进行的发送传输（中断模式）。
  * @param  huart UART 句柄。
  * @note   此过程可用于中止以中断或 DMA 模式启动的任何正在进行的 Tx 传输。
  *         此过程执行以下操作：
  *           - 禁用 UART 中断（Tx）
  *           - 禁用外设寄存器中的 DMA 传输（如果启用）
  *           - 通过调用 HAL_DMA_Abort_IT 中止 DMA 传输（DMA 模式下的传输）
  *           - 将句柄状态设置为 READY
  *           - 在中止完成时，调用用户中止完成回调
  * @note   此过程在中断模式下执行，意味着中止过程只能
  *         在执行用户中止完成回调时被视为完成（而不是在退出函数时）。
  * @retval HAL 状态
  */
HAL_StatusTypeDef HAL_UART_AbortTransmit_IT(UART_HandleTypeDef *huart)
{
  /* 禁用 TXEIE 和 TCIE 中断 */
  ATOMIC_CLEAR_BIT(huart->Instance->CR1, (USART_CR1_TXEIE | USART_CR1_TCIE));

  /* 如果启用，则禁用 UART DMA Tx 请求 */
  if (HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAT))
  {
    ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAT);

    /* 中止 UART DMA Tx 通道：使用阻塞 DMA Abort API（无回调） */
    if (huart->hdmatx != NULL)
    {
      /* 设置 UART DMA Abort 回调：
         将导致在 DMA 中止过程结束时调用 HAL_UART_AbortCpltCallback() */
      huart->hdmatx->XferAbortCallback = UART_DMATxOnlyAbortCallback;

      /* 中止 DMA TX */
      if (HAL_DMA_Abort_IT(huart->hdmatx) != HAL_OK)
      {
        /* 在错误情况下直接调用 huart->hdmatx->XferAbortCallback 函数 */
        huart->hdmatx->XferAbortCallback(huart->hdmatx);
      }
    }
    else
    {
      /* 重置 Tx 传输计数器 */
      huart->TxXferCount = 0x00U;

      /* 将 huart->gState 恢复为 Ready */
      huart->gState = HAL_UART_STATE_READY;

      /* 由于没有 DMA 被中止，直接调用用户 Abort complete 回调 */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
      /* 调用已注册的 Abort Transmit Complete 回调 */
      huart->AbortTransmitCpltCallback(huart);
#else
      /* 调用旧版 weak Abort Transmit Complete 回调 */
      HAL_UART_AbortTransmitCpltCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
    }
  }
  else
  {
    /* 重置 Tx 传输计数器 */
    huart->TxXferCount = 0x00U;

    /* 将 huart->gState 恢复为 Ready */
    huart->gState = HAL_UART_STATE_READY;

    /* 由于没有 DMA 被中止，直接调用用户 Abort complete 回调 */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
    /* 调用已注册的 Abort Transmit Complete 回调 */
    huart->AbortTransmitCpltCallback(huart);
#else
    /* 调用旧版 weak Abort Transmit Complete 回调 */
    HAL_UART_AbortTransmitCpltCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
  }

  return HAL_OK;
}

/**
  * @brief  中止正在进行的接收传输（中断模式）。
  * @param  huart UART 句柄。
  * @note   此过程可用于中止以中断或 DMA 模式启动的任何正在进行的 Rx 传输。
  *         此过程执行以下操作：
  *           - 禁用 UART 中断（Rx）
  *           - 禁用外设寄存器中的 DMA 传输（如果启用）
  *           - 通过调用 HAL_DMA_Abort_IT 中止 DMA 传输（DMA 模式下的传输）
  *           - 将句柄状态设置为 READY
  *           - 在中止完成时，调用用户中止完成回调
  * @note   此过程在中断模式下执行，意味着中止过程只能
  *         在执行用户中止完成回调时被视为完成（而不是在退出函数时）。
  * @retval HAL 状态
  */
HAL_StatusTypeDef HAL_UART_AbortReceive_IT(UART_HandleTypeDef *huart)
{
  /* 禁用 RXNE、PE 和 ERR（帧错误、噪声错误、溢出错误）中断 */
  ATOMIC_CLEAR_BIT(huart->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE));
  ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE);

  /* 如果正在进行直到 IDLE 事件的接收，则禁用 IDLEIE 中断 */
  if (huart->ReceptionType == HAL_UART_RECEPTION_TOIDLE)
  {
    ATOMIC_CLEAR_BIT(huart->Instance->CR1, (USART_CR1_IDLEIE));
  }

  /* 如果启用，则禁用 UART DMA Rx 请求 */
  if (HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAR))
  {
    ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAR);

    /* 中止 UART DMA Rx 通道：使用阻塞 DMA Abort API（无回调） */
    if (huart->hdmarx != NULL)
    {
      /* 设置 UART DMA Abort 回调：
         将导致在 DMA 中止过程结束时调用 HAL_UART_AbortCpltCallback() */
      huart->hdmarx->XferAbortCallback = UART_DMARxOnlyAbortCallback;

      /* 中止 DMA RX */
      if (HAL_DMA_Abort_IT(huart->hdmarx) != HAL_OK)
      {
        /* 在错误情况下直接调用 huart->hdmarx->XferAbortCallback 函数 */
        huart->hdmarx->XferAbortCallback(huart->hdmarx);
      }
    }
    else
    {
      /* 重置 Rx 传输计数器 */
      huart->RxXferCount = 0x00U;

      /* 将 huart->RxState 恢复为 Ready */
      huart->RxState = HAL_UART_STATE_READY;
      huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;

      /* 由于没有 DMA 被中止，直接调用用户 Abort complete 回调 */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
      /* 调用已注册的 Abort Receive Complete 回调 */
      huart->AbortReceiveCpltCallback(huart);
#else
      /* 调用旧版 weak Abort Receive Complete 回调 */
      HAL_UART_AbortReceiveCpltCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
    }
  }
  else
  {
    /* 重置 Rx 传输计数器 */
    huart->RxXferCount = 0x00U;

    /* 将 huart->RxState 恢复为 Ready */
    huart->RxState = HAL_UART_STATE_READY;
    huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;

    /* 由于没有 DMA 被中止，直接调用用户 Abort complete 回调 */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
    /* 调用已注册的 Abort Receive Complete 回调 */
    huart->AbortReceiveCpltCallback(huart);
#else
    /* 调用旧版 weak Abort Receive Complete 回调 */
    HAL_UART_AbortReceiveCpltCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
  }

  return HAL_OK;
}

/**
  * @brief  此函数处理 UART 中断请求。
  * @param  huart  指向 UART_HandleTypeDef 结构体的指针，其中包含
  *                指定 UART 模块的配置信息。
  * @retval None
  */
void HAL_UART_IRQHandler(UART_HandleTypeDef *huart)
{
  uint32_t isrflags   = READ_REG(huart->Instance->SR);
  uint32_t cr1its     = READ_REG(huart->Instance->CR1);
  uint32_t cr3its     = READ_REG(huart->Instance->CR3);
  uint32_t errorflags = 0x00U;
  uint32_t dmarequest = 0x00U;

  /* 如果没有错误发生 */
  errorflags = (isrflags & (uint32_t)(USART_SR_PE | USART_SR_FE | USART_SR_ORE | USART_SR_NE));
  if (errorflags == RESET)
  {
    /* UART 处于接收器模式 -------------------------------------------------*/
    if (((isrflags & USART_SR_RXNE) != RESET) && ((cr1its & USART_CR1_RXNEIE) != RESET))
    {
      UART_Receive_IT(huart);
      return;
    }
  }

  /* 如果发生某些错误 */
  if ((errorflags != RESET) && (((cr3its & USART_CR3_EIE) != RESET)
                                || ((cr1its & (USART_CR1_RXNEIE | USART_CR1_PEIE)) != RESET)))
  {
    /* UART 校验错误中断发生 ----------------------------------------*/
    if (((isrflags & USART_SR_PE) != RESET) && ((cr1its & USART_CR1_PEIE) != RESET))
    {
      huart->ErrorCode |= HAL_UART_ERROR_PE;
    }

    /* UART 噪声错误中断发生 -----------------------------------------*/
    if (((isrflags & USART_SR_NE) != RESET) && ((cr3its & USART_CR3_EIE) != RESET))
    {
      huart->ErrorCode |= HAL_UART_ERROR_NE;
    }

    /* UART 帧错误中断发生 -----------------------------------------*/
    if (((isrflags & USART_SR_FE) != RESET) && ((cr3its & USART_CR3_EIE) != RESET))
    {
      huart->ErrorCode |= HAL_UART_ERROR_FE;
    }

    /* UART 溢出中断发生 --------------------------------------------*/
    if (((isrflags & USART_SR_ORE) != RESET) && (((cr1its & USART_CR1_RXNEIE) != RESET)
                                                 || ((cr3its & USART_CR3_EIE) != RESET)))
    {
      huart->ErrorCode |= HAL_UART_ERROR_ORE;
    }

    /* 如有必要，调用 UART 错误回调函数 --------------------------*/
    if (huart->ErrorCode != HAL_UART_ERROR_NONE)
    {
      /* UART 处于接收器模式 -----------------------------------------------*/
      if (((isrflags & USART_SR_RXNE) != RESET) && ((cr1its & USART_CR1_RXNEIE) != RESET))
      {
        UART_Receive_IT(huart);
      }

      /* 如果发生溢出错误，或 DMA 模式接收中发生任何错误，
         则将错误视为阻塞 */
      dmarequest = HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAR);
      if (((huart->ErrorCode & HAL_UART_ERROR_ORE) != RESET) || dmarequest)
      {
        /* 阻塞错误：传输被中止
           将 UART 状态设置为就绪以能够重新启动过程，
           禁用 Rx 中断，并禁用 Rx DMA 请求（如果正在进行） */
        UART_EndRxTransfer(huart);

        /* 如果启用，则禁用 UART DMA Rx 请求 */
        if (HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAR))
        {
          ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAR);

          /* 中止 UART DMA Rx 通道 */
          if (huart->hdmarx != NULL)
          {
            /* 设置 UART DMA Abort 回调：
               将导致在 DMA 中止过程结束时调用 HAL_UART_ErrorCallback() */
            huart->hdmarx->XferAbortCallback = UART_DMAAbortOnError;
            if (HAL_DMA_Abort_IT(huart->hdmarx) != HAL_OK)
            {
              /* 在错误情况下直接调用 XferAbortCallback 函数 */
              huart->hdmarx->XferAbortCallback(huart->hdmarx);
            }
          }
          else
          {
            /* 调用用户错误回调 */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
            /* 调用已注册的错误回调 */
            huart->ErrorCallback(huart);
#else
            /* 调用旧版 weak 错误回调 */
            HAL_UART_ErrorCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
          }
        }
        else
        {
          /* 调用用户错误回调 */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
          /* 调用已注册的错误回调 */
          huart->ErrorCallback(huart);
#else
          /* 调用旧版 weak 错误回调 */
          HAL_UART_ErrorCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
        }
      }
      else
      {
        /* 非阻塞错误：传输可以继续。
           错误通过用户错误回调通知用户 */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
        /* 调用已注册的错误回调 */
        huart->ErrorCallback(huart);
#else
        /* 调用旧版 weak 错误回调 */
        HAL_UART_ErrorCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */

        huart->ErrorCode = HAL_UART_ERROR_NONE;
      }
    }
    return;
  } /* End if some error occurs */

  /* 检查当前接收模式：
     如果选择了直到 IDLE 事件的接收：*/
  if ((huart->ReceptionType == HAL_UART_RECEPTION_TOIDLE)
      && ((isrflags & USART_SR_IDLE) != 0U)
      && ((cr1its & USART_SR_IDLE) != 0U))
  {
    __HAL_UART_CLEAR_IDLEFLAG(huart);

    /* 检查 UART 中是否启用了 DMA 模式 */
    if (HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAR))
    {
      /* DMA 模式已启用 */
      /* 检查接收长度：如果已接收到所有预期数据，则不执行任何操作，
         （DMA cplt 回调将被调用）。
         否则，如果已接收到至少一个数据，则 IDLE 事件要通知用户 */
      uint16_t nb_remaining_rx_data = (uint16_t) __HAL_DMA_GET_COUNTER(huart->hdmarx);
      if ((nb_remaining_rx_data > 0U)
          && (nb_remaining_rx_data < huart->RxXferSize))
      {
        /* 接收未完成 */
        huart->RxXferCount = nb_remaining_rx_data;

        /* 在正常模式下，结束 DMA xfer 和 HAL UART Rx 过程 */
        if (huart->hdmarx->Init.Mode != DMA_CIRCULAR)
        {
          /* 禁用 PE 和 ERR（帧错误、噪声错误、溢出错误）中断 */
          ATOMIC_CLEAR_BIT(huart->Instance->CR1, USART_CR1_PEIE);
          ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE);

          /* 通过重置 UART CR3 寄存器中的 DMAR 位
             禁用接收器请求的 DMA 传输 */
          ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAR);

          /* 在 Rx 过程结束时，将 huart->RxState 恢复为 Ready */
          huart->RxState = HAL_UART_STATE_READY;
          huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;

          ATOMIC_CLEAR_BIT(huart->Instance->CR1, USART_CR1_IDLEIE);

          /* 接收到的最后几个字节，因此中止是即时的，无需等待 */
          (void)HAL_DMA_Abort(huart->hdmarx);
        }

        /* 初始化与 RxEvent 回调执行对应的 RxEvent 类型；
        在这种情况下，Rx 事件类型是空闲事件 */
        huart->RxEventType = HAL_UART_RXEVENT_IDLE;

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
        /* 调用已注册的 Rx 事件回调 */
        huart->RxEventCallback(huart, (huart->RxXferSize - huart->RxXferCount));
#else
        /* 调用旧版 weak Rx 事件回调 */
        HAL_UARTEx_RxEventCallback(huart, (huart->RxXferSize - huart->RxXferCount));
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
      }
      return;
    }
    else
    {
      /* DMA 模式未启用 */
      /* 检查接收长度：如果已接收到所有预期数据，则不执行任何操作。
         否则，如果已接收到至少一个数据，则 IDLE 事件要通知用户 */
      uint16_t nb_rx_data = huart->RxXferSize - huart->RxXferCount;
      if ((huart->RxXferCount > 0U)
          && (nb_rx_data > 0U))
      {
        /* 禁用 UART 校验错误中断和 RXNE 中断 */
        ATOMIC_CLEAR_BIT(huart->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE));

        /* 禁用 UART 错误中断：（帧错误、噪声错误、溢出错误） */
        ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE);

        /* Rx 过程已完成，将 huart->RxState 恢复为 Ready */
        huart->RxState = HAL_UART_STATE_READY;
        huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;

        ATOMIC_CLEAR_BIT(huart->Instance->CR1, USART_CR1_IDLEIE);

        /* 初始化与 RxEvent 回调执行对应的 RxEvent 类型；
           在这种情况下，Rx 事件类型是空闲事件 */
        huart->RxEventType = HAL_UART_RXEVENT_IDLE;

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
        /* 调用已注册的 Rx 完成回调 */
        huart->RxEventCallback(huart, nb_rx_data);
#else
        /* 调用旧版 weak Rx 事件回调 */
        HAL_UARTEx_RxEventCallback(huart, nb_rx_data);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
      }
      return;
    }
  }

  /* UART 处于发送器模式 ------------------------------------------------*/
  if (((isrflags & USART_SR_TXE) != RESET) && ((cr1its & USART_CR1_TXEIE) != RESET))
  {
    UART_Transmit_IT(huart);
    return;
  }

  /* UART 处于发送器结束模式 --------------------------------------------*/
  if (((isrflags & USART_SR_TC) != RESET) && ((cr1its & USART_CR1_TCIE) != RESET))
  {
    UART_EndTransmit_IT(huart);
    return;
  }
} weak Abort complete callback */
    HAL_UART_AbortCpltCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
  }

  return HAL_OK;
}

/**
  * @brief  Abort ongoing Transmit transfer (Interrupt mode).
  * @param  huart UART handle.
  * @note   This procedure could be used for aborting any ongoing Tx transfer started in Interrupt or DMA mode.
  *         This procedure performs following operations :
  *           - Disable UART Interrupts (Tx)
  *           - Disable the DMA transfer in the peripheral register (if enabled)
  *           - Abort DMA transfer by calling HAL_DMA_Abort_IT (in case of transfer in DMA mode)
  *           - Set handle State to READY
  *           - At abort completion, call user abort complete callback
  * @note   This procedure is executed in Interrupt mode, meaning that abort procedure could be
  *         considered as completed only when user abort complete callback is executed (not when exiting function).
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_UART_AbortTransmit_IT(UART_HandleTypeDef *huart)
{
  /* Disable TXEIE and TCIE interrupts */
  ATOMIC_CLEAR_BIT(huart->Instance->CR1, (USART_CR1_TXEIE | USART_CR1_TCIE));

  /* Disable the UART DMA Tx request if enabled */
  if (HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAT))
  {
    ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAT);

    /* Abort the UART DMA Tx channel : use blocking DMA Abort API (no callback) */
    if (huart->hdmatx != NULL)
    {
      /* Set the UART DMA Abort callback :
         will lead to call HAL_UART_AbortCpltCallback() at end of DMA abort procedure */
      huart->hdmatx->XferAbortCallback = UART_DMATxOnlyAbortCallback;

      /* Abort DMA TX */
      if (HAL_DMA_Abort_IT(huart->hdmatx) != HAL_OK)
      {
        /* Call Directly huart->hdmatx->XferAbortCallback function in case of error */
        huart->hdmatx->XferAbortCallback(huart->hdmatx);
      }
    }
    else
    {
      /* Reset Tx transfer counter */
      huart->TxXferCount = 0x00U;

      /* Restore huart->gState to Ready */
      huart->gState = HAL_UART_STATE_READY;

      /* As no DMA to be aborted, call directly user Abort complete callback */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
      /* Call registered Abort Transmit Complete Callback */
      huart->AbortTransmitCpltCallback(huart);
#else
      /* Call legacy weak Abort Transmit Complete Callback */
      HAL_UART_AbortTransmitCpltCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
    }
  }
  else
  {
    /* Reset Tx transfer counter */
    huart->TxXferCount = 0x00U;

    /* Restore huart->gState to Ready */
    huart->gState = HAL_UART_STATE_READY;

    /* As no DMA to be aborted, call directly user Abort complete callback */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
    /* Call registered Abort Transmit Complete Callback */
    huart->AbortTransmitCpltCallback(huart);
#else
    /* Call legacy weak Abort Transmit Complete Callback */
    HAL_UART_AbortTransmitCpltCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
  }

  return HAL_OK;
}

/**
  * @brief  Abort ongoing Receive transfer (Interrupt mode).
  * @param  huart UART handle.
  * @note   This procedure could be used for aborting any ongoing Rx transfer started in Interrupt or DMA mode.
  *         This procedure performs following operations :
  *           - Disable UART Interrupts (Rx)
  *           - Disable the DMA transfer in the peripheral register (if enabled)
  *           - Abort DMA transfer by calling HAL_DMA_Abort_IT (in case of transfer in DMA mode)
  *           - Set handle State to READY
  *           - At abort completion, call user abort complete callback
  * @note   This procedure is executed in Interrupt mode, meaning that abort procedure could be
  *         considered as completed only when user abort complete callback is executed (not when exiting function).
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_UART_AbortReceive_IT(UART_HandleTypeDef *huart)
{
  /* Disable RXNE, PE and ERR (Frame error, noise error, overrun error) interrupts */
  ATOMIC_CLEAR_BIT(huart->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE));
  ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE);

  /* If Reception till IDLE event was ongoing, disable IDLEIE interrupt */
  if (huart->ReceptionType == HAL_UART_RECEPTION_TOIDLE)
  {
    ATOMIC_CLEAR_BIT(huart->Instance->CR1, (USART_CR1_IDLEIE));
  }

  /* Disable the UART DMA Rx request if enabled */
  if (HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAR))
  {
    ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAR);

    /* Abort the UART DMA Rx channel : use blocking DMA Abort API (no callback) */
    if (huart->hdmarx != NULL)
    {
      /* Set the UART DMA Abort callback :
         will lead to call HAL_UART_AbortCpltCallback() at end of DMA abort procedure */
      huart->hdmarx->XferAbortCallback = UART_DMARxOnlyAbortCallback;

      /* Abort DMA RX */
      if (HAL_DMA_Abort_IT(huart->hdmarx) != HAL_OK)
      {
        /* Call Directly huart->hdmarx->XferAbortCallback function in case of error */
        huart->hdmarx->XferAbortCallback(huart->hdmarx);
      }
    }
    else
    {
      /* Reset Rx transfer counter */
      huart->RxXferCount = 0x00U;

      /* Restore huart->RxState to Ready */
      huart->RxState = HAL_UART_STATE_READY;
      huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;

      /* As no DMA to be aborted, call directly user Abort complete callback */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
      /* Call registered Abort Receive Complete Callback */
      huart->AbortReceiveCpltCallback(huart);
#else
      /* Call legacy weak Abort Receive Complete Callback */
      HAL_UART_AbortReceiveCpltCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
    }
  }
  else
  {
    /* Reset Rx transfer counter */
    huart->RxXferCount = 0x00U;

    /* Restore huart->RxState to Ready */
    huart->RxState = HAL_UART_STATE_READY;
    huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;

    /* As no DMA to be aborted, call directly user Abort complete callback */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
    /* Call registered Abort Receive Complete Callback */
    huart->AbortReceiveCpltCallback(huart);
#else
    /* Call legacy weak Abort Receive Complete Callback */
    HAL_UART_AbortReceiveCpltCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
  }

  return HAL_OK;
}

/**
  * @brief  This function handles UART interrupt request.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */
void HAL_UART_IRQHandler(UART_HandleTypeDef *huart)
{
  uint32_t isrflags   = READ_REG(huart->Instance->SR);
  uint32_t cr1its     = READ_REG(huart->Instance->CR1);
  uint32_t cr3its     = READ_REG(huart->Instance->CR3);
  uint32_t errorflags = 0x00U;
  uint32_t dmarequest = 0x00U;

  /* If no error occurs */
  errorflags = (isrflags & (uint32_t)(USART_SR_PE | USART_SR_FE | USART_SR_ORE | USART_SR_NE));
  if (errorflags == RESET)
  {
    /* UART in mode Receiver -------------------------------------------------*/
    if (((isrflags & USART_SR_RXNE) != RESET) && ((cr1its & USART_CR1_RXNEIE) != RESET))
    {
      UART_Receive_IT(huart);
      return;
    }
  }

  /* If some errors occur */
  if ((errorflags != RESET) && (((cr3its & USART_CR3_EIE) != RESET)
                                || ((cr1its & (USART_CR1_RXNEIE | USART_CR1_PEIE)) != RESET)))
  {
    /* UART parity error interrupt occurred ----------------------------------*/
    if (((isrflags & USART_SR_PE) != RESET) && ((cr1its & USART_CR1_PEIE) != RESET))
    {
      huart->ErrorCode |= HAL_UART_ERROR_PE;
    }

    /* UART noise error interrupt occurred -----------------------------------*/
    if (((isrflags & USART_SR_NE) != RESET) && ((cr3its & USART_CR3_EIE) != RESET))
    {
      huart->ErrorCode |= HAL_UART_ERROR_NE;
    }

    /* UART frame error interrupt occurred -----------------------------------*/
    if (((isrflags & USART_SR_FE) != RESET) && ((cr3its & USART_CR3_EIE) != RESET))
    {
      huart->ErrorCode |= HAL_UART_ERROR_FE;
    }

    /* UART Over-Run interrupt occurred --------------------------------------*/
    if (((isrflags & USART_SR_ORE) != RESET) && (((cr1its & USART_CR1_RXNEIE) != RESET)
                                                 || ((cr3its & USART_CR3_EIE) != RESET)))
    {
      huart->ErrorCode |= HAL_UART_ERROR_ORE;
    }

    /* Call UART Error Call back function if need be --------------------------*/
    if (huart->ErrorCode != HAL_UART_ERROR_NONE)
    {
      /* UART in mode Receiver -----------------------------------------------*/
      if (((isrflags & USART_SR_RXNE) != RESET) && ((cr1its & USART_CR1_RXNEIE) != RESET))
      {
        UART_Receive_IT(huart);
      }

      /* If Overrun error occurs, or if any error occurs in DMA mode reception,
         consider error as blocking */
      dmarequest = HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAR);
      if (((huart->ErrorCode & HAL_UART_ERROR_ORE) != RESET) || dmarequest)
      {
        /* Blocking error : transfer is aborted
           Set the UART state ready to be able to start again the process,
           Disable Rx Interrupts, and disable Rx DMA request, if ongoing */
        UART_EndRxTransfer(huart);

        /* Disable the UART DMA Rx request if enabled */
        if (HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAR))
        {
          ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAR);

          /* Abort the UART DMA Rx channel */
          if (huart->hdmarx != NULL)
          {
            /* Set the UART DMA Abort callback :
               will lead to call HAL_UART_ErrorCallback() at end of DMA abort procedure */
            huart->hdmarx->XferAbortCallback = UART_DMAAbortOnError;
            if (HAL_DMA_Abort_IT(huart->hdmarx) != HAL_OK)
            {
              /* Call Directly XferAbortCallback function in case of error */
              huart->hdmarx->XferAbortCallback(huart->hdmarx);
            }
          }
          else
          {
            /* Call user error callback */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
            /*Call registered error callback*/
            huart->ErrorCallback(huart);
#else
            /*Call legacy weak error callback*/
            HAL_UART_ErrorCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
          }
        }
        else
        {
          /* Call user error callback */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
          /*Call registered error callback*/
          huart->ErrorCallback(huart);
#else
          /*Call legacy weak error callback*/
          HAL_UART_ErrorCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
        }
      }
      else
      {
        /* Non Blocking error : transfer could go on.
           Error is notified to user through user error callback */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
        /*Call registered error callback*/
        huart->ErrorCallback(huart);
#else
        /*Call legacy weak error callback*/
        HAL_UART_ErrorCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */

        huart->ErrorCode = HAL_UART_ERROR_NONE;
      }
    }
    return;
  } /* End if some error occurs */

  /* Check current reception Mode :
     If Reception till IDLE event has been selected : */
  if ((huart->ReceptionType == HAL_UART_RECEPTION_TOIDLE)
      && ((isrflags & USART_SR_IDLE) != 0U)
      && ((cr1its & USART_SR_IDLE) != 0U))
  {
    __HAL_UART_CLEAR_IDLEFLAG(huart);

    /* Check if DMA mode is enabled in UART */
    if (HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAR))
    {
      /* DMA mode enabled */
      /* Check received length : If all expected data are received, do nothing,
         (DMA cplt callback will be called).
         Otherwise, if at least one data has already been received, IDLE event is to be notified to user */
      uint16_t nb_remaining_rx_data = (uint16_t) __HAL_DMA_GET_COUNTER(huart->hdmarx);
      if ((nb_remaining_rx_data > 0U)
          && (nb_remaining_rx_data < huart->RxXferSize))
      {
        /* Reception is not complete */
        huart->RxXferCount = nb_remaining_rx_data;

        /* In Normal mode, end DMA xfer and HAL UART Rx process*/
        if (huart->hdmarx->Init.Mode != DMA_CIRCULAR)
        {
          /* Disable PE and ERR (Frame error, noise error, overrun error) interrupts */
          ATOMIC_CLEAR_BIT(huart->Instance->CR1, USART_CR1_PEIE);
          ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE);

          /* Disable the DMA transfer for the receiver request by resetting the DMAR bit
             in the UART CR3 register */
          ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAR);

          /* At end of Rx process, restore huart->RxState to Ready */
          huart->RxState = HAL_UART_STATE_READY;
          huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;

          ATOMIC_CLEAR_BIT(huart->Instance->CR1, USART_CR1_IDLEIE);

          /* Last bytes received, so no need as the abort is immediate */
          (void)HAL_DMA_Abort(huart->hdmarx);
        }

        /* Initialize type of RxEvent that correspond to RxEvent callback execution;
        In this case, Rx Event type is Idle Event */
        huart->RxEventType = HAL_UART_RXEVENT_IDLE;

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
        /*Call registered Rx Event callback*/
        huart->RxEventCallback(huart, (huart->RxXferSize - huart->RxXferCount));
#else
        /*Call legacy weak Rx Event callback*/
        HAL_UARTEx_RxEventCallback(huart, (huart->RxXferSize - huart->RxXferCount));
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
      }
      return;
    }
    else
    {
      /* DMA mode not enabled */
      /* Check received length : If all expected data are received, do nothing.
         Otherwise, if at least one data has already been received, IDLE event is to be notified to user */
      uint16_t nb_rx_data = huart->RxXferSize - huart->RxXferCount;
      if ((huart->RxXferCount > 0U)
          && (nb_rx_data > 0U))
      {
        /* Disable the UART Parity Error Interrupt and RXNE interrupts */
        ATOMIC_CLEAR_BIT(huart->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE));

        /* Disable the UART Error Interrupt: (Frame error, noise error, overrun error) */
        ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE);

        /* Rx process is completed, restore huart->RxState to Ready */
        huart->RxState = HAL_UART_STATE_READY;
        huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;

        ATOMIC_CLEAR_BIT(huart->Instance->CR1, USART_CR1_IDLEIE);

        /* Initialize type of RxEvent that correspond to RxEvent callback execution;
           In this case, Rx Event type is Idle Event */
        huart->RxEventType = HAL_UART_RXEVENT_IDLE;

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
        /*Call registered Rx complete callback*/
        huart->RxEventCallback(huart, nb_rx_data);
#else
        /*Call legacy weak Rx Event callback*/
        HAL_UARTEx_RxEventCallback(huart, nb_rx_data);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
      }
      return;
    }
  }

  /* UART in mode Transmitter ------------------------------------------------*/
  if (((isrflags & USART_SR_TXE) != RESET) && ((cr1its & USART_CR1_TXEIE) != RESET))
  {
    UART_Transmit_IT(huart);
    return;
  }

  /* UART in mode Transmitter end --------------------------------------------*/
  if (((isrflags & USART_SR_TC) != RESET) && ((cr1its & USART_CR1_TCIE) != RESET))
  {
    UART_EndTransmit_IT(huart);
    return;
  }
}

/**
  * @brief  Tx Transfer completed callbacks.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */
__weak void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);
  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_UART_TxCpltCallback could be implemented in the user file
   */
}

/**
  * @brief  Tx Half Transfer completed callbacks.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */
__weak void HAL_UART_TxHalfCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);
  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_UART_TxHalfCpltCallback could be implemented in the user file
   */
}

/**
  * @brief  Rx Transfer completed callbacks.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */
__weak void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);
  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_UART_RxCpltCallback could be implemented in the user file
   */
}

/**
  * @brief  Rx Half Transfer completed callbacks.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */
__weak void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);
  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_UART_RxHalfCpltCallback could be implemented in the user file
   */
}

/**
  * @brief  UART error callbacks.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */
__weak void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);
  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_UART_ErrorCallback could be implemented in the user file
   */
}

/**
  * @brief  UART Abort Complete callback.
  * @param  huart UART handle.
  * @retval None
  */
__weak void HAL_UART_AbortCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_UART_AbortCpltCallback can be implemented in the user file.
   */
}

/**
  * @brief  UART Abort Complete callback.
  * @param  huart UART handle.
  * @retval None
  */
__weak void HAL_UART_AbortTransmitCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_UART_AbortTransmitCpltCallback can be implemented in the user file.
   */
}

/**
  * @brief  UART Abort Receive Complete callback.
  * @param  huart UART handle.
  * @retval None
  */
__weak void HAL_UART_AbortReceiveCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_UART_AbortReceiveCpltCallback can be implemented in the user file.
   */
}

/**
  * @brief  Reception Event Callback (Rx event notification called after use of advanced reception service).
  * @param  huart UART handle
  * @param  Size  Number of data available in application reception buffer (indicates a position in
  *               reception buffer until which, data are available)
  * @retval None
  */
__weak void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);
  UNUSED(Size);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_UARTEx_RxEventCallback can be implemented in the user file.
   */
}

/**
  * @}
  */

/** @defgroup UART_Exported_Functions_Group3 Peripheral Control functions
  *  @brief   UART control functions
  *
@verbatim
  ==============================================================================
                      ##### Peripheral Control functions #####
  ==============================================================================
  [..]
    This subsection provides a set of functions allowing to control the UART:
    (+) HAL_LIN_SendBreak() API can be helpful to transmit the break character.
    (+) HAL_MultiProcessor_EnterMuteMode() API can be helpful to enter the UART in mute mode.
    (+) HAL_MultiProcessor_ExitMuteMode() API can be helpful to exit the UART mute mode by software.
    (+) HAL_HalfDuplex_EnableTransmitter() API to enable the UART transmitter and disables the UART receiver in Half Duplex mode
    (+) HAL_HalfDuplex_EnableReceiver() API to enable the UART receiver and disables the UART transmitter in Half Duplex mode

@endverbatim
  * @{
  */

/**
  * @brief  Transmits break characters.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_LIN_SendBreak(UART_HandleTypeDef *huart)
{
  /* Check the parameters */
  assert_param(IS_UART_INSTANCE(huart->Instance));

  /* Process Locked */
  __HAL_LOCK(huart);

  huart->gState = HAL_UART_STATE_BUSY;

  /* Send break characters */
  ATOMIC_SET_BIT(huart->Instance->CR1, USART_CR1_SBK);

  huart->gState = HAL_UART_STATE_READY;

  /* Process Unlocked */
  __HAL_UNLOCK(huart);

  return HAL_OK;
}

/**
  * @brief  Enters the UART in mute mode.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_MultiProcessor_EnterMuteMode(UART_HandleTypeDef *huart)
{
  /* Check the parameters */
  assert_param(IS_UART_INSTANCE(huart->Instance));

  /* Process Locked */
  __HAL_LOCK(huart);

  huart->gState = HAL_UART_STATE_BUSY;

  /* Enable the USART mute mode  by setting the RWU bit in the CR1 register */
  ATOMIC_SET_BIT(huart->Instance->CR1, USART_CR1_RWU);

  huart->gState = HAL_UART_STATE_READY;
  huart->RxEventType = HAL_UART_RXEVENT_TC;

  /* Process Unlocked */
  __HAL_UNLOCK(huart);

  return HAL_OK;
}

/**
  * @brief  Exits the UART mute mode: wake up software.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_MultiProcessor_ExitMuteMode(UART_HandleTypeDef *huart)
{
  /* Check the parameters */
  assert_param(IS_UART_INSTANCE(huart->Instance));

  /* Process Locked */
  __HAL_LOCK(huart);

  huart->gState = HAL_UART_STATE_BUSY;

  /* Disable the USART mute mode by clearing the RWU bit in the CR1 register */
  ATOMIC_CLEAR_BIT(huart->Instance->CR1, USART_CR1_RWU);

  huart->gState = HAL_UART_STATE_READY;
  huart->RxEventType = HAL_UART_RXEVENT_TC;

  /* Process Unlocked */
  __HAL_UNLOCK(huart);

  return HAL_OK;
}

/**
  * @brief  Enables the UART transmitter and disables the UART receiver.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HalfDuplex_EnableTransmitter(UART_HandleTypeDef *huart)
{
  uint32_t tmpreg = 0x00U;

  /* Process Locked */
  __HAL_LOCK(huart);

  huart->gState = HAL_UART_STATE_BUSY;

  /*-------------------------- USART CR1 Configuration -----------------------*/
  tmpreg = huart->Instance->CR1;

  /* Clear TE and RE bits */
  tmpreg &= (uint32_t)~((uint32_t)(USART_CR1_TE | USART_CR1_RE));

  /* Enable the USART's transmit interface by setting the TE bit in the USART CR1 register */
  tmpreg |= (uint32_t)USART_CR1_TE;

  /* Write to USART CR1 */
  WRITE_REG(huart->Instance->CR1, (uint32_t)tmpreg);

  huart->gState = HAL_UART_STATE_READY;

  /* Process Unlocked */
  __HAL_UNLOCK(huart);

  return HAL_OK;
}

/**
  * @brief  Enables the UART receiver and disables the UART transmitter.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HalfDuplex_EnableReceiver(UART_HandleTypeDef *huart)
{
  uint32_t tmpreg = 0x00U;

  /* Process Locked */
  __HAL_LOCK(huart);

  huart->gState = HAL_UART_STATE_BUSY;

  /*-------------------------- USART CR1 Configuration -----------------------*/
  tmpreg = huart->Instance->CR1;

  /* Clear TE and RE bits */
  tmpreg &= (uint32_t)~((uint32_t)(USART_CR1_TE | USART_CR1_RE));

  /* Enable the USART's receive interface by setting the RE bit in the USART CR1 register */
  tmpreg |= (uint32_t)USART_CR1_RE;

  /* Write to USART CR1 */
  WRITE_REG(huart->Instance->CR1, (uint32_t)tmpreg);

  huart->gState = HAL_UART_STATE_READY;

  /* Process Unlocked */
  __HAL_UNLOCK(huart);

  return HAL_OK;
}

/**
  * @}
  */

/** @defgroup UART_Exported_Functions_Group4 Peripheral State and Errors functions
  *  @brief   UART State and Errors functions
  *
@verbatim
  ==============================================================================
                 ##### Peripheral State and Errors functions #####
  ==============================================================================
 [..]
   This subsection provides a set of functions allowing to return the State of
   UART communication process, return Peripheral Errors occurred during communication
   process
   (+) HAL_UART_GetState() API can be helpful to check in run-time the state of the UART peripheral.
   (+) HAL_UART_GetError() check in run-time errors that could be occurred during communication.

@endverbatim
  * @{
  */

/**
  * @brief  Returns the UART state.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval HAL state
  */
HAL_UART_StateTypeDef HAL_UART_GetState(const UART_HandleTypeDef *huart)
{
  uint32_t temp1 = 0x00U, temp2 = 0x00U;
  temp1 = huart->gState;
  temp2 = huart->RxState;

  return (HAL_UART_StateTypeDef)(temp1 | temp2);
}

/**
  * @brief  Return the UART error code
  * @param  huart Pointer to a UART_HandleTypeDef structure that contains
  *               the configuration information for the specified UART.
  * @retval UART Error Code
  */
uint32_t HAL_UART_GetError(const UART_HandleTypeDef *huart)
{
  return huart->ErrorCode;
}

/**
  * @}
  */

/**
  * @}
  */

/** @defgroup UART_Private_Functions UART Private Functions
  * @{
  */

/**
  * @brief  Initialize the callbacks to their default values.
  * @param  huart UART handle.
  * @retval none
  */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
void UART_InitCallbacksToDefault(UART_HandleTypeDef *huart)
{
  /* Init the UART Callback settings */
  huart->TxHalfCpltCallback        = HAL_UART_TxHalfCpltCallback;        /* Legacy weak TxHalfCpltCallback        */
  huart->TxCpltCallback            = HAL_UART_TxCpltCallback;            /* Legacy weak TxCpltCallback            */
  huart->RxHalfCpltCallback        = HAL_UART_RxHalfCpltCallback;        /* Legacy weak RxHalfCpltCallback        */
  huart->RxCpltCallback            = HAL_UART_RxCpltCallback;            /* Legacy weak RxCpltCallback            */
  huart->ErrorCallback             = HAL_UART_ErrorCallback;             /* Legacy weak ErrorCallback             */
  huart->AbortCpltCallback         = HAL_UART_AbortCpltCallback;         /* Legacy weak AbortCpltCallback         */
  huart->AbortTransmitCpltCallback = HAL_UART_AbortTransmitCpltCallback; /* Legacy weak AbortTransmitCpltCallback */
  huart->AbortReceiveCpltCallback  = HAL_UART_AbortReceiveCpltCallback;  /* Legacy weak AbortReceiveCpltCallback  */
  huart->RxEventCallback           = HAL_UARTEx_RxEventCallback;         /* Legacy weak RxEventCallback           */

}
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */

/**
  * @brief  DMA UART transmit process complete callback.
  * @param  hdma  Pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA module.
  * @retval None
  */
static void UART_DMATransmitCplt(DMA_HandleTypeDef *hdma)
{
  UART_HandleTypeDef *huart = (UART_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;
  /* DMA Normal mode*/
  if ((hdma->Instance->CCR & DMA_CCR_CIRC) == 0U)
  {
    huart->TxXferCount = 0x00U;

    /* Disable the DMA transfer for transmit request by setting the DMAT bit
       in the UART CR3 register */
    ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAT);

    /* Enable the UART Transmit Complete Interrupt */
    ATOMIC_SET_BIT(huart->Instance->CR1, USART_CR1_TCIE);

  }
  /* DMA Circular mode */
  else
  {
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
    /*Call registered Tx complete callback*/
    huart->TxCpltCallback(huart);
#else
    /*Call legacy weak Tx complete callback*/
    HAL_UART_TxCpltCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
  }
}

/**
  * @brief DMA UART transmit process half complete callback
  * @param  hdma  Pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA module.
  * @retval None
  */
static void UART_DMATxHalfCplt(DMA_HandleTypeDef *hdma)
{
  UART_HandleTypeDef *huart = (UART_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
  /*Call registered Tx complete callback*/
  huart->TxHalfCpltCallback(huart);
#else
  /*Call legacy weak Tx complete callback*/
  HAL_UART_TxHalfCpltCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
}

/**
  * @brief  DMA UART receive process complete callback.
  * @param  hdma  Pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA module.
  * @retval None
  */
static void UART_DMAReceiveCplt(DMA_HandleTypeDef *hdma)
{
  UART_HandleTypeDef *huart = (UART_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;

  /* DMA Normal mode*/
  if ((hdma->Instance->CCR & DMA_CCR_CIRC) == 0U)
  {
    huart->RxXferCount = 0U;

    /* Disable RXNE, PE and ERR (Frame error, noise error, overrun error) interrupts */
    ATOMIC_CLEAR_BIT(huart->Instance->CR1, USART_CR1_PEIE);
    ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE);

    /* Disable the DMA transfer for the receiver request by setting the DMAR bit
       in the UART CR3 register */
    ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAR);

    /* At end of Rx process, restore huart->RxState to Ready */
    huart->RxState = HAL_UART_STATE_READY;

    /* If Reception till IDLE event has been selected, Disable IDLE Interrupt */
    if (huart->ReceptionType == HAL_UART_RECEPTION_TOIDLE)
    {
      ATOMIC_CLEAR_BIT(huart->Instance->CR1, USART_CR1_IDLEIE);
    }
  }

  /* Initialize type of RxEvent that correspond to RxEvent callback execution;
   In this case, Rx Event type is Transfer Complete */
  huart->RxEventType = HAL_UART_RXEVENT_TC;

  /* Check current reception Mode :
     If Reception till IDLE event has been selected : use Rx Event callback */
  if (huart->ReceptionType == HAL_UART_RECEPTION_TOIDLE)
  {
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
    /*Call registered Rx Event callback*/
    huart->RxEventCallback(huart, huart->RxXferSize);
#else
    /*Call legacy weak Rx Event callback*/
    HAL_UARTEx_RxEventCallback(huart, huart->RxXferSize);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
  }
  else
  {
    /* In other cases : use Rx Complete callback */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
    /*Call registered Rx complete callback*/
    huart->RxCpltCallback(huart);
#else
    /*Call legacy weak Rx complete callback*/
    HAL_UART_RxCpltCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
  }
}

/**
  * @brief DMA UART receive process half complete callback
  * @param  hdma  Pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA module.
  * @retval None
  */
static void UART_DMARxHalfCplt(DMA_HandleTypeDef *hdma)
{
  UART_HandleTypeDef *huart = (UART_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;

  /* Initialize type of RxEvent that correspond to RxEvent callback execution;
     In this case, Rx Event type is Half Transfer */
  huart->RxEventType = HAL_UART_RXEVENT_HT;

  /* Check current reception Mode :
     If Reception till IDLE event has been selected : use Rx Event callback */
  if (huart->ReceptionType == HAL_UART_RECEPTION_TOIDLE)
  {
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
    /*Call registered Rx Event callback*/
    huart->RxEventCallback(huart, huart->RxXferSize / 2U);
#else
    /*Call legacy weak Rx Event callback*/
    HAL_UARTEx_RxEventCallback(huart, huart->RxXferSize / 2U);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
  }
  else
  {
    /* In other cases : use Rx Half Complete callback */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
    /*Call registered Rx Half complete callback*/
    huart->RxHalfCpltCallback(huart);
#else
    /*Call legacy weak Rx Half complete callback*/
    HAL_UART_RxHalfCpltCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
  }
}

/**
  * @brief  DMA UART communication error callback.
  * @param  hdma  Pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA module.
  * @retval None
  */
static void UART_DMAError(DMA_HandleTypeDef *hdma)
{
  uint32_t dmarequest = 0x00U;
  UART_HandleTypeDef *huart = (UART_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;

  /* Stop UART DMA Tx request if ongoing */
  dmarequest = HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAT);
  if ((huart->gState == HAL_UART_STATE_BUSY_TX) && dmarequest)
  {
    huart->TxXferCount = 0x00U;
    UART_EndTxTransfer(huart);
  }

  /* Stop UART DMA Rx request if ongoing */
  dmarequest = HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAR);
  if ((huart->RxState == HAL_UART_STATE_BUSY_RX) && dmarequest)
  {
    huart->RxXferCount = 0x00U;
    UART_EndRxTransfer(huart);
  }

  huart->ErrorCode |= HAL_UART_ERROR_DMA;
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
  /*Call registered error callback*/
  huart->ErrorCallback(huart);
#else
  /*Call legacy weak error callback*/
  HAL_UART_ErrorCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
}

/**
  * @brief  This function handles UART Communication Timeout. It waits
  *         until a flag is no longer in the specified status.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @param  Flag specifies the UART flag to check.
  * @param  Status The actual Flag status (SET or RESET).
  * @param  Tickstart Tick start value
  * @param  Timeout Timeout duration
  * @retval HAL status
  */
static HAL_StatusTypeDef UART_WaitOnFlagUntilTimeout(UART_HandleTypeDef *huart, uint32_t Flag, FlagStatus Status,
                                                     uint32_t Tickstart, uint32_t Timeout)
{
  /* Wait until flag is set */
  while ((__HAL_UART_GET_FLAG(huart, Flag) ? SET : RESET) == Status)
  {
    /* Check for the Timeout */
    if (Timeout != HAL_MAX_DELAY)
    {
      if (((HAL_GetTick() - Tickstart) > Timeout) || (Timeout == 0U))
      {

        return HAL_TIMEOUT;
      }

      if ((READ_BIT(huart->Instance->CR1, USART_CR1_RE) != 0U) && (Flag != UART_FLAG_TXE) && (Flag != UART_FLAG_TC))
      {
        if (__HAL_UART_GET_FLAG(huart, UART_FLAG_ORE) == SET)
        {
          /* Clear Overrun Error flag*/
          __HAL_UART_CLEAR_OREFLAG(huart);

          /* Blocking error : transfer is aborted
          Set the UART state ready to be able to start again the process,
          Disable Rx Interrupts if ongoing */
          UART_EndRxTransfer(huart);

          huart->ErrorCode = HAL_UART_ERROR_ORE;

          /* Process Unlocked */
          __HAL_UNLOCK(huart);

          return HAL_ERROR;
        }
      }
    }
  }
  return HAL_OK;
}

/**
  * @brief  Start Receive operation in interrupt mode.
  * @note   This function could be called by all HAL UART API providing reception in Interrupt mode.
  * @note   When calling this function, parameters validity is considered as already checked,
  *         i.e. Rx State, buffer address, ...
  *         UART Handle is assumed as Locked.
  * @param  huart UART handle.
  * @param  pData Pointer to data buffer (u8 or u16 data elements).
  * @param  Size  Amount of data elements (u8 or u16) to be received.
  * @retval HAL status
  */
HAL_StatusTypeDef UART_Start_Receive_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
  huart->pRxBuffPtr = pData;
  huart->RxXferSize = Size;
  huart->RxXferCount = Size;

  huart->ErrorCode = HAL_UART_ERROR_NONE;
  huart->RxState = HAL_UART_STATE_BUSY_RX;

  if (huart->Init.Parity != UART_PARITY_NONE)
  {
    /* Enable the UART Parity Error Interrupt */
    __HAL_UART_ENABLE_IT(huart, UART_IT_PE);
  }

  /* Enable the UART Error Interrupt: (Frame error, noise error, overrun error) */
  __HAL_UART_ENABLE_IT(huart, UART_IT_ERR);

  /* Enable the UART Data Register not empty Interrupt */
  __HAL_UART_ENABLE_IT(huart, UART_IT_RXNE);

  return HAL_OK;
}

/**
  * @brief  Start Receive operation in DMA mode.
  * @note   This function could be called by all HAL UART API providing reception in DMA mode.
  * @note   When calling this function, parameters validity is considered as already checked,
  *         i.e. Rx State, buffer address, ...
  *         UART Handle is assumed as Locked.
  * @param  huart UART handle.
  * @param  pData Pointer to data buffer (u8 or u16 data elements).
  * @param  Size  Amount of data elements (u8 or u16) to be received.
  * @retval HAL status
  */
HAL_StatusTypeDef UART_Start_Receive_DMA(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
  uint32_t *tmp;

  huart->pRxBuffPtr = pData;
  huart->RxXferSize = Size;

  huart->ErrorCode = HAL_UART_ERROR_NONE;
  huart->RxState = HAL_UART_STATE_BUSY_RX;

  /* Set the UART DMA transfer complete callback */
  huart->hdmarx->XferCpltCallback = UART_DMAReceiveCplt;

  /* Set the UART DMA Half transfer complete callback */
  huart->hdmarx->XferHalfCpltCallback = UART_DMARxHalfCplt;

  /* Set the DMA error callback */
  huart->hdmarx->XferErrorCallback = UART_DMAError;

  /* Set the DMA abort callback */
  huart->hdmarx->XferAbortCallback = NULL;

  /* Enable the DMA stream */
  tmp = (uint32_t *)&pData;
  HAL_DMA_Start_IT(huart->hdmarx, (uint32_t)&huart->Instance->DR, *(uint32_t *)tmp, Size);

  /* Clear the Overrun flag just before enabling the DMA Rx request: can be mandatory for the second transfer */
  __HAL_UART_CLEAR_OREFLAG(huart);

  if (huart->Init.Parity != UART_PARITY_NONE)
  {
    /* Enable the UART Parity Error Interrupt */
    ATOMIC_SET_BIT(huart->Instance->CR1, USART_CR1_PEIE);
  }

  /* Enable the UART Error Interrupt: (Frame error, noise error, overrun error) */
  ATOMIC_SET_BIT(huart->Instance->CR3, USART_CR3_EIE);

  /* Enable the DMA transfer for the receiver request by setting the DMAR bit
  in the UART CR3 register */
  ATOMIC_SET_BIT(huart->Instance->CR3, USART_CR3_DMAR);

  return HAL_OK;
}

/**
  * @brief  End ongoing Tx transfer on UART peripheral (following error detection or Transmit completion).
  * @param  huart UART handle.
  * @retval None
  */
static void UART_EndTxTransfer(UART_HandleTypeDef *huart)
{
  /* Disable TXEIE and TCIE interrupts */
  ATOMIC_CLEAR_BIT(huart->Instance->CR1, (USART_CR1_TXEIE | USART_CR1_TCIE));

  /* At end of Tx process, restore huart->gState to Ready */
  huart->gState = HAL_UART_STATE_READY;
}

/**
  * @brief  End ongoing Rx transfer on UART peripheral (following error detection or Reception completion).
  * @param  huart UART handle.
  * @retval None
  */
static void UART_EndRxTransfer(UART_HandleTypeDef *huart)
{
  /* Disable RXNE, PE and ERR (Frame error, noise error, overrun error) interrupts */
  ATOMIC_CLEAR_BIT(huart->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE));
  ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE);

  /* In case of reception waiting for IDLE event, disable also the IDLE IE interrupt source */
  if (huart->ReceptionType == HAL_UART_RECEPTION_TOIDLE)
  {
    ATOMIC_CLEAR_BIT(huart->Instance->CR1, USART_CR1_IDLEIE);
  }

  /* At end of Rx process, restore huart->RxState to Ready */
  huart->RxState = HAL_UART_STATE_READY;
  huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;
}

/**
  * @brief  DMA UART communication abort callback, when initiated by HAL services on Error
  *         (To be called at end of DMA Abort procedure following error occurrence).
  * @param  hdma  Pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA module.
  * @retval None
  */
static void UART_DMAAbortOnError(DMA_HandleTypeDef *hdma)
{
  UART_HandleTypeDef *huart = (UART_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;
  huart->RxXferCount = 0x00U;
  huart->TxXferCount = 0x00U;

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
  /*Call registered error callback*/
  huart->ErrorCallback(huart);
#else
  /*Call legacy weak error callback*/
  HAL_UART_ErrorCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
}

/**
  * @brief  DMA UART Tx communication abort callback, when initiated by user
  *         (To be called at end of DMA Tx Abort procedure following user abort request).
  * @note   When this callback is executed, User Abort complete call back is called only if no
  *         Abort still ongoing for Rx DMA Handle.
  * @param  hdma  Pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA module.
  * @retval None
  */
static void UART_DMATxAbortCallback(DMA_HandleTypeDef *hdma)
{
  UART_HandleTypeDef *huart = (UART_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;

  huart->hdmatx->XferAbortCallback = NULL;

  /* Check if an Abort process is still ongoing */
  if (huart->hdmarx != NULL)
  {
    if (huart->hdmarx->XferAbortCallback != NULL)
    {
      return;
    }
  }

  /* No Abort process still ongoing : All DMA channels are aborted, call user Abort Complete callback */
  huart->TxXferCount = 0x00U;
  huart->RxXferCount = 0x00U;

  /* Reset ErrorCode */
  huart->ErrorCode = HAL_UART_ERROR_NONE;

  /* Restore huart->gState and huart->RxState to Ready */
  huart->gState  = HAL_UART_STATE_READY;
  huart->RxState = HAL_UART_STATE_READY;
  huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;

  /* Call user Abort complete callback */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
  /* Call registered Abort complete callback */
  huart->AbortCpltCallback(huart);
#else
  /* Call legacy weak Abort complete callback */
  HAL_UART_AbortCpltCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
}

/**
  * @brief  DMA UART Rx communication abort callback, when initiated by user
  *         (To be called at end of DMA Rx Abort procedure following user abort request).
  * @note   When this callback is executed, User Abort complete call back is called only if no
  *         Abort still ongoing for Tx DMA Handle.
  * @param  hdma  Pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA module.
  * @retval None
  */
static void UART_DMARxAbortCallback(DMA_HandleTypeDef *hdma)
{
  UART_HandleTypeDef *huart = (UART_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;

  huart->hdmarx->XferAbortCallback = NULL;

  /* Check if an Abort process is still ongoing */
  if (huart->hdmatx != NULL)
  {
    if (huart->hdmatx->XferAbortCallback != NULL)
    {
      return;
    }
  }

  /* No Abort process still ongoing : All DMA channels are aborted, call user Abort Complete callback */
  huart->TxXferCount = 0x00U;
  huart->RxXferCount = 0x00U;

  /* Reset ErrorCode */
  huart->ErrorCode = HAL_UART_ERROR_NONE;

  /* Restore huart->gState and huart->RxState to Ready */
  huart->gState  = HAL_UART_STATE_READY;
  huart->RxState = HAL_UART_STATE_READY;
  huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;

  /* Call user Abort complete callback */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
  /* Call registered Abort complete callback */
  huart->AbortCpltCallback(huart);
#else
  /* Call legacy weak Abort complete callback */
  HAL_UART_AbortCpltCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
}

/**
  * @brief  DMA UART Tx communication abort callback, when initiated by user by a call to
  *         HAL_UART_AbortTransmit_IT API (Abort only Tx transfer)
  *         (This callback is executed at end of DMA Tx Abort procedure following user abort request,
  *         and leads to user Tx Abort Complete callback execution).
  * @param  hdma  Pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA module.
  * @retval None
  */
static void UART_DMATxOnlyAbortCallback(DMA_HandleTypeDef *hdma)
{
  UART_HandleTypeDef *huart = (UART_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;

  huart->TxXferCount = 0x00U;

  /* Restore huart->gState to Ready */
  huart->gState = HAL_UART_STATE_READY;

  /* Call user Abort complete callback */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
  /* Call registered Abort Transmit Complete Callback */
  huart->AbortTransmitCpltCallback(huart);
#else
  /* Call legacy weak Abort Transmit Complete Callback */
  HAL_UART_AbortTransmitCpltCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
}

/**
  * @brief  DMA UART Rx communication abort callback, when initiated by user by a call to
  *         HAL_UART_AbortReceive_IT API (Abort only Rx transfer)
  *         (This callback is executed at end of DMA Rx Abort procedure following user abort request,
  *         and leads to user Rx Abort Complete callback execution).
  * @param  hdma  Pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA module.
  * @retval None
  */
static void UART_DMARxOnlyAbortCallback(DMA_HandleTypeDef *hdma)
{
  UART_HandleTypeDef *huart = (UART_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;

  huart->RxXferCount = 0x00U;

  /* Restore huart->RxState to Ready */
  huart->RxState = HAL_UART_STATE_READY;
  huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;

  /* Call user Abort complete callback */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
  /* Call registered Abort Receive Complete Callback */
  huart->AbortReceiveCpltCallback(huart);
#else
  /* Call legacy weak Abort Receive Complete Callback */
  HAL_UART_AbortReceiveCpltCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
}

/**
  * @brief  Sends an amount of data in non blocking mode.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval HAL status
  */
static HAL_StatusTypeDef UART_Transmit_IT(UART_HandleTypeDef *huart)
{
  const uint16_t *tmp;

  /* Check that a Tx process is ongoing */
  if (huart->gState == HAL_UART_STATE_BUSY_TX)
  {
    if ((huart->Init.WordLength == UART_WORDLENGTH_9B) && (huart->Init.Parity == UART_PARITY_NONE))
    {
      tmp = (const uint16_t *) huart->pTxBuffPtr;
      huart->Instance->DR = (uint16_t)(*tmp & (uint16_t)0x01FF);
      huart->pTxBuffPtr += 2U;
    }
    else
    {
      huart->Instance->DR = (uint8_t)(*huart->pTxBuffPtr++ & (uint8_t)0x00FF);
    }

    if (--huart->TxXferCount == 0U)
    {
      /* Disable the UART Transmit Data Register Empty Interrupt */
      __HAL_UART_DISABLE_IT(huart, UART_IT_TXE);

      /* Enable the UART Transmit Complete Interrupt */
      __HAL_UART_ENABLE_IT(huart, UART_IT_TC);
    }
    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Wraps up transmission in non blocking mode.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval HAL status
  */
static HAL_StatusTypeDef UART_EndTransmit_IT(UART_HandleTypeDef *huart)
{
  /* Disable the UART Transmit Complete Interrupt */
  __HAL_UART_DISABLE_IT(huart, UART_IT_TC);

  /* Tx process is ended, restore huart->gState to Ready */
  huart->gState = HAL_UART_STATE_READY;

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
  /*Call registered Tx complete callback*/
  huart->TxCpltCallback(huart);
#else
  /*Call legacy weak Tx complete callback*/
  HAL_UART_TxCpltCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */

  return HAL_OK;
}

/**
  * @brief  Receives an amount of data in non blocking mode
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval HAL status
  */
static HAL_StatusTypeDef UART_Receive_IT(UART_HandleTypeDef *huart)
{
  uint8_t  *pdata8bits;
  uint16_t *pdata16bits;

  /* Check that a Rx process is ongoing */
  if (huart->RxState == HAL_UART_STATE_BUSY_RX)
  {
    if ((huart->Init.WordLength == UART_WORDLENGTH_9B) && (huart->Init.Parity == UART_PARITY_NONE))
    {
      pdata8bits  = NULL;
      pdata16bits = (uint16_t *) huart->pRxBuffPtr;
      *pdata16bits = (uint16_t)(huart->Instance->DR & (uint16_t)0x01FF);
      huart->pRxBuffPtr += 2U;
    }
    else
    {
      pdata8bits = (uint8_t *) huart->pRxBuffPtr;
      pdata16bits  = NULL;

      if ((huart->Init.WordLength == UART_WORDLENGTH_9B) || ((huart->Init.WordLength == UART_WORDLENGTH_8B) && (huart->Init.Parity == UART_PARITY_NONE)))
      {
        *pdata8bits = (uint8_t)(huart->Instance->DR & (uint8_t)0x00FF);
      }
      else
      {
        *pdata8bits = (uint8_t)(huart->Instance->DR & (uint8_t)0x007F);
      }
      huart->pRxBuffPtr += 1U;
    }

    if (--huart->RxXferCount == 0U)
    {
      /* Disable the UART Data Register not empty Interrupt */
      __HAL_UART_DISABLE_IT(huart, UART_IT_RXNE);

      /* Disable the UART Parity Error Interrupt */
      __HAL_UART_DISABLE_IT(huart, UART_IT_PE);

      /* Disable the UART Error Interrupt: (Frame error, noise error, overrun error) */
      __HAL_UART_DISABLE_IT(huart, UART_IT_ERR);

      /* Rx process is completed, restore huart->RxState to Ready */
      huart->RxState = HAL_UART_STATE_READY;

      /* Initialize type of RxEvent to Transfer Complete */
      huart->RxEventType = HAL_UART_RXEVENT_TC;

      /* Check current reception Mode :
         If Reception till IDLE event has been selected : */
      if (huart->ReceptionType == HAL_UART_RECEPTION_TOIDLE)
      {
        /* Set reception type to Standard */
        huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;

        /* Disable IDLE interrupt */
        ATOMIC_CLEAR_BIT(huart->Instance->CR1, USART_CR1_IDLEIE);

        /* Check if IDLE flag is set */
        if (__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE))
        {
          /* Clear IDLE flag in ISR */
          __HAL_UART_CLEAR_IDLEFLAG(huart);
        }

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
        /*Call registered Rx Event callback*/
        huart->RxEventCallback(huart, huart->RxXferSize);
#else
        /*Call legacy weak Rx Event callback*/
        HAL_UARTEx_RxEventCallback(huart, huart->RxXferSize);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
      }
      else
      {
        /* Standard reception API called */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
        /*Call registered Rx complete callback*/
        huart->RxCpltCallback(huart);
#else
        /*Call legacy weak Rx complete callback*/
        HAL_UART_RxCpltCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
      }

      return HAL_OK;
    }
    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Configures the UART peripheral.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */
static void UART_SetConfig(UART_HandleTypeDef *huart)
{
  uint32_t tmpreg;
  uint32_t pclk;

  /* Check the parameters */
  assert_param(IS_UART_BAUDRATE(huart->Init.BaudRate));
  assert_param(IS_UART_STOPBITS(huart->Init.StopBits));
  assert_param(IS_UART_PARITY(huart->Init.Parity));
  assert_param(IS_UART_MODE(huart->Init.Mode));

  /*-------------------------- USART CR2 Configuration -----------------------*/
  /* Configure the UART Stop Bits: Set STOP[13:12] bits
     according to huart->Init.StopBits value */
  MODIFY_REG(huart->Instance->CR2, USART_CR2_STOP, huart->Init.StopBits);

  /*-------------------------- USART CR1 Configuration -----------------------*/
  /* Configure the UART Word Length, Parity and mode:
     Set the M bits according to huart->Init.WordLength value
     Set PCE and PS bits according to huart->Init.Parity value
     Set TE and RE bits according to huart->Init.Mode value
     Set OVER8 bit according to huart->Init.OverSampling value */

#if defined(USART_CR1_OVER8)
  tmpreg = (uint32_t)huart->Init.WordLength | huart->Init.Parity | huart->Init.Mode | huart->Init.OverSampling;
  MODIFY_REG(huart->Instance->CR1,
             (uint32_t)(USART_CR1_M | USART_CR1_PCE | USART_CR1_PS | USART_CR1_TE | USART_CR1_RE | USART_CR1_OVER8),
             tmpreg);
#else
  tmpreg = (uint32_t)huart->Init.WordLength | huart->Init.Parity | huart->Init.Mode;
  MODIFY_REG(huart->Instance->CR1,
             (uint32_t)(USART_CR1_M | USART_CR1_PCE | USART_CR1_PS | USART_CR1_TE | USART_CR1_RE),
             tmpreg);
#endif /* USART_CR1_OVER8 */

  /*-------------------------- USART CR3 Configuration -----------------------*/
  /* Configure the UART HFC: Set CTSE and RTSE bits according to huart->Init.HwFlowCtl value */
  MODIFY_REG(huart->Instance->CR3, (USART_CR3_RTSE | USART_CR3_CTSE), huart->Init.HwFlowCtl);


  if(huart->Instance == USART1)
  {
    pclk = HAL_RCC_GetPCLK2Freq();
  }
  else
  {
    pclk = HAL_RCC_GetPCLK1Freq();
  }

  /*-------------------------- USART BRR Configuration ---------------------*/
#if defined(USART_CR1_OVER8)
  if (huart->Init.OverSampling == UART_OVERSAMPLING_8)
  {
    huart->Instance->BRR = UART_BRR_SAMPLING8(pclk, huart->Init.BaudRate);
  }
  else
  {
    huart->Instance->BRR = UART_BRR_SAMPLING16(pclk, huart->Init.BaudRate);
  }
#else
  huart->Instance->BRR = UART_BRR_SAMPLING16(pclk, huart->Init.BaudRate);
#endif /* USART_CR1_OVER8 */
}

/**
  * @}
  */

#endif /* HAL_UART_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */

