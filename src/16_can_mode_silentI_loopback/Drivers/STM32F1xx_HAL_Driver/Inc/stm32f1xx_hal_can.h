/**
  ******************************************************************************
  * @file    stm32f1xx_hal_can.h
  * @author  MCD Application Team
  * @brief   CAN HAL 模块头文件
  ******************************************************************************
  * @attention
  *
  * 版权所有 (c) 2016 STMicroelectronics.
  * 保留所有权利.
  *
  * 本软件根据 LICENSE 文件中的条款授权
  * 在本软件组件的根目录中可找到
  * 如果本软件未随附 LICENSE 文件，则按"原样"提供
  *
  ******************************************************************************
  */

/* 防止递归包含 -----------------------------------------------------------*/
#ifndef STM32F1xx_HAL_CAN_H
#define STM32F1xx_HAL_CAN_H

#ifdef __cplusplus
extern "C" {
#endif

/* 头文件包含 ------------------------------------------------------------------*/
#include "stm32f1xx_hal_def.h"

/** @addtogroup STM32F1xx_HAL_Driver STM32F1xx_HAL 驱动
  * @{
  */

#if defined (CAN1)
/** @addtogroup CAN CAN控制器
  * @{
  */

/* 导出类型 ------------------------------------------------------------*/
/** @defgroup CAN_Exported_Types CAN 导出类型
  * @{
  */
/**
  * @brief  HAL 状态结构体定义
  */
typedef enum
{
  HAL_CAN_STATE_RESET             = 0x00U,  /*!< CAN 未初始化或已禁用 */
  HAL_CAN_STATE_READY             = 0x01U,  /*!< CAN 已初始化并可使用   */
  HAL_CAN_STATE_LISTENING         = 0x02U,  /*!< CAN 接收进程进行中      */
  HAL_CAN_STATE_SLEEP_PENDING     = 0x03U,  /*!< CAN 睡眠请求等待中        */
  HAL_CAN_STATE_SLEEP_ACTIVE      = 0x04U,  /*!< CAN 睡眠模式已激活            */
  HAL_CAN_STATE_ERROR             = 0x05U   /*!< CAN 错误状态                     */

} HAL_CAN_StateTypeDef;

/**
  * @brief  CAN 初始化结构体定义
  */
typedef struct
{
  uint32_t Prescaler;                  /*!< 指定时间量子长度.
                                            此参数必须介于 Min_Data = 1 和 Max_Data = 1024 之间 */

  uint32_t Mode;                       /*!< 指定 CAN 工作模式.
                                            此参数可以是 @ref CAN_operating_mode 的值 */

  uint32_t SyncJumpWidth;              /*!< 指定 CAN 硬件允许延长或缩短一位以执行重同步的最大时间量子数.
                                            此参数可以是 @ref CAN_synchronisation_jump_width 的值 */

  uint32_t TimeSeg1;                   /*!< 指定位段1中的时间量子数.
                                            此参数可以是 @ref CAN_time_quantum_in_bit_segment_1 的值 */

  uint32_t TimeSeg2;                   /*!< 指定位段2中的时间量子数.
                                            此参数可以是 @ref CAN_time_quantum_in_bit_segment_2 的值 */

  FunctionalState TimeTriggeredMode;   /*!< 启用或禁用时间触发通信模式.
                                            此参数可设置为 ENABLE 或 DISABLE */

  FunctionalState AutoBusOff;          /*!< 启用或禁用自动总线关闭管理.
                                            此参数可设置为 ENABLE 或 DISABLE */

  FunctionalState AutoWakeUp;          /*!< 启用或禁用自动唤醒模式.
                                            此参数可设置为 ENABLE 或 DISABLE */

  FunctionalState AutoRetransmission;  /*!< 启用或禁用非自动重传模式.
                                            此参数可设置为 ENABLE 或 DISABLE */

  FunctionalState ReceiveFifoLocked;   /*!< 启用或禁用接收 FIFO 锁定模式.
                                            此参数可设置为 ENABLE 或 DISABLE */

  FunctionalState TransmitFifoPriority;/*!< 启用或禁用发送 FIFO 优先级.
                                            此参数可设置为 ENABLE 或 DISABLE */

} CAN_InitTypeDef;

/**
  * @brief  CAN 过滤器配置结构体定义
  */
typedef struct
{
  uint32_t FilterIdHigh;          /*!< 指定过滤器标识号（32位配置为 MSB，16位配置为第一个）
                                       此参数必须介于 Min_Data = 0x0000 和 Max_Data = 0xFFFF 之间 */

  uint32_t FilterIdLow;           /*!< 指定过滤器标识号（32位配置为 LSB，16位配置为第二个）
                                       此参数必须介于 Min_Data = 0x0000 和 Max_Data = 0xFFFF 之间 */

  uint32_t FilterMaskIdHigh;      /*!< 根据模式指定过滤器掩码号或标识号（32位配置为 MSB，16位配置为第一个）
                                       此参数必须介于 Min_Data = 0x0000 和 Max_Data = 0xFFFF 之间 */

  uint32_t FilterMaskIdLow;       /*!< 根据模式指定过滤器掩码号或标识号（32位配置为 LSB，16位配置为第二个）
                                       此参数必须介于 Min_Data = 0x0000 和 Max_Data = 0xFFFF 之间 */

  uint32_t FilterFIFOAssignment;  /*!< 指定将分配给过滤器的 FIFO（0 或 1）
                                       此参数可以是 @ref CAN_filter_FIFO 的值 */

  uint32_t FilterBank;            /*!< 指定要初始化的过滤器组
                                       对于单个 CAN 实例（14 个专用过滤器组），此参数必须介于 Min_Data = 0 和 Max_Data = 13 之间
                                       对于双 CAN 实例（28 个共享过滤器组），此参数必须介于 Min_Data = 0 和 Max_Data = 27 之间 */

  uint32_t FilterMode;            /*!< 指定要初始化的过滤器模式
                                       此参数可以是 @ref CAN_filter_mode 的值 */

  uint32_t FilterScale;           /*!< 指定过滤器比例
                                       此参数可以是 @ref CAN_filter_scale 的值 */

  uint32_t FilterActivation;      /*!< 启用或禁用过滤器
                                       此参数可以是 @ref CAN_filter_activation 的值 */

  uint32_t SlaveStartFilterBank;  /*!< 选择从 CAN 实例的起始过滤器组
                                       对于单个 CAN 实例，此参数无意义
                                       对于双 CAN 实例，所有索引较低的过滤器组分配给主 CAN 实例，索引较高的分配给从 CAN 实例
                                       此参数必须介于 Min_Data = 0 和 Max_Data = 27 之间 */

} CAN_FilterTypeDef;

/**
  * @brief  CAN 发送消息头结构体定义
  */
typedef struct
{
  uint32_t StdId;    /*!< 指定标准标识符
                          此参数必须介于 Min_Data = 0 和 Max_Data = 0x7FF 之间 */

  uint32_t ExtId;    /*!< 指定扩展标识符
                          此参数必须介于 Min_Data = 0 和 Max_Data = 0x1FFFFFFF 之间 */

  uint32_t IDE;      /*!< 指定要发送的消息的标识符类型
                          此参数可以是 @ref CAN_identifier_type 的值 */

  uint32_t RTR;      /*!< 指定要发送的消息的帧类型
                          此参数可以是 @ref CAN_remote_transmission_request 的值 */

  uint32_t DLC;      /*!< 指定要发送的帧的长度
                          此参数必须介于 Min_Data = 0 和 Max_Data = 8 之间 */

  FunctionalState TransmitGlobalTime; /*!< 指定是否在帧传输开始时捕获的时间戳计数器值
                          替换 pData[6] 和 pData[7] 后通过 DATA6 和 DATA7 发送
                          注意：必须启用时间触发通信模式
                          注意：DLC 必须编程为 8 字节，以便发送这 2 个字节
                          此参数可设置为 ENABLE 或 DISABLE */

} CAN_TxHeaderTypeDef;

/**
  * @brief  CAN 接收消息头结构体定义
  */
typedef struct
{
  uint32_t StdId;    /*!< 指定标准标识符
                          此参数必须介于 Min_Data = 0 和 Max_Data = 0x7FF 之间 */

  uint32_t ExtId;    /*!< 指定扩展标识符
                          此参数必须介于 Min_Data = 0 和 Max_Data = 0x1FFFFFFF 之间 */

  uint32_t IDE;      /*!< 指定接收消息的标识符类型
                          此参数可以是 @ref CAN_identifier_type 的值 */

  uint32_t RTR;      /*!< 指定接收消息的帧类型
                          此参数可以是 @ref CAN_remote_transmission_request 的值 */

  uint32_t DLC;      /*!< 指定接收帧的长度
                          此参数必须介于 Min_Data = 0 和 Max_Data = 8 之间 */

  uint32_t Timestamp; /*!< 指定在帧接收开始时捕获的时间戳计数器值
                          注意：必须启用时间触发通信模式
                          此参数必须介于 Min_Data = 0 和 Max_Data = 0xFFFF 之间 */

  uint32_t FilterMatchIndex; /*!< 指定匹配的接收验收过滤器元素的索引
                          此参数必须介于 Min_Data = 0 和 Max_Data = 0xFF 之间 */

} CAN_RxHeaderTypeDef;

/**
  * @brief  CAN 句柄结构体定义
  */
#if USE_HAL_CAN_REGISTER_CALLBACKS == 1
typedef struct __CAN_HandleTypeDef
#else
typedef struct
#endif /* USE_HAL_CAN_REGISTER_CALLBACKS */
{
  CAN_TypeDef                 *Instance;                 /*!< 寄存器基地址 */

  CAN_InitTypeDef             Init;                      /*!< CAN 必需参数 */

  __IO HAL_CAN_StateTypeDef   State;                     /*!< CAN 通信状态 */

  __IO uint32_t               ErrorCode;                 /*!< CAN 错误码
                                                              此参数可以是 @ref CAN_Error_Code 的值 */

#if USE_HAL_CAN_REGISTER_CALLBACKS == 1
  void (* TxMailbox0CompleteCallback)(struct __CAN_HandleTypeDef *hcan);/*!< CAN 发送邮箱 0 完成回调    */
  void (* TxMailbox1CompleteCallback)(struct __CAN_HandleTypeDef *hcan);/*!< CAN 发送邮箱 1 完成回调    */
  void (* TxMailbox2CompleteCallback)(struct __CAN_HandleTypeDef *hcan);/*!< CAN 发送邮箱 2 完成回调    */
  void (* TxMailbox0AbortCallback)(struct __CAN_HandleTypeDef *hcan);   /*!< CAN 发送邮箱 0 中止回调       */
  void (* TxMailbox1AbortCallback)(struct __CAN_HandleTypeDef *hcan);   /*!< CAN 发送邮箱 1 中止回调       */
  void (* TxMailbox2AbortCallback)(struct __CAN_HandleTypeDef *hcan);   /*!< CAN 发送邮箱 2 中止回调       */
  void (* RxFifo0MsgPendingCallback)(struct __CAN_HandleTypeDef *hcan); /*!< CAN 接收 FIFO 0 消息挂起回调    */
  void (* RxFifo0FullCallback)(struct __CAN_HandleTypeDef *hcan);       /*!< CAN 接收 FIFO 0 满回调           */
  void (* RxFifo1MsgPendingCallback)(struct __CAN_HandleTypeDef *hcan); /*!< CAN 接收 FIFO 1 消息挂起回调    */
  void (* RxFifo1FullCallback)(struct __CAN_HandleTypeDef *hcan);       /*!< CAN 接收 FIFO 1 满回调           */
  void (* SleepCallback)(struct __CAN_HandleTypeDef *hcan);             /*!< CAN 睡眠回调                    */
  void (* WakeUpFromRxMsgCallback)(struct __CAN_HandleTypeDef *hcan);   /*!< CAN 从接收消息唤醒回调      */
  void (* ErrorCallback)(struct __CAN_HandleTypeDef *hcan);             /*!< CAN 错误回调                    */

  void (* MspInitCallback)(struct __CAN_HandleTypeDef *hcan);           /*!< CAN MSP 初始化回调                 */
  void (* MspDeInitCallback)(struct __CAN_HandleTypeDef *hcan);         /*!< CAN MSP 反初始化回调               */

#endif /* (USE_HAL_CAN_REGISTER_CALLBACKS) */
} CAN_HandleTypeDef;

#if USE_HAL_CAN_REGISTER_CALLBACKS == 1
/**
  * @brief  HAL CAN 通用回调 ID 枚举定义
  */
typedef enum
{
  HAL_CAN_TX_MAILBOX0_COMPLETE_CB_ID       = 0x00U,    /*!< CAN 发送邮箱 0 完成回调 ID         */
  HAL_CAN_TX_MAILBOX1_COMPLETE_CB_ID       = 0x01U,    /*!< CAN 发送邮箱 1 完成回调 ID         */
  HAL_CAN_TX_MAILBOX2_COMPLETE_CB_ID       = 0x02U,    /*!< CAN 发送邮箱 2 完成回调 ID         */
  HAL_CAN_TX_MAILBOX0_ABORT_CB_ID          = 0x03U,    /*!< CAN 发送邮箱 0 中止回调 ID            */
  HAL_CAN_TX_MAILBOX1_ABORT_CB_ID          = 0x04U,    /*!< CAN 发送邮箱 1 中止回调 ID            */
  HAL_CAN_TX_MAILBOX2_ABORT_CB_ID          = 0x05U,    /*!< CAN 发送邮箱 2 中止回调 ID            */
  HAL_CAN_RX_FIFO0_MSG_PENDING_CB_ID       = 0x06U,    /*!< CAN 接收 FIFO 0 消息挂起回调 ID     */
  HAL_CAN_RX_FIFO0_FULL_CB_ID              = 0x07U,    /*!< CAN 接收 FIFO 0 满回调 ID                */
  HAL_CAN_RX_FIFO1_MSG_PENDING_CB_ID       = 0x08U,    /*!< CAN 接收 FIFO 1 消息挂起回调 ID     */
  HAL_CAN_RX_FIFO1_FULL_CB_ID              = 0x09U,    /*!< CAN 接收 FIFO 1 满回调 ID                */
  HAL_CAN_SLEEP_CB_ID                      = 0x0AU,    /*!< CAN 睡眠回调 ID                         */
  HAL_CAN_WAKEUP_FROM_RX_MSG_CB_ID         = 0x0BU,    /*!< CAN 从接收消息唤醒回调 ID          */
  HAL_CAN_ERROR_CB_ID                      = 0x0CU,    /*!< CAN 错误回调 ID                         */

  HAL_CAN_MSPINIT_CB_ID                    = 0x0DU,    /*!< CAN MSP 初始化回调 ID                       */
  HAL_CAN_MSPDEINIT_CB_ID                  = 0x0EU,    /*!< CAN MSP 反初始化回调 ID                     */

} HAL_CAN_CallbackIDTypeDef;

/**
  * @brief  HAL CAN 回调指针定义
  */
typedef  void (*pCAN_CallbackTypeDef)(CAN_HandleTypeDef *hcan); /*!< 指向 CAN 回调函数的指针   */

#endif /* USE_HAL_CAN_REGISTER_CALLBACKS */
/**
  * @}
  */

/* 导出常量 --------------------------------------------------------*/

/** @defgroup CAN_Exported_Constants CAN 导出常量
  * @{
  */

/** @defgroup CAN_Error_Code CAN 错误码
  * @{
  */
#define HAL_CAN_ERROR_NONE            (0x00000000U)  /*!< 无错误                                             */
#define HAL_CAN_ERROR_EWG             (0x00000001U)  /*!< 协议错误警告                               */
#define HAL_CAN_ERROR_EPV             (0x00000002U)  /*!< 错误被动                                        */
#define HAL_CAN_ERROR_BOF             (0x00000004U)  /*!< 总线关闭错误                                        */
#define HAL_CAN_ERROR_STF             (0x00000008U)  /*!< 位填充错误                                          */
#define HAL_CAN_ERROR_FOR             (0x00000010U)  /*!< 格式错误                                           */
#define HAL_CAN_ERROR_ACK             (0x00000020U)  /*!< 确认错误                                 */
#define HAL_CAN_ERROR_BR              (0x00000040U)  /*!< 位隐性错误                                  */
#define HAL_CAN_ERROR_BD              (0x00000080U)  /*!< 位显性错误                                   */
#define HAL_CAN_ERROR_CRC             (0x00000100U)  /*!< CRC 错误                                            */
#define HAL_CAN_ERROR_RX_FOV0         (0x00000200U)  /*!< 接收 FIFO0 溢出错误                               */
#define HAL_CAN_ERROR_RX_FOV1         (0x00000400U)  /*!< 接收 FIFO1 溢出错误                               */
#define HAL_CAN_ERROR_TX_ALST0        (0x00000800U)  /*!< 发送邮箱 0 因仲裁丢失导致发送失败 */
#define HAL_CAN_ERROR_TX_TERR0        (0x00001000U)  /*!< 发送邮箱 0 因发送错误导致发送失败   */
#define HAL_CAN_ERROR_TX_ALST1        (0x00002000U)  /*!< 发送邮箱 1 因仲裁丢失导致发送失败 */
#define HAL_CAN_ERROR_TX_TERR1        (0x00004000U)  /*!< 发送邮箱 1 因发送错误导致发送失败   */
#define HAL_CAN_ERROR_TX_ALST2        (0x00008000U)  /*!< 发送邮箱 2 因仲裁丢失导致发送失败 */
#define HAL_CAN_ERROR_TX_TERR2        (0x00010000U)  /*!< 发送邮箱 2 因发送错误导致发送失败   */
#define HAL_CAN_ERROR_TIMEOUT         (0x00020000U)  /*!< 超时错误                                        */
#define HAL_CAN_ERROR_NOT_INITIALIZED (0x00040000U)  /*!< 外设未初始化                           */
#define HAL_CAN_ERROR_NOT_READY       (0x00080000U)  /*!< 外设未就绪                                 */
#define HAL_CAN_ERROR_NOT_STARTED     (0x00100000U)  /*!< 外设未启动                               */
#define HAL_CAN_ERROR_PARAM           (0x00200000U)  /*!< 参数错误                                      */

#if USE_HAL_CAN_REGISTER_CALLBACKS == 1
#define HAL_CAN_ERROR_INVALID_CALLBACK (0x00400000U) /*!< 无效回调错误                               */
#endif /* USE_HAL_CAN_REGISTER_CALLBACKS */
#define HAL_CAN_ERROR_INTERNAL        (0x00800000U)  /*!< 内部错误                                       */

/**
  * @}
  */

/** @defgroup CAN_InitStatus CAN 初始化状态
  * @{
  */
#define CAN_INITSTATUS_FAILED       (0x00000000U)  /*!< CAN 初始化失败 */
#define CAN_INITSTATUS_SUCCESS      (0x00000001U)  /*!< CAN 初始化成功     */
/**
  * @}
  */

/** @defgroup CAN_operating_mode CAN 工作模式
  * @{
  */
#define CAN_MODE_NORMAL             (0x00000000U)                              /*!< 正常模式   */
#define CAN_MODE_LOOPBACK           ((uint32_t)CAN_BTR_LBKM)                   /*!< 回环模式 */
#define CAN_MODE_SILENT             ((uint32_t)CAN_BTR_SILM)                   /*!< 静默模式   */
#define CAN_MODE_SILENT_LOOPBACK    ((uint32_t)(CAN_BTR_LBKM | CAN_BTR_SILM))  /*!< 静默回环模式   */
/**
  * @}
  */


/** @defgroup CAN_synchronisation_jump_width CAN 同步跳转宽度
  * @{
  */
#define CAN_SJW_1TQ                 (0x00000000U)              /*!< 1 个时间量子 */
#define CAN_SJW_2TQ                 ((uint32_t)CAN_BTR_SJW_0)  /*!< 2 个时间量子 */
#define CAN_SJW_3TQ                 ((uint32_t)CAN_BTR_SJW_1)  /*!< 3 个时间量子 */
#define CAN_SJW_4TQ                 ((uint32_t)CAN_BTR_SJW)    /*!< 4 个时间量子 */
/**
  * @}
  */

/** @defgroup CAN_time_quantum_in_bit_segment_1 CAN 位段1中的时间量子
  * @{
  */
#define CAN_BS1_1TQ                 (0x00000000U)                                                /*!< 1 个时间量子  */
#define CAN_BS1_2TQ                 ((uint32_t)CAN_BTR_TS1_0)                                    /*!< 2 个时间量子  */
#define CAN_BS1_3TQ                 ((uint32_t)CAN_BTR_TS1_1)                                    /*!< 3 个时间量子  */
#define CAN_BS1_4TQ                 ((uint32_t)(CAN_BTR_TS1_1 | CAN_BTR_TS1_0))                  /*!< 4 个时间量子  */
#define CAN_BS1_5TQ                 ((uint32_t)CAN_BTR_TS1_2)                                    /*!< 5 个时间量子  */
#define CAN_BS1_6TQ                 ((uint32_t)(CAN_BTR_TS1_2 | CAN_BTR_TS1_0))                  /*!< 6 个时间量子  */
#define CAN_BS1_7TQ                 ((uint32_t)(CAN_BTR_TS1_2 | CAN_BTR_TS1_1))                  /*!< 7 个时间量子  */
#define CAN_BS1_8TQ                 ((uint32_t)(CAN_BTR_TS1_2 | CAN_BTR_TS1_1 | CAN_BTR_TS1_0))  /*!< 8 个时间量子  */
#define CAN_BS1_9TQ                 ((uint32_t)CAN_BTR_TS1_3)                                    /*!< 9 个时间量子  */
#define CAN_BS1_10TQ                ((uint32_t)(CAN_BTR_TS1_3 | CAN_BTR_TS1_0))                  /*!< 10 个时间量子 */
#define CAN_BS1_11TQ                ((uint32_t)(CAN_BTR_TS1_3 | CAN_BTR_TS1_1))                  /*!< 11 个时间量子 */
#define CAN_BS1_12TQ                ((uint32_t)(CAN_BTR_TS1_3 | CAN_BTR_TS1_1 | CAN_BTR_TS1_0))  /*!< 12 个时间量子 */
#define CAN_BS1_13TQ                ((uint32_t)(CAN_BTR_TS1_3 | CAN_BTR_TS1_2))                  /*!< 13 个时间量子 */
#define CAN_BS1_14TQ                ((uint32_t)(CAN_BTR_TS1_3 | CAN_BTR_TS1_2 | CAN_BTR_TS1_0))  /*!< 14 个时间量子 */
#define CAN_BS1_15TQ                ((uint32_t)(CAN_BTR_TS1_3 | CAN_BTR_TS1_2 | CAN_BTR_TS1_1))  /*!< 15 个时间量子 */
#define CAN_BS1_16TQ                ((uint32_t)CAN_BTR_TS1) /*!< 16 个时间量子 */
/**
  * @}
  */

/** @defgroup CAN_time_quantum_in_bit_segment_2 CAN 位段2中的时间量子
  * @{
  */
#define CAN_BS2_1TQ                 (0x00000000U)                                /*!< 1 个时间量子 */
#define CAN_BS2_2TQ                 ((uint32_t)CAN_BTR_TS2_0)                    /*!< 2 个时间量子 */
#define CAN_BS2_3TQ                 ((uint32_t)CAN_BTR_TS2_1)                    /*!< 3 个时间量子 */
#define CAN_BS2_4TQ                 ((uint32_t)(CAN_BTR_TS2_1 | CAN_BTR_TS2_0))  /*!< 4 个时间量子 */
#define CAN_BS2_5TQ                 ((uint32_t)CAN_BTR_TS2_2)                    /*!< 5 个时间量子 */
#define CAN_BS2_6TQ                 ((uint32_t)(CAN_BTR_TS2_2 | CAN_BTR_TS2_0))  /*!< 6 个时间量子 */
#define CAN_BS2_7TQ                 ((uint32_t)(CAN_BTR_TS2_2 | CAN_BTR_TS2_1))  /*!< 7 个时间量子 */
#define CAN_BS2_8TQ                 ((uint32_t)CAN_BTR_TS2)                      /*!< 8 个时间量子 */
/**
  * @}
  */

/** @defgroup CAN_filter_mode CAN 过滤器模式
  * @{
  */
#define CAN_FILTERMODE_IDMASK       (0x00000000U)  /*!< 标识符掩码模式 */
#define CAN_FILTERMODE_IDLIST       (0x00000001U)  /*!< 标识符列表模式 */
/**
  * @}
  */

/** @defgroup CAN_filter_scale CAN 过滤器比例
  * @{
  */
#define CAN_FILTERSCALE_16BIT       (0x00000000U)  /*!< 两个 16 位过滤器 */
#define CAN_FILTERSCALE_32BIT       (0x00000001U)  /*!< 一个 32 位过滤器  */
/**
  * @}
  */

/** @defgroup CAN_filter_activation CAN 过滤器激活
  * @{
  */
#define CAN_FILTER_DISABLE          (0x00000000U)  /*!< 禁用过滤器 */
#define CAN_FILTER_ENABLE           (0x00000001U)  /*!< 启用过滤器  */
/**
  * @}
  */

/** @defgroup CAN_filter_FIFO CAN 过滤器 FIFO
  * @{
  */
#define CAN_FILTER_FIFO0            (0x00000000U)  /*!< 过滤器 x 分配给 FIFO 0 */
#define CAN_FILTER_FIFO1            (0x00000001U)  /*!< 过滤器 x 分配给 FIFO 1 */
/**
  * @}
  */

/** @defgroup CAN_identifier_type CAN 标识符类型
  * @{
  */
#define CAN_ID_STD                  (0x00000000U)  /*!< 标准 ID */
#define CAN_ID_EXT                  (0x00000004U)  /*!< 扩展 ID */
/**
  * @}
  */

/** @defgroup CAN_remote_transmission_request CAN 远程传输请求
  * @{
  */
#define CAN_RTR_DATA                (0x00000000U)  /*!< 数据帧   */
#define CAN_RTR_REMOTE              (0x00000002U)  /*!< 远程帧 */
/**
  * @}
  */

/** @defgroup CAN_receive_FIFO_number CAN 接收 FIFO 编号
  * @{
  */
#define CAN_RX_FIFO0                (0x00000000U)  /*!< CAN 接收 FIFO 0 */
#define CAN_RX_FIFO1                (0x00000001U)  /*!< CAN 接收 FIFO 1 */
/**
  * @}
  */

/** @defgroup CAN_Tx_Mailboxes CAN 发送邮箱
  * @{
  */
#define CAN_TX_MAILBOX0             (0x00000001U)  /*!< 发送邮箱 0  */
#define CAN_TX_MAILBOX1             (0x00000002U)  /*!< 发送邮箱 1  */
#define CAN_TX_MAILBOX2             (0x00000004U)  /*!< 发送邮箱 2  */
/**
  * @}
  */

/** @defgroup CAN_flags CAN 标志
  * @{
  */
/* 发送标志 */
#define CAN_FLAG_RQCP0              (0x00000500U)  /*!< 请求完成 邮箱 0 标志   */
#define CAN_FLAG_TXOK0              (0x00000501U)  /*!< 传输成功 邮箱 0 标志    */
#define CAN_FLAG_ALST0              (0x00000502U)  /*!< 仲裁丢失 邮箱 0 标志   */
#define CAN_FLAG_TERR0              (0x00000503U)  /*!< 传输错误 邮箱 0 标志 */
#define CAN_FLAG_RQCP1              (0x00000508U)  /*!< 请求完成 邮箱 1 标志    */
#define CAN_FLAG_TXOK1              (0x00000509U)  /*!< 传输成功 邮箱 1 标志    */
#define CAN_FLAG_ALST1              (0x0000050AU)  /*!< 仲裁丢失 邮箱 1 标志   */
#define CAN_FLAG_TERR1              (0x0000050BU)  /*!< 传输错误 邮箱 1 标志 */
#define CAN_FLAG_RQCP2              (0x00000510U)  /*!< 请求完成 邮箱 2 标志    */
#define CAN_FLAG_TXOK2              (0x00000511U)  /*!< 传输成功 邮箱 2 标志    */
#define CAN_FLAG_ALST2              (0x00000512U)  /*!< 仲裁丢失 邮箱 2 标志   */
#define CAN_FLAG_TERR2              (0x00000513U)  /*!< 传输错误 邮箱 2 标志 */
#define CAN_FLAG_TME0               (0x0000051AU)  /*!< 发送邮箱 0 空标志     */
#define CAN_FLAG_TME1               (0x0000051BU)  /*!< 发送邮箱 1 空标志     */
#define CAN_FLAG_TME2               (0x0000051CU)  /*!< 发送邮箱 2 空标志     */
#define CAN_FLAG_LOW0               (0x0000051DU)  /*!< 最低优先级 邮箱 0 标志    */
#define CAN_FLAG_LOW1               (0x0000051EU)  /*!< 最低优先级 邮箱 1 标志    */
#define CAN_FLAG_LOW2               (0x0000051FU)  /*!< 最低优先级 邮箱 2 标志    */

/* 接收标志 */
#define CAN_FLAG_FF0                (0x00000203U)  /*!< 接收 FIFO 0 满标志               */
#define CAN_FLAG_FOV0               (0x00000204U)  /*!< 接收 FIFO 0 溢出标志            */
#define CAN_FLAG_FF1                (0x00000403U)  /*!< 接收 FIFO 1 满标志               */
#define CAN_FLAG_FOV1               (0x00000404U)  /*!< 接收 FIFO 1 溢出标志            */

/* 工作模式标志 */
#define CAN_FLAG_INAK               (0x00000100U)  /*!< 初始化确认标志   */
#define CAN_FLAG_SLAK               (0x00000101U)  /*!< 睡眠确认标志            */
#define CAN_FLAG_ERRI               (0x00000102U)  /*!< 错误标志                        */
#define CAN_FLAG_WKU                (0x00000103U)  /*!< 唤醒中断标志            */
#define CAN_FLAG_SLAKI              (0x00000104U)  /*!< 睡眠确认中断标志  */

/* 错误标志 */
#define CAN_FLAG_EWG                (0x00000300U)  /*!< 错误警告标志                */
#define CAN_FLAG_EPV                (0x00000301U)  /*!< 错误被动标志                */
#define CAN_FLAG_BOF                (0x00000302U)  /*!< 总线关闭标志                      */
/**
  * @}
  */


/** @defgroup CAN_Interrupts CAN 中断
  * @{
  */
/* 发送中断 */
#define CAN_IT_TX_MAILBOX_EMPTY     ((uint32_t)CAN_IER_TMEIE)   /*!< 发送邮箱空中断 */

/* 接收中断 */
#define CAN_IT_RX_FIFO0_MSG_PENDING ((uint32_t)CAN_IER_FMPIE0)  /*!< FIFO 0 消息挂起中断 */
#define CAN_IT_RX_FIFO0_FULL        ((uint32_t)CAN_IER_FFIE0)   /*!< FIFO 0 满中断            */
#define CAN_IT_RX_FIFO0_OVERRUN     ((uint32_t)CAN_IER_FOVIE0)  /*!< FIFO 0 溢出中断         */
#define CAN_IT_RX_FIFO1_MSG_PENDING ((uint32_t)CAN_IER_FMPIE1)  /*!< FIFO 1 消息挂起中断 */
#define CAN_IT_RX_FIFO1_FULL        ((uint32_t)CAN_IER_FFIE1)   /*!< FIFO 1 满中断            */
#define CAN_IT_RX_FIFO1_OVERRUN     ((uint32_t)CAN_IER_FOVIE1)  /*!< FIFO 1 溢出中断         */

/* 工作模式中断 */
#define CAN_IT_WAKEUP               ((uint32_t)CAN_IER_WKUIE)   /*!< 唤醒中断                */
#define CAN_IT_SLEEP_ACK            ((uint32_t)CAN_IER_SLKIE)   /*!< 睡眠确认中断      */

/* 错误中断 */
#define CAN_IT_ERROR_WARNING        ((uint32_t)CAN_IER_EWGIE)   /*!< 错误警告中断          */
#define CAN_IT_ERROR_PASSIVE        ((uint32_t)CAN_IER_EPVIE)   /*!< 错误被动中断          */
#define CAN_IT_BUSOFF               ((uint32_t)CAN_IER_BOFIE)   /*!< 总线关闭中断                */
#define CAN_IT_LAST_ERROR_CODE      ((uint32_t)CAN_IER_LECIE)   /*!< 最后错误码中断                */
#define CAN_IT_ERROR                ((uint32_t)CAN_IER_ERRIE)   /*!< 错误中断                  */
/**
  * @}
  */

/**
  * @}
  */

/* 导出宏 -----------------------------------------------------------*/
/** @defgroup CAN_Exported_Macros CAN 导出宏
  * @{
  */

/** @brief  重置 CAN 句柄状态
  * @param  __HANDLE__ CAN 句柄
  * @retval 无
  */
#if USE_HAL_CAN_REGISTER_CALLBACKS == 1
#define __HAL_CAN_RESET_HANDLE_STATE(__HANDLE__) do{                                              \
                                                     (__HANDLE__)->State = HAL_CAN_STATE_RESET;   \
                                                     (__HANDLE__)->MspInitCallback = NULL;        \
                                                     (__HANDLE__)->MspDeInitCallback = NULL;      \
                                                   } while(0)
#else
#define __HAL_CAN_RESET_HANDLE_STATE(__HANDLE__) ((__HANDLE__)->State = HAL_CAN_STATE_RESET)
#endif /*USE_HAL_CAN_REGISTER_CALLBACKS */

/**
  * @brief  启用指定的 CAN 中断
  * @param  __HANDLE__ CAN 句柄
  * @param  __INTERRUPT__ 要启用的 CAN 中断源
  *           此参数可以是 @arg CAN_Interrupts 的任意组合
  * @retval 无
  */
#define __HAL_CAN_ENABLE_IT(__HANDLE__, __INTERRUPT__) (((__HANDLE__)->Instance->IER) |= (__INTERRUPT__))

/**
  * @brief  禁用指定的 CAN 中断
  * @param  __HANDLE__ CAN 句柄
  * @param  __INTERRUPT__ 要禁用的 CAN 中断源
  *           此参数可以是 @arg CAN_Interrupts 的任意组合
  * @retval 无
  */
#define __HAL_CAN_DISABLE_IT(__HANDLE__, __INTERRUPT__) (((__HANDLE__)->Instance->IER) &= ~(__INTERRUPT__))

/** @brief  检查指定的 CAN 中断源是启用还是禁用
  * @param  __HANDLE__ 指定 CAN 句柄
  * @param  __INTERRUPT__ 指定要检查的 CAN 中断源
  *           此参数可以是 @arg CAN_Interrupts 的值
  * @retval __IT__ 的状态 (TRUE 或 FALSE)
  */
#define __HAL_CAN_GET_IT_SOURCE(__HANDLE__, __INTERRUPT__) (((__HANDLE__)->Instance->IER) & (__INTERRUPT__))

/** @brief  检查指定的 CAN 标志是否置位
  * @param  __HANDLE__ 指定 CAN 句柄
  * @param  __FLAG__ 要检查的标志
  *         此参数可以是 @arg CAN_flags 的值
  * @retval __FLAG__ 的状态 (TRUE 或 FALSE)
  */
#define __HAL_CAN_GET_FLAG(__HANDLE__, __FLAG__) \
  ((((__FLAG__) >> 8U) == 5U)? ((((__HANDLE__)->Instance->TSR) & (1U << ((__FLAG__) & CAN_FLAG_MASK))) == (1U << ((__FLAG__) & CAN_FLAG_MASK))): \
   (((__FLAG__) >> 8U) == 2U)? ((((__HANDLE__)->Instance->RF0R) & (1U << ((__FLAG__) & CAN_FLAG_MASK))) == (1U << ((__FLAG__) & CAN_FLAG_MASK))): \
   (((__FLAG__) >> 8U) == 4U)? ((((__HANDLE__)->Instance->RF1R) & (1U << ((__FLAG__) & CAN_FLAG_MASK))) == (1U << ((__FLAG__) & CAN_FLAG_MASK))): \
   (((__FLAG__) >> 8U) == 1U)? ((((__HANDLE__)->Instance->MSR) & (1U << ((__FLAG__) & CAN_FLAG_MASK))) == (1U << ((__FLAG__) & CAN_FLAG_MASK))): \
   (((__FLAG__) >> 8U) == 3U)? ((((__HANDLE__)->Instance->ESR) & (1U << ((__FLAG__) & CAN_FLAG_MASK))) == (1U << ((__FLAG__) & CAN_FLAG_MASK))): 0U)

/** @brief  清除指定的 CAN 挂起标志
  * @param  __HANDLE__ 指定 CAN 句柄
  * @param  __FLAG__ 要清除的标志
  *         此参数可以是以下值之一:
  *            @arg CAN_FLAG_RQCP0: 请求完成 邮箱 0 标志
  *            @arg CAN_FLAG_TXOK0: 传输成功 邮箱 0 标志
  *            @arg CAN_FLAG_ALST0: 仲裁丢失 邮箱 0 标志
  *            @arg CAN_FLAG_TERR0: 传输错误 邮箱 0 标志
  *            @arg CAN_FLAG_RQCP1: 请求完成 邮箱 1 标志
  *            @arg CAN_FLAG_TXOK1: 传输成功 邮箱 1 标志
  *            @arg CAN_FLAG_ALST1: 仲裁丢失 邮箱 1 标志
  *            @arg CAN_FLAG_TERR1: 传输错误 邮箱 1 标志
  *            @arg CAN_FLAG_RQCP2: 请求完成 邮箱 2 标志
  *            @arg CAN_FLAG_TXOK2: 传输成功 邮箱 2 标志
  *            @arg CAN_FLAG_ALST2: 仲裁丢失 邮箱 2 标志
  *            @arg CAN_FLAG_TERR2: 传输错误 邮箱 2 标志
  *            @arg CAN_FLAG_FF0:   接收 FIFO 0 满标志
  *            @arg CAN_FLAG_FOV0:  接收 FIFO 0 溢出标志
  *            @arg CAN_FLAG_FF1:   接收 FIFO 1 满标志
  *            @arg CAN_FLAG_FOV1:  接收 FIFO 1 溢出标志
  *            @arg CAN_FLAG_WKUI:  唤醒中断标志
  *            @arg CAN_FLAG_SLAKI: 睡眠确认中断标志
  * @retval 无
  */
#define __HAL_CAN_CLEAR_FLAG(__HANDLE__, __FLAG__) \
  ((((__FLAG__) >> 8U) == 5U)? (((__HANDLE__)->Instance->TSR) = (1U << ((__FLAG__) & CAN_FLAG_MASK))): \
   (((__FLAG__) >> 8U) == 2U)? (((__HANDLE__)->Instance->RF0R) = (1U << ((__FLAG__) & CAN_FLAG_MASK))): \
   (((__FLAG__) >> 8U) == 4U)? (((__HANDLE__)->Instance->RF1R) = (1U << ((__FLAG__) & CAN_FLAG_MASK))): \
   (((__FLAG__) >> 8U) == 1U)? (((__HANDLE__)->Instance->MSR) = (1U << ((__FLAG__) & CAN_FLAG_MASK))): 0U)

/**
 * @}
 */

/* 导出函数 --------------------------------------------------------*/
/** @addtogroup CAN_Exported_Functions CAN 导出函数
  * @{
  */

/** @addtogroup CAN_Exported_Functions_Group1 初始化和反初始化函数
 *  @brief    初始化和配置函数
 * @{
  */

/* 初始化和反初始化函数 *****************************/
HAL_StatusTypeDef HAL_CAN_Init(CAN_HandleTypeDef *hcan);
HAL_StatusTypeDef HAL_CAN_DeInit(CAN_HandleTypeDef *hcan);
void HAL_CAN_MspInit(CAN_HandleTypeDef *hcan);
void HAL_CAN_MspDeInit(CAN_HandleTypeDef *hcan);

#if USE_HAL_CAN_REGISTER_CALLBACKS == 1
/* 回调注册/注销函数  ***********************************/
HAL_StatusTypeDef HAL_CAN_RegisterCallback(CAN_HandleTypeDef *hcan, HAL_CAN_CallbackIDTypeDef CallbackID,
                                           void (* pCallback)(CAN_HandleTypeDef *_hcan));
HAL_StatusTypeDef HAL_CAN_UnRegisterCallback(CAN_HandleTypeDef *hcan, HAL_CAN_CallbackIDTypeDef CallbackID);

#endif /* (USE_HAL_CAN_REGISTER_CALLBACKS) */
/**
 * @}
 */

/** @addtogroup CAN_Exported_Functions_Group2 配置函数
 *  @brief    配置函数
 * @{
  */

/* 配置函数 ****************************************************/
HAL_StatusTypeDef HAL_CAN_ConfigFilter(CAN_HandleTypeDef *hcan, const CAN_FilterTypeDef *sFilterConfig);

/**
 * @}
 */

/** @addtogroup CAN_Exported_Functions_Group3 控制函数
 *  @brief    控制函数
 * @{
  */

/* 控制函数 **********************************************************/
HAL_StatusTypeDef HAL_CAN_Start(CAN_HandleTypeDef *hcan);                                    /*!< 启动 CAN */
HAL_StatusTypeDef HAL_CAN_Stop(CAN_HandleTypeDef *hcan);                                     /*!< 停止 CAN */
HAL_StatusTypeDef HAL_CAN_RequestSleep(CAN_HandleTypeDef *hcan);                             /*!< 请求进入睡眠 */
HAL_StatusTypeDef HAL_CAN_WakeUp(CAN_HandleTypeDef *hcan);                                  /*!< 唤醒 CAN */
uint32_t HAL_CAN_IsSleepActive(const CAN_HandleTypeDef *hcan);                             /*!< 检查睡眠状态 */
HAL_StatusTypeDef HAL_CAN_AddTxMessage(CAN_HandleTypeDef *hcan, const CAN_TxHeaderTypeDef *pHeader,
                                       const uint8_t aData[], uint32_t *pTxMailbox);        /*!< 添加发送消息 */
HAL_StatusTypeDef HAL_CAN_AbortTxRequest(CAN_HandleTypeDef *hcan, uint32_t TxMailboxes);    /*!< 中止发送请求 */
uint32_t HAL_CAN_GetTxMailboxesFreeLevel(const CAN_HandleTypeDef *hcan);                   /*!< 获取空闲发送邮箱数 */
uint32_t HAL_CAN_IsTxMessagePending(const CAN_HandleTypeDef *hcan, uint32_t TxMailboxes);  /*!< 检查消息是否挂起 */
uint32_t HAL_CAN_GetTxTimestamp(const CAN_HandleTypeDef *hcan, uint32_t TxMailbox);        /*!< 获取发送时间戳 */
HAL_StatusTypeDef HAL_CAN_GetRxMessage(CAN_HandleTypeDef *hcan, uint32_t RxFifo,
                                       CAN_RxHeaderTypeDef *pHeader, uint8_t aData[]);      /*!< 获取接收消息 */
uint32_t HAL_CAN_GetRxFifoFillLevel(const CAN_HandleTypeDef *hcan, uint32_t RxFifo);       /*!< 获取 FIFO 填充级别 */

/**
 * @}
 */

/** @addtogroup CAN_Exported_Functions_Group4 中断管理
 *  @brief    中断管理
 * @{
  */
/* 中断管理 ******************************************************/
HAL_StatusTypeDef HAL_CAN_ActivateNotification(CAN_HandleTypeDef *hcan, uint32_t ActiveITs);     /*!< 激活中断通知 */
HAL_StatusTypeDef HAL_CAN_DeactivateNotification(CAN_HandleTypeDef *hcan, uint32_t InactiveITs); /*!< 禁用中断通知 */
void HAL_CAN_IRQHandler(CAN_HandleTypeDef *hcan);                                                   /*!< CAN 中断处理函数 */

/**
 * @}
 */

/** @addtogroup CAN_Exported_Functions_Group5 回调函数
 *  @brief    回调函数
 * @{
  */
/* 回调函数 ********************************************************/

void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan);    /*!< 发送邮箱 0 完成回调 */
void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef *hcan);    /*!< 发送邮箱 1 完成回调 */
void HAL_CAN_TxMailbox2CompleteCallback(CAN_HandleTypeDef *hcan);    /*!< 发送邮箱 2 完成回调 */
void HAL_CAN_TxMailbox0AbortCallback(CAN_HandleTypeDef *hcan);       /*!< 发送邮箱 0 中止回调 */
void HAL_CAN_TxMailbox1AbortCallback(CAN_HandleTypeDef *hcan);       /*!< 发送邮箱 1 中止回调 */
void HAL_CAN_TxMailbox2AbortCallback(CAN_HandleTypeDef *hcan);       /*!< 发送邮箱 2 中止回调 */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan);    /*!< 接收 FIFO 0 消息挂起回调 */
void HAL_CAN_RxFifo0FullCallback(CAN_HandleTypeDef *hcan);           /*!< 接收 FIFO 0 满回调 */
void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan);    /*!< 接收 FIFO 1 消息挂起回调 */
void HAL_CAN_RxFifo1FullCallback(CAN_HandleTypeDef *hcan);           /*!< 接收 FIFO 1 满回调 */
void HAL_CAN_SleepCallback(CAN_HandleTypeDef *hcan);                /*!< 睡眠回调 */
void HAL_CAN_WakeUpFromRxMsgCallback(CAN_HandleTypeDef *hcan);       /*!< 从接收消息唤醒回调 */
void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan);               /*!< 错误回调 */

/**
 * @}
 */

/** @addtogroup CAN_Exported_Functions_Group6 外设状态和错误函数
 *  @brief   CAN 外设状态函数
 * @{
  */
/* 外设状态和错误函数 ***************************************/
HAL_CAN_StateTypeDef HAL_CAN_GetState(const CAN_HandleTypeDef *hcan);    /*!< 获取 CAN 状态 */
uint32_t HAL_CAN_GetError(const CAN_HandleTypeDef *hcan);               /*!< 获取错误码 */
HAL_StatusTypeDef HAL_CAN_ResetError(CAN_HandleTypeDef *hcan);         /*!< 重置错误 */

/**
 * @}
 */

/**
 * @}
 */

/* 私有类型 -------------------------------------------------------------*/
/** @defgroup CAN_Private_Types CAN 私有类型
  * @{
  */

/**
  * @}
  */

/* 私有变量 ---------------------------------------------------------*/
/** @defgroup CAN_Private_Variables CAN 私有变量
  * @{
  */

/**
  * @}
  */

/* 私有常量 ---------------------------------------------------------*/
/** @defgroup CAN_Private_Constants CAN 私有常量
  * @{
  */
#define CAN_FLAG_MASK  (0x000000FFU)  /*!< CAN 标志掩码 */
/**
  * @}
  */

/* 私有宏 -----------------------------------------------------------*/
/** @defgroup CAN_Private_Macros CAN 私有宏（参数验证）
  * @{
  */

/*!< CAN 模式验证：检测是否为有效的工作模式 */
#define IS_CAN_MODE(MODE) (((MODE) == CAN_MODE_NORMAL) || \
                           ((MODE) == CAN_MODE_LOOPBACK)|| \
                           ((MODE) == CAN_MODE_SILENT) || \
                           ((MODE) == CAN_MODE_SILENT_LOOPBACK))
/*!< CAN 同步跳转宽度验证：检测 SJW 值是否有效（1-4个时间量子） */
#define IS_CAN_SJW(SJW) (((SJW) == CAN_SJW_1TQ) || ((SJW) == CAN_SJW_2TQ) || \
                         ((SJW) == CAN_SJW_3TQ) || ((SJW) == CAN_SJW_4TQ))
/*!< CAN 位段1时间量子验证：检测 BS1 值是否有效（1-16个时间量子） */
#define IS_CAN_BS1(BS1) (((BS1) == CAN_BS1_1TQ) || ((BS1) == CAN_BS1_2TQ) || \
                         ((BS1) == CAN_BS1_3TQ) || ((BS1) == CAN_BS1_4TQ) || \
                         ((BS1) == CAN_BS1_5TQ) || ((BS1) == CAN_BS1_6TQ) || \
                         ((BS1) == CAN_BS1_7TQ) || ((BS1) == CAN_BS1_8TQ) || \
                         ((BS1) == CAN_BS1_9TQ) || ((BS1) == CAN_BS1_10TQ)|| \
                         ((BS1) == CAN_BS1_11TQ)|| ((BS1) == CAN_BS1_12TQ)|| \
                         ((BS1) == CAN_BS1_13TQ)|| ((BS1) == CAN_BS1_14TQ)|| \
                         ((BS1) == CAN_BS1_15TQ)|| ((BS1) == CAN_BS1_16TQ))
/*!< CAN 位段2时间量子验证：检测 BS2 值是否有效（1-8个时间量子） */
#define IS_CAN_BS2(BS2) (((BS2) == CAN_BS2_1TQ) || ((BS2) == CAN_BS2_2TQ) || \
                         ((BS2) == CAN_BS2_3TQ) || ((BS2) == CAN_BS2_4TQ) || \
                         ((BS2) == CAN_BS2_5TQ) || ((BS2) == CAN_BS2_6TQ) || \
                         ((BS2) == CAN_BS2_7TQ) || ((BS2) == CAN_BS2_8TQ))
/*!< CAN 预分频器验证：检测预分频值是否有效（1-1024） */
#define IS_CAN_PRESCALER(PRESCALER) (((PRESCALER) >= 1U) && ((PRESCALER) <= 1024U))
/*!< CAN 过滤器 ID 半字验证：检测过滤器 ID 值是否有效（0-0xFFFF） */
#define IS_CAN_FILTER_ID_HALFWORD(HALFWORD) ((HALFWORD) <= 0xFFFFU)
#if   defined(CAN2)
/*!< 双 CAN 实例过滤器组验证：检测过滤器组号是否有效（0-27） */
#define IS_CAN_FILTER_BANK_DUAL(BANK) ((BANK) <= 27U)
#endif
/*!< 单 CAN 实例过滤器组验证：检测过滤器组号是否有效（0-13） */
#define IS_CAN_FILTER_BANK_SINGLE(BANK) ((BANK) <= 13U)
/*!< CAN 过滤器模式验证：检测是否为掩码模式或列表模式 */
#define IS_CAN_FILTER_MODE(MODE) (((MODE) == CAN_FILTERMODE_IDMASK) || \
                                  ((MODE) == CAN_FILTERMODE_IDLIST))
/*!< CAN 过滤器比例验证：检测是否为 16 位或 32 位比例 */
#define IS_CAN_FILTER_SCALE(SCALE) (((SCALE) == CAN_FILTERSCALE_16BIT) || \
                                    ((SCALE) == CAN_FILTERSCALE_32BIT))
/*!< CAN 过滤器激活验证：检测启用或禁用状态 */
#define IS_CAN_FILTER_ACTIVATION(ACTIVATION) (((ACTIVATION) == CAN_FILTER_DISABLE) || \
                                              ((ACTIVATION) == CAN_FILTER_ENABLE))
/*!< CAN 过滤器 FIFO 验证：检测 FIFO 分配（0 或 1） */
#define IS_CAN_FILTER_FIFO(FIFO) (((FIFO) == CAN_FILTER_FIFO0) || \
                                  ((FIFO) == CAN_FILTER_FIFO1))
/*!< CAN 发送邮箱验证：检测邮箱号是否有效（0-2） */
#define IS_CAN_TX_MAILBOX(TRANSMITMAILBOX) (((TRANSMITMAILBOX) == CAN_TX_MAILBOX0 ) || \
                                            ((TRANSMITMAILBOX) == CAN_TX_MAILBOX1 ) || \
                                            ((TRANSMITMAILBOX) == CAN_TX_MAILBOX2 ))
/*!< CAN 发送邮箱列表验证：检测邮箱位掩码是否有效 */
#define IS_CAN_TX_MAILBOX_LIST(TRANSMITMAILBOX) ((TRANSMITMAILBOX) <= (CAN_TX_MAILBOX0 | CAN_TX_MAILBOX1 | \
                                                                       CAN_TX_MAILBOX2))
/*!< CAN 标准 ID 验证：检测标准标识符是否有效（0-0x7FF） */
#define IS_CAN_STDID(STDID)   ((STDID) <= 0x7FFU)
/*!< CAN 扩展 ID 验证：检测扩展标识符是否有效（0-0x1FFFFFFF） */
#define IS_CAN_EXTID(EXTID)   ((EXTID) <= 0x1FFFFFFFU)
/*!< CAN DLC 验证：检测数据长度码是否有效（0-8） */
#define IS_CAN_DLC(DLC)       ((DLC) <= 8U)
/*!< CAN 标识符类型验证：检测标准帧或扩展帧 */
#define IS_CAN_IDTYPE(IDTYPE)  (((IDTYPE) == CAN_ID_STD) || \
                                ((IDTYPE) == CAN_ID_EXT))
/*!< CAN 远程帧验证：检测数据帧或远程帧 */
#define IS_CAN_RTR(RTR) (((RTR) == CAN_RTR_DATA) || ((RTR) == CAN_RTR_REMOTE))
/*!< CAN 接收 FIFO 验证：检测 FIFO 号（0 或 1） */
#define IS_CAN_RX_FIFO(FIFO) (((FIFO) == CAN_RX_FIFO0) || ((FIFO) == CAN_RX_FIFO1))
/*!< CAN 中断验证：检测中断源是否有效 */
#define IS_CAN_IT(IT) ((IT) <= (CAN_IT_TX_MAILBOX_EMPTY     | CAN_IT_RX_FIFO0_MSG_PENDING      | \
                                CAN_IT_RX_FIFO0_FULL        | CAN_IT_RX_FIFO0_OVERRUN          | \
                                CAN_IT_RX_FIFO1_MSG_PENDING | CAN_IT_RX_FIFO1_FULL             | \
                                CAN_IT_RX_FIFO1_OVERRUN     | CAN_IT_WAKEUP                    | \
                                CAN_IT_SLEEP_ACK            | CAN_IT_ERROR_WARNING             | \
                                CAN_IT_ERROR_PASSIVE        | CAN_IT_BUSOFF                    | \
                                CAN_IT_LAST_ERROR_CODE      | CAN_IT_ERROR))

/**
  * @}
  */
/* 私有宏结束 -----------------------------------------------------*/

/**
  * @}
  */


#endif /* CAN1 */
/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* STM32F1xx_HAL_CAN_H */
