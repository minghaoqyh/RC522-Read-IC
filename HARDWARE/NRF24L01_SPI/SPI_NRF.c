#include "spi_nrf.h"
#include "delay.h"
u8 TX_ADDRESS[TX_ADR_WIDTH] = {0x0A,0x01,0x07,0x0E,0x01};  // 定义一个静态发送地址

void GPIO_Init_24L01(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA |RCC_APB2Periph_GPIOC, ENABLE );	

	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_2 |GPIO_Pin_3 | GPIO_Pin_4 ;//NRF_NRF_IRQ/NRF_CE
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_SetBits(GPIOA,GPIO_Pin_2 |GPIO_Pin_3| GPIO_Pin_4);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 ;//NRF_CS
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
 	GPIO_SetBits(GPIOC,GPIO_Pin_4);	
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_5 | GPIO_Pin_7 ; //PA.5 = SCK ,PA.7 = MOSI
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 ;//PA.6 = MISO ,数据输入引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  //上拉输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOA,GPIO_Pin_1 | GPIO_Pin_5|GPIO_Pin_7);
}

static u8 SPI_RW(u8 byte)
{
	u8 bit_ctr = 0 , d = 0 ;
	for(bit_ctr=0; bit_ctr<8; bit_ctr++)
	{
		d <<= 1;			
			GPIO_WriteBit(GPIOA , GPIO_Pin_5 , 0);//sck = 0
			delay_us(100);
			if(byte & 0x80)
				GPIO_WriteBit(GPIOA , GPIO_Pin_7 , 1);//mosi = 1
			else
				GPIO_WriteBit(GPIOA , GPIO_Pin_7 , 0);//mosi = 0
     
			GPIO_WriteBit(GPIOA , GPIO_Pin_5 , 1);//sck = 1
			//CCDelay(100);

			if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6))
				d |= 1;

			byte <<= 1;	                                
	}
			GPIO_WriteBit(GPIOA , GPIO_Pin_5 , 0);//sck = 0
	
	return d;                              
}


/********************************************************
函数功能：SI24R1引脚初始化                
入口参数：无
返回  值：无
*********************************************************/
void SI24R1_Init(void)
{
	SCK = 0; 													//SPI时钟线拉低
	CSN = 1;				
	CE 	= 0;				
	IRQ = 1;
}


/********************************************************
函数功能：写寄存器的值（单字节）                
入口参数：reg:寄存器映射地址（格式：NRF_WRITE_REG｜reg）
					value:寄存器的值
返回  值：状态寄存器的值
*********************************************************/
u8 SI24R1_NRF_WRITE_REG(u8 reg, u8 value)
{
	u8 status;

	CSN = 0;                 
	status = SPI_RW(reg);				
	SPI_RW(value);
	CSN = 1;  
	
	return(status);
}


/********************************************************
函数功能：写寄存器的值（多字节）                  
入口参数：reg:寄存器映射地址（格式：NRF_WRITE_REG｜reg）
					pBuf:写数据首地址
					bytes:写数据字节数
返回  值：状态寄存器的值
*********************************************************/
u8 SI24R1_Write_Buf(u8 reg, const u8 *pBuf, u8 bytes)
{
	u8 status,byte_ctr;

  CSN = 0;                                  			
  status = SPI_RW(reg);                          
  for(byte_ctr=0; byte_ctr<bytes; byte_ctr++)     
    SPI_RW(*pBuf++);
  CSN = 1;                                      	

  return(status);       
}							  					   


/********************************************************
函数功能：读取寄存器的值（单字节）                  
入口参数：reg:寄存器映射地址（格式：READ_REG｜reg）
返回  值：寄存器值
*********************************************************/
u8 SI24R1_Read_Reg(u8 reg)
{
 	u8 value;

	CSN = 0;    
	SPI_RW(reg);			
	value = SPI_RW(0);
	CSN = 1;              

	return(value);
}


/********************************************************
函数功能：读取寄存器的值（多字节）                  
入口参数：reg:寄存器映射地址（READ_REG｜reg）
					pBuf:接收缓冲区的首地址
					bytes:读取字节数
返回  值：状态寄存器的值
*********************************************************/
u8 SI24R1_Read_Buf(u8 reg, u8 *pBuf, u8 bytes)
{
	u8 status,byte_ctr;

  CSN = 0;                                        
  status = SPI_RW(reg);                           
  for(byte_ctr=0;byte_ctr<bytes;byte_ctr++)
    pBuf[byte_ctr] = SPI_RW(0);                   //读取数据，低字节在前
  CSN = 1;                                        

  return(status);    
}


/********************************************************
函数功能：SI24R1接收模式初始化                      
入口参数：无
返回  值：无
*********************************************************/
void SI24R1_RX_Mode(void)
{
	CE = 0;
	SI24R1_Write_Buf(NRF_WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH);	// 接收设备接收通道0使用和发送设备相同的发送地址
	SI24R1_NRF_WRITE_REG(NRF_WRITE_REG + EN_AA, 0x01);               						// 使能接收通道0自动应答
	SI24R1_NRF_WRITE_REG(NRF_WRITE_REG + EN_RXADDR, 0x01);           						// 使能接收通道0
	SI24R1_NRF_WRITE_REG(NRF_WRITE_REG + RF_CH, 40);                 						// 选择射频通道0x40
	SI24R1_NRF_WRITE_REG(NRF_WRITE_REG + RX_PW_P0, TX_PLOAD_WIDTH);  						// 接收通道0选择和发送通道相同有效数据宽度
	SI24R1_NRF_WRITE_REG(NRF_WRITE_REG + RF_SETUP, 0x0f);            						// 数据传输率2Mbps，发射功率7dBm
	SI24R1_NRF_WRITE_REG(NRF_WRITE_REG + CONFIG, 0x0f);              						// CRC使能，16位CRC校验，上电，接收模式
	SI24R1_NRF_WRITE_REG(NRF_WRITE_REG + STATUS, 0xff);  												//清除所有的中断标志位
	CE = 1;                                            									// 拉高CE启动接收设备
}						


/********************************************************
函数功能：SI24R1发送模式初始化                      
入口参数：无
返回  值：无
*********************************************************/
void SI24R1_TX_Mode(void)
{
	CE = 0;
	SI24R1_Write_Buf(NRF_WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);     // 写入发送地址
	SI24R1_Write_Buf(NRF_WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH);  // 为了应答接收设备，接收通道0地址和发送地址相同

	SI24R1_NRF_WRITE_REG(NRF_WRITE_REG + EN_AA, 0x00);       											// 使能接收通道0自动应答(调试过程关闭了自动应答，调试完成添加上)
	SI24R1_NRF_WRITE_REG(NRF_WRITE_REG + EN_RXADDR, 0x00);   											// 使能接收通道0(调试过程关闭了接收通道0，调试完成添加上)
	SI24R1_NRF_WRITE_REG(NRF_WRITE_REG + SETUP_RETR, 0x00);  											// 0X0A自动重发延时等待250us+86us，自动重发10次(调试过程关闭了接收通道0，调试完成添加上)
	SI24R1_NRF_WRITE_REG(NRF_WRITE_REG + RF_CH, 40);         											// 选择射频通道0x40
	SI24R1_NRF_WRITE_REG(NRF_WRITE_REG + RF_SETUP, 0x0f);    											// 数据传输率2Mbps，发射功率7dBm
	SI24R1_NRF_WRITE_REG(NRF_WRITE_REG + CONFIG, 0x0e);      											// CRC使能，16位CRC校验，上电
	CE = 1;
	delay_ms(10);
}


/********************************************************
函数功能：读取接收数据                       
入口参数：rxbuf:接收数据存放首地址
返回  值：0:接收到数据
          1:没有接收到数据
*********************************************************/
u8 SI24R1_RxPacket(u8 *rxbuf)
{
	u8 state;
	state = SI24R1_Read_Reg(STATUS);  			                 //读取状态寄存器的值    	  
	SI24R1_NRF_WRITE_REG(NRF_WRITE_REG+STATUS,state);               //清除RX_DS中断标志

	if(state & RX_DR)								                           //接收到数据
	{
		SI24R1_Read_Buf(RD_RX_PLOAD,rxbuf,TX_PLOAD_WIDTH);     //读取数据
		SI24R1_NRF_WRITE_REG(FLUSH_RX,0xff);					              //清除RX FIFO寄存器
		return 0; 
	}	   
	return 1;                                                   //没收到任何数据
}


/********************************************************
函数功能：发送一个数据包                      
入口参数：txbuf:要发送的数据
返回  值：0x10:达到最大重发次数，发送失败 
          0x20:发送成功            
          0xff:发送失败                  
*********************************************************/
u8 SI24R1_TxPacket(u8 *txbuf)
{
	u8 state;
	CE=0;																										  //CE拉低，使能SI24R1配置
  SI24R1_Write_Buf(WR_TX_PLOAD, txbuf, TX_PLOAD_WIDTH);	    //写数据到TX FIFO,32个字节
 	CE=1;																										  //CE置高，使能发送	   
	
//	while(IRQ == 1);																				  //等待发送完成
	state = SI24R1_Read_Reg(STATUS);  											  //读取状态寄存器的值	   
	SI24R1_NRF_WRITE_REG(NRF_WRITE_REG+STATUS, state); 								//清除TX_DS或MAX_RT中断标志
	if(state&MAX_RT)																			    //达到最大重发次数
	{
		SI24R1_NRF_WRITE_REG(FLUSH_TX,0xff);										    //清除TX FIFO寄存器 
		return MAX_RT; 
	}
	if(state&TX_DS)																			      //发送完成
	{
		return TX_DS;
	}
	return 0XFF;																						  //发送失败
}
/**************************************************
函数：NRF_Check()

描述：
    开机自检函数
**************************************************/
u8 NRF_Check(void)  
{  
    u8 buf[5]={0xC2,0xC2,0xC2,0xC2,0xC2};  
    u8 buf1[5];  
    u8 i;   
       
    SI24R1_Write_Buf(NRF_WRITE_REG+ TX_ADDR,buf,5); /*写入5个字节的地址.  */    
    SI24R1_Read_Buf( NRF_READ_REG+ TX_ADDR,buf1 ,5 ); /*读出写入的地址 */                   
    for(i=0;i<5;i++)     /*比较*/   
    {  
        if(buf1[i]!=0xC2)  
        break;  
    }     
    if(i==5)  
        return SUCCESS ;        //MCU与NRF成功连接   
    else  
        return ERROR ;        //MCU与NRF不正常连接  
} 
/**************************************************
函数：NRF_WriteTXPayload_Ack

描述：
    
**************************************************/
void NRF_WriteTXPayload_Ack( char *pBuff, u8 nBytes )
{
    u8 btmp;
    u8 length = ( nBytes > 32 ) ? 32 : nBytes;

   	SI24R1_NRF_WRITE_REG(FLUSH_TX,0xff);										    //清除TX FIFO寄存器 //清除TX_FIFO寄存器
    CSN = 0;
    SPI_RW( WR_TX_PLOAD );
    for( btmp = 0; btmp < length; btmp ++ )
    {
        SPI_RW( *( pBuff + btmp ) );
    }
     CSN = 1;
}