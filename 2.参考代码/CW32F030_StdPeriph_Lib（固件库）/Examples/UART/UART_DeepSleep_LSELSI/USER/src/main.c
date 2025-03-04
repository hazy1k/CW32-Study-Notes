/******************************************************************************/
/** \file main.c
 **
 ** A detailed description is available at
 ** @link Sample Group Some description @endlink
 **
 **   - 2021-03-12  1.0  xiebin First version for Device Driver Library of Module.
 **
 ******************************************************************************/
/*******************************************************************************
*
* 代码许可和免责信息
* 武汉力源半导体有限公司授予您使用所有编程代码示例的非专属的版权许可，您可以由此
* 生成根据您的特定需要而定制的相似功能。根据不能被排除的任何法定保证，武汉力源半
* 导体有限公司及其程序开发商和供应商对程序或技术支持（如果有）不提供任何明示或暗
* 含的保证或条件，包括但不限于暗含的有关适销性、适用于某种特定用途和非侵权的保证
* 或条件。
* 无论何种情形，武汉力源半导体有限公司及其程序开发商或供应商均不对下列各项负责，
* 即使被告知其发生的可能性时，也是如此：数据的丢失或损坏；直接的、特别的、附带的
* 或间接的损害，或任何后果性经济损害；或利润、业务、收入、商誉或预期可节省金额的
* 损失。
* 某些司法辖区不允许对直接的、附带的或后果性的损害有任何的排除或限制，因此某些或
* 全部上述排除或限制可能并不适用于您。
*
*******************************************************************************/
/******************************************************************************
 * Include files
 ******************************************************************************/
#include "main.h"
/******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
//HCLK for SysTick
#define  HCLKFREQ                       8000000
//UARTx
#define  DEBUG_USARTx                   CW_UART1
#define  DEBUG_USART_CLK                RCC_APB2_PERIPH_UART1
#define  DEBUG_USART_APBClkENx          RCC_APBPeriphClk_Enable2
#define  DEBUG_USART_BaudRate           9600
#define  DEBUG_USART_UclkFreq           32800

//UARTx GPIO
#define  DEBUG_USART_GPIO_CLK           RCC_AHB_PERIPH_GPIOA
#define  DEBUG_USART_TX_GPIO_PORT       CW_GPIOA   
#define  DEBUG_USART_TX_GPIO_PIN        GPIO_PIN_8
#define  DEBUG_USART_RX_GPIO_PORT       CW_GPIOA
#define  DEBUG_USART_RX_GPIO_PIN        GPIO_PIN_9

//GPIO AF
#define  DEBUG_USART_AFTX               PA08_AFx_UART1TXD()
#define  DEBUG_USART_AFRX               PA09_AFx_UART1RXD()

//中断
#define  DEBUG_USART_IRQ                UART1_IRQn

/******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/

/******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/

/******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/
void RCC_Configuration(void);
void GPIO_Configuration(void);
void UART_Configuration(void);
void PWR_Configuration(void);
void NVIC_Configuration(void);
void USART_SendString(UART_TypeDef* USARTx, char *String);
void USART_SendBuf_Polling(UART_TypeDef* USARTx, uint8_t *TxBuf, uint8_t TxCnt);
uint8_t USART_RecvBuf_Polling(UART_TypeDef* USARTx, uint8_t *RxBuf);
/******************************************************************************
 * Local variable definitions ('static')                                      *
 ******************************************************************************/
uint8_t  TxRxBufferSize;
uint8_t  TxRxBuffer[300];
/******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/

/*****************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/

/**
 ******************************************************************************
 ** \brief  Main function of project
 **
 ** \return uint32_t return value, if needed
 **
 ******************************************************************************/
int32_t main(void)
{   
  //配置RCC
  RCC_Configuration();

  //配置GPIO
  GPIO_Configuration();

  //配置UART
  UART_Configuration();

  //配置低功耗模式
  PWR_Configuration();

  //配置NVIC
  NVIC_Configuration();

  //初始化SysTick
  InitTick(HCLKFREQ); 

  //DeepSleep唤醒时，保持原系统时钟来源
  RCC_WAKEUPCLK_Config(RCC_SYSCTRL_WAKEUPCLKDIS);

  USART_SendString(DEBUG_USARTx, "\r\nCW32F030 UART DeepSleep mode LSE/LSI\r\n");
  
  while(1)
  { 
    //进入深度休眠模式
    USART_SendString(DEBUG_USARTx, "\r\nEnter DeepSleep mode\r\n");
    USART_SendString(DEBUG_USARTx, "\r\nPC send data to wake up MCU\r\n");

    //使能UARTx RC中断
    USART_ITConfig(DEBUG_USARTx, USART_IT_RC, ENABLE); 
    PWR_GotoLpmMode();
    //失能UARTx RC中断
    USART_ITConfig(CW_UART1, USART_IT_RC, DISABLE); 

    //唤醒后轮询收发
    TxRxBufferSize = USART_RecvBuf_Polling(DEBUG_USARTx, TxRxBuffer);
    USART_SendBuf_Polling(DEBUG_USARTx, TxRxBuffer, TxRxBufferSize);
    
    //闪灯
    for(int i = 0; i<10; i++)
    {
      PB09_TOG();
      SysTickDelay(500); 
    }
  }
}

/**
 * @brief 外设时钟使能
 * 
 */
void RCC_Configuration(void)
{
  //复位后延时
  InitTick(8000000);
  SysTickDelay(1000); 

  //SYSCLK = HSI = 8MHz = HCLK = PCLK
  RCC_HSI_Enable(RCC_HSIOSC_DIV6);
  RCC_LSI_Enable();

  //外设时钟使能
  RCC_AHBPeriphClk_Enable(DEBUG_USART_GPIO_CLK | RCC_AHB_PERIPH_GPIOB, ENABLE);
  DEBUG_USART_APBClkENx(DEBUG_USART_CLK, ENABLE);
}

/**
 * @brief 配置GPIO
 * 
 */
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  //UART TX RX 复用 
  DEBUG_USART_AFTX;                     
  DEBUG_USART_AFRX;                     

  GPIO_InitStructure.Pins = DEBUG_USART_TX_GPIO_PIN;
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
  GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStructure);
    
  GPIO_InitStructure.Pins = DEBUG_USART_RX_GPIO_PIN;
  GPIO_InitStructure.Mode = GPIO_MODE_INPUT_PULLUP;
  GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStructure);

  //PB9 LED1
  GPIO_InitStructure.Pins = GPIO_PIN_9;
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_Init(CW_GPIOB, &GPIO_InitStructure);

  PB09_SETLOW();
}

/**
 * @brief 配置UART
 * 
 */
void UART_Configuration(void)
{
  USART_InitTypeDef USART_InitStructure;

  USART_InitStructure.USART_BaudRate = DEBUG_USART_BaudRate;
  USART_InitStructure.USART_Over = USART_Over_sp;            //专用采样
  USART_InitStructure.USART_Source = USART_Source_LSI;       //传输时钟 UCLK = LSI
  USART_InitStructure.USART_UclkFreq = DEBUG_USART_UclkFreq;
  USART_InitStructure.USART_StartBit = USART_StartBit_FE;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No ;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(DEBUG_USARTx, &USART_InitStructure); 	
}

/**
 * @brief 配置低功耗模式
 * 
 */
void PWR_Configuration(void)
{
  PWR_InitTypeDef PWR_InitStructure;

  PWR_InitStructure.PWR_Sevonpend = PWR_Sevonpend_Disable;
  PWR_InitStructure.PWR_SleepDeep = PWR_SleepDeep_Enable; //Deep Sleep
  PWR_InitStructure.PWR_SleepOnExit = PWR_SleepOnExit_Disable;
  PWR_Config(&PWR_InitStructure);
}

/**
 * @brief 配置NVIC
 * 
 */
void NVIC_Configuration(void)
{
  //优先级，无优先级分组
  NVIC_SetPriority(DEBUG_USART_IRQ, 0);
  //UARTx中断使能
  NVIC_EnableIRQ(DEBUG_USART_IRQ);
}

/**
 * @brief 发送字符串
 * 
 * @param USARTx :USARTx外设
 *        参数可以是:
 *           CW_UART1、CW_UART2、CW_UART3
 * @param String :待发送的字符串
 */
void USART_SendString(UART_TypeDef* USARTx, char *String)
{
  while(*String != '\0')
  {
    USART_SendData_8bit(USARTx, *String);
    while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
    String++;
  }
  while(USART_GetFlagStatus(USARTx, USART_FLAG_TXBUSY) == SET);
}

/**
 * @brief 发送8位数组
 * 
 * @param USARTx :USARTx外设
 *        参数可以是:
 *           CW_UART1、CW_UART2、CW_UART3
 * @param TxBuf :待发送的数组
 * @param TxCnt :待发送的数组元素个数
 */
void USART_SendBuf_Polling(UART_TypeDef* USARTx, uint8_t *TxBuf, uint8_t TxCnt)
{
  while(TxCnt)
  {
    USART_SendData_8bit(USARTx, *TxBuf);
    while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
    TxBuf++;
    TxCnt--;
  }	
  while(USART_GetFlagStatus(USARTx, USART_FLAG_TXBUSY) == SET);
}

/**
 * @brief 接收8位数组
 * 
 * @param USARTx :USARTx外设
 *        参数可以是:
 *           CW_UART1、CW_UART2、CW_UART3
 * @param RxBuf :接收Buf
 * @return uint8_t :接收的字符个数
 */
uint8_t USART_RecvBuf_Polling(UART_TypeDef* USARTx, uint8_t *RxBuf)
{
  uint8_t RxCnt = 0;
  RxBuf[RxCnt] = USART_ReceiveData_8bit(USARTx);
  RxCnt++;
  do
  {
    //等待RC
    while(USART_GetFlagStatus(USARTx, USART_FLAG_RC) == RESET);
    //清RC 
    USART_ClearFlag(USARTx, USART_FLAG_RC); 
    //ERROR: PE or FE
    if(USART_GetFlagStatus(USARTx, USART_FLAG_PE|USART_FLAG_FE))
    {
      USART_ClearFlag(USARTx, USART_FLAG_PE|USART_FLAG_FE);
      RxCnt = 0x00;
    }
    else
    {
      RxBuf[RxCnt] = USART_ReceiveData_8bit(USARTx);
      RxCnt++;            
    }
  }while(RxBuf[RxCnt-1] != '\n');
  return RxCnt; 
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

