> 说明：将 `printf` 重定向到 `USART1` 的实现代码。  
> 需放入 `main.c` 的 `USER CODE BEGIN 4` / `USER CODE END 4` 之间，  
> 之后即可调用 `printf("Hello STM32 串口打印测试\r\n");` 通过串口 1 输出。

```c
/* USER CODE BEGIN 4 */
/* 重定向函数（放在 main.c 任意全局区域） */
// 使用 printf("Hello STM32 串口打印测试\r\n");
// 重定向printf输出到串口huart1
int fputc(int ch, FILE *f)
{
HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
return ch;
}
```