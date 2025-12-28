#include "Lime_USB_Uart.h"
#include "usart.h"
#include "usbd_cdc_if.h"
#include <stdio.h>

#define DEBUG_PRINTF      1

#if DEBUG_PRINTF
#define DEBUG_LOG(...)	printf(__VA_ARGS__)
#else
#define DEBUG_LOG(...)
#endif

USB_Uart_t USB_Uart = {NoConnect,NoConnect,0};



  /*******************************************************************************/
  /* Line Coding Structure                                                       */
  /*-----------------------------------------------------------------------------*/
  /* Offset | Field       | Size | Value  | Description                          */
  /* 0      | dwDTERate   |   4  | Number |Data terminal rate, in bits per second*/
  /* 4      | bCharFormat |   1  | Number | Stop bits                            */
  /*                                        0 - 1 Stop bit                       */
  /*                                        1 - 1.5 Stop bits                    */
  /*                                        2 - 2 Stop bits                      */
  /* 5      | bParityType |  1   | Number | Parity                               */
  /*                                        0 - None                             */
  /*                                        1 - Odd                              */
  /*                                        2 - Even                             */
  /*                                        3 - Mark                             */
  /*                                        4 - Space                            */
  /* 6      | bDataBits  |   1   | Number Data bits (5, 6, 7, 8 or 16).          */
  /*******************************************************************************/
uint8_t uart3TempRecvData = 0;
void Lime_USB_Uart_ResetVal(uint8_t* pbuf)
{
	/* Line State */
	uint8_t control_line_state = pbuf[0];
	
	/* DTR */
	if(control_line_state & 0x01)
		DEBUG_LOG("DTR 1\n");
	else
		DEBUG_LOG("DTR 0\n");
	
	/* RTS */
	if(control_line_state & 0x02)
		DEBUG_LOG("RTS 1\n");
	else
		DEBUG_LOG("RTS 0\n");
	
	/* debug test */
	uint32_t *debugBuf = (uint32_t*)pbuf;
	DEBUG_LOG("debug cat:0x%08x,0x%08x,0x%08x,0x%08x", debugBuf[0], debugBuf[1], debugBuf[2], debugBuf[3]);
	
//  static uint32_t LastInTime = 0;
  USB_Uart.BaudRate = (pbuf[0] | (pbuf[1] << 8)| (pbuf[2] << 16)| (pbuf[3] << 24));
  USB_Uart.WordLength = pbuf[6];
  USB_Uart.StopBits = pbuf[4];
  USB_Uart.Parity = pbuf[5];
  
//  if(HAL_GetTick() - LastInTime < 10)
//  {
//    USB_Uart.Mode = Connect_Open;
//  }
//  else
//  {
//    USB_Uart.Mode = Connect_NoOpen;
//  }
//  LastInTime = HAL_GetTick();
  
	DEBUG_LOG("rate:%d\r\n", USB_Uart.BaudRate);
  DEBUG_LOG("Stop:%d\r\n",USB_Uart.StopBits);
  DEBUG_LOG("parity:%d\r\n",USB_Uart.Parity);
  DEBUG_LOG("DataBits:%d\r\n",USB_Uart.WordLength);
  
  if(USB_Uart.BaudRate == 0)
  {
    USB_Uart.Mode = Connect_NoOpen;
  }
  else
  {
    USB_Uart.Mode = Connect_Open;
  }
  
  if((USB_Uart.Mode == Connect_Open) && (USB_Uart.LastMode != Connect_Open))
  {
    USB_RESET_UART_HANDLE.Init.BaudRate = USB_Uart.BaudRate;
    
    switch(USB_Uart.WordLength)
    {
//      case 7:
//        USB_RESET_UART_HANDLE.Init.WordLength = UART_WORDLENGTH_7B;
//        break;
      case 8:
        USB_RESET_UART_HANDLE.Init.WordLength = UART_WORDLENGTH_8B;
        break;
      case 9:
        USB_RESET_UART_HANDLE.Init.WordLength = UART_WORDLENGTH_9B;
        break;
      default:break;
    }
    switch(USB_RESET_UART_HANDLE.Init.StopBits)
    {
      case 0:
        USB_RESET_UART_HANDLE.Init.StopBits = UART_STOPBITS_1;
        break;
//      case 1:
//        USB_RESET_UART_HANDLE.Init.StopBits = UART_STOPBITS_1_5;
//        break;
      case 2:
        USB_RESET_UART_HANDLE.Init.StopBits = UART_STOPBITS_2;
        break;
      default:break;
    }
    switch(USB_Uart.Parity)
    {
      case 0:
        USB_RESET_UART_HANDLE.Init.Parity = UART_PARITY_NONE;
        break;
      case 1:
        USB_RESET_UART_HANDLE.Init.Parity = UART_PARITY_ODD;
        break;
      case 2:
        USB_RESET_UART_HANDLE.Init.Parity = UART_PARITY_EVEN;
        break;
      //其余硬件不支持
      default:break;
    }
    if(DEBUG_PRINTF)printf("WritingInREG\r\n");
    //memset(pbuf,0,7);
    if (HAL_UART_Init(&USB_RESET_UART_HANDLE) != HAL_OK)
    {
      Error_Handler();
    }
		
		
    HAL_UART_Receive_IT(&USB_RESET_UART_HANDLE, &uart3TempRecvData, 1);
  }
  
  USB_Uart.LastMode = USB_Uart.Mode;
}


extern USBD_HandleTypeDef hUsbDeviceFS;

inline uint8_t Lime_USB_UART_Is_Connected(void)         //如果已连接，返回1，未连接返回0
{
  if(USB_HARDWARE_DISCONNECT)return 0;
//  {
//    USB_Uart.Mode = NoConnect;
//    USB_Uart.LastMode = NoConnect;
//  }
  if(USB_Uart.Mode == Connect_Open)return 1;
  return 0;
}

void Lime_USB_UART_RetransUSB_handle(uint8_t chr)  //放串口接收中断里
{
  if(Lime_USB_UART_Is_Connected())
    CDC_Transmit_FS(&chr,1);
}
inline void Lime_USB_UART_RetransUART_handle(uint8_t* Buf, uint32_t *Len)  //放USB接收中断
{
  if(Lime_USB_UART_Is_Connected())
    HAL_UART_Transmit(&USB_RESET_UART_HANDLE,Buf,*Len,0xff);
}


//new
#include "usb_device.h"

/* 全局变量定义 */
USBD_CDC_LineCodingTypeDef LineCoding = 
{
  115200,  // 默认波特率
  0x00,    // 停止位: 1位
  0x00,    // 校验位: 无
  0x08     // 数据位: 8位
};

uint8_t uart_rx_buffer[64];
volatile uint8_t usb_tx_busy = 0;

/* 设置UART参数 */
/* 设置UART参数 - 优化版，避免重复初始化 */
void CDC_SetLineCoding(USBD_CDC_LineCodingTypeDef *line_coding)
{
    DEBUG_LOG("%s(),start\n", __FUNCTION__);
    
    // 静态变量保存当前配置
    static USBD_CDC_LineCodingTypeDef current_coding = {
        .bitrate = 115200,
        .format = 0,
        .paritytype = 0,
        .datatype = 8
    };
    
    // 检查配置是否相同
    if (current_coding.bitrate == line_coding->bitrate &&
        current_coding.format == line_coding->format &&
        current_coding.paritytype == line_coding->paritytype &&
        current_coding.datatype == line_coding->datatype) {
        
        DEBUG_LOG("配置相同，无需重新初始化UART\n");
        DEBUG_LOG("rate:%d, Stop:%d, parity:%d, DataBits:%d\n",
                 current_coding.bitrate, current_coding.format,
                 current_coding.paritytype, current_coding.datatype);
        return;
    }
    
    DEBUG_LOG("检测到配置变化，重新初始化UART...\n");
    DEBUG_LOG("新配置: rate:%d, Stop:%d, parity:%d, DataBits:%d\n",
             line_coding->bitrate, line_coding->format,
             line_coding->paritytype, line_coding->datatype);
    
    // 保存新配置
    current_coding.bitrate = line_coding->bitrate;
    current_coding.format = line_coding->format;
    current_coding.paritytype = line_coding->paritytype;
    current_coding.datatype = line_coding->datatype;
    
    // 停止UART
    HAL_UART_DeInit(&huart2);
    
    // 配置数据位
    switch(line_coding->datatype)
    {
        case 7:
            // STM32通常不支持7位数据位，使用8位代替
            huart2.Init.WordLength = UART_WORDLENGTH_8B;
            DEBUG_LOG("警告：7位数据位不受支持，使用8位代替\n");
            break;
        case 8:
        default:
            huart2.Init.WordLength = UART_WORDLENGTH_8B;
            break;
    }
    
    // 配置停止位
    switch(line_coding->format)
    {
        case 1:
            // STM32通常不支持1.5停止位，使用1位代替
            huart2.Init.StopBits = UART_STOPBITS_1;
            DEBUG_LOG("警告：1.5停止位不受支持，使用1位代替\n");
            break;
        case 2:
            huart2.Init.StopBits = UART_STOPBITS_2;
            break;
        case 0:
        default:
            huart2.Init.StopBits = UART_STOPBITS_1;
            break;
    }
    
    // 配置校验位
    switch(line_coding->paritytype)
    {
        case 1:
            huart2.Init.Parity = UART_PARITY_ODD;
            break;
        case 2:
            huart2.Init.Parity = UART_PARITY_EVEN;
            break;
        case 0:
        default:
            huart2.Init.Parity = UART_PARITY_NONE;
            break;
    }
    
    // 设置波特率
    huart2.Init.BaudRate = line_coding->bitrate;
    
    // 重新初始化UART
    if (HAL_UART_Init(&huart2) != HAL_OK)
    {
        DEBUG_LOG("错误：UART初始化失败\n");
        Error_Handler();
    }
    
    DEBUG_LOG("UART重新初始化成功\n");
    DEBUG_LOG("当前配置 - rate:%d, Stop:%d, parity:%d, DataBits:%d\n",
             huart2.Init.BaudRate, huart2.Init.StopBits,
             huart2.Init.Parity, huart2.Init.WordLength);
    
    // 重新启动UART接收
    if (HAL_UART_Receive_IT(&huart2, uart_rx_buffer, sizeof(uart_rx_buffer)) != HAL_OK)
    {
        DEBUG_LOG("错误：无法启动UART接收中断\n");
    }
    else
    {
        DEBUG_LOG("UART接收中断已启动\n");
    }
}

/* 处理RTS/DTR控制线 */
void CDC_ControlLineState(uint8_t* pbuf)
{
	DEBUG_LOG("%s(%#x),start\n", __FUNCTION__, *pbuf);
	
  uint8_t control_line_state = *pbuf;
  
  // DTR信号 (bit0)
  if (control_line_state & 0x01) 
  {
    DEBUG_LOG("DTR 1\n");
		HAL_GPIO_WritePin(SIM_UART_DTR_GPIO_Port, SIM_UART_DTR_Pin, GPIO_PIN_SET);
  } 
  else 
  {
    DEBUG_LOG("DTR 0\n");
		HAL_GPIO_WritePin(SIM_UART_DTR_GPIO_Port, SIM_UART_DTR_Pin, GPIO_PIN_RESET);
  }
  
  // RTS信号 (bit1)
  if (control_line_state & 0x02) 
  {
    DEBUG_LOG("RTS 1\n");
		HAL_GPIO_WritePin(SIM_UART_RTS_GPIO_Port, SIM_UART_RTS_Pin, GPIO_PIN_SET);
  } 
  else 
  {
    DEBUG_LOG("RTS 0\n");
		HAL_GPIO_WritePin(SIM_UART_RTS_GPIO_Port, SIM_UART_RTS_Pin, GPIO_PIN_RESET);
  }
}

/* USB数据发送 */
/* UART2 DMA缓冲区 */
uint8_t uart2_rx_buf[UART2_RX_BUF_SIZE];
uint8_t uart2_tx_buf[UART2_TX_BUF_SIZE];
volatile uint16_t uart2_rx_size = 0;
volatile uint8_t uart2_dma_receiving = 0;

void UART2_DMA_Init(void)
{
    // 清零缓冲区
    memset(uart2_rx_buf, 0, UART2_RX_BUF_SIZE);
    memset(uart2_tx_buf, 0, UART2_TX_BUF_SIZE);
    uart2_rx_size = 0;
    uart2_dma_receiving = 0;
    
    DEBUG_LOG("UART2 DMA初始化完成\n");
}
void UART2_Start_Receive(void)
{
    if (HAL_UARTEx_ReceiveToIdle_DMA(&huart2, uart2_rx_buf, UART2_RX_BUF_SIZE) == HAL_OK)
    {
        uart2_dma_receiving = 1;
        DEBUG_LOG("UART2 DMA接收启动，缓冲区大小: %d\n", UART2_RX_BUF_SIZE);
    }
    else
    {
        uart2_dma_receiving = 0;
        DEBUG_LOG("UART2 DMA接收启动失败\n");
    }
}

/* UART2接收完成回调函数（IDLE中断） */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart->Instance == USART2)  // 根据你的UART2实例修改
    {
        DEBUG_LOG("UART2 IDLE中断，接收数据长度: %d\n", Size);
        
        if (Size > 0 && Size <= UART2_RX_BUF_SIZE)
        {
            // 保存接收到的数据长度
            uart2_rx_size = Size;
            
            // 通过USB CDC发送接收到的数据
            if (CDC_Transmit_FS(uart2_rx_buf, Size) == USBD_OK)
            {
                DEBUG_LOG("UART2数据通过USB发送成功，长度: %d\n", Size);
                
                // 调试：打印前几个字节
                if (Size > 0) {
                    DEBUG_LOG("数据前4字节: ");
                    for (int i = 0; i < (Size < 4 ? Size : 4); i++) {
                        DEBUG_LOG("0x%02X ", uart2_rx_buf[i]);
                    }
                    DEBUG_LOG("\n");
                }
            }
            else
            {
                DEBUG_LOG("USB发送繁忙，数据丢失\n");
            }
            
            // 清零接收长度
            uart2_rx_size = 0;
        }
        
        // 重新启动DMA接收
        UART2_Start_Receive();
    }
}

/* UART错误回调 */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2)
    {
        DEBUG_LOG("UART2错误: 0x%08lX\n", huart->ErrorCode);
        
        // 重新启动接收
        if (huart->ErrorCode != HAL_UART_ERROR_NONE)
        {
            HAL_UART_AbortReceive(&huart2);
            uart2_dma_receiving = 0;
            UART2_Start_Receive();
        }
    }
}
