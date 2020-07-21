#include "spi_nrf.h"
#include "delay.h"
u8 TX_ADDRESS[TX_ADR_WIDTH] = {0x0A,0x01,0x07,0x0E,0x01};  // ����һ����̬���͵�ַ

void GPIO_Init_24L01(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA |RCC_APB2Periph_GPIOC, ENABLE );	

	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_2 |GPIO_Pin_3 | GPIO_Pin_4 ;//NRF_NRF_IRQ/NRF_CE
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_SetBits(GPIOA,GPIO_Pin_2 |GPIO_Pin_3| GPIO_Pin_4);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 ;//NRF_CS
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
 	GPIO_SetBits(GPIOC,GPIO_Pin_4);	
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_5 | GPIO_Pin_7 ; //PA.5 = SCK ,PA.7 = MOSI
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 ;//PA.6 = MISO ,������������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  //��������
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
�������ܣ�SI24R1���ų�ʼ��                
��ڲ�������
����  ֵ����
*********************************************************/
void SI24R1_Init(void)
{
	SCK = 0; 													//SPIʱ��������
	CSN = 1;				
	CE 	= 0;				
	IRQ = 1;
}


/********************************************************
�������ܣ�д�Ĵ�����ֵ�����ֽڣ�                
��ڲ�����reg:�Ĵ���ӳ���ַ����ʽ��NRF_WRITE_REG��reg��
					value:�Ĵ�����ֵ
����  ֵ��״̬�Ĵ�����ֵ
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
�������ܣ�д�Ĵ�����ֵ�����ֽڣ�                  
��ڲ�����reg:�Ĵ���ӳ���ַ����ʽ��NRF_WRITE_REG��reg��
					pBuf:д�����׵�ַ
					bytes:д�����ֽ���
����  ֵ��״̬�Ĵ�����ֵ
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
�������ܣ���ȡ�Ĵ�����ֵ�����ֽڣ�                  
��ڲ�����reg:�Ĵ���ӳ���ַ����ʽ��READ_REG��reg��
����  ֵ���Ĵ���ֵ
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
�������ܣ���ȡ�Ĵ�����ֵ�����ֽڣ�                  
��ڲ�����reg:�Ĵ���ӳ���ַ��READ_REG��reg��
					pBuf:���ջ��������׵�ַ
					bytes:��ȡ�ֽ���
����  ֵ��״̬�Ĵ�����ֵ
*********************************************************/
u8 SI24R1_Read_Buf(u8 reg, u8 *pBuf, u8 bytes)
{
	u8 status,byte_ctr;

  CSN = 0;                                        
  status = SPI_RW(reg);                           
  for(byte_ctr=0;byte_ctr<bytes;byte_ctr++)
    pBuf[byte_ctr] = SPI_RW(0);                   //��ȡ���ݣ����ֽ���ǰ
  CSN = 1;                                        

  return(status);    
}


/********************************************************
�������ܣ�SI24R1����ģʽ��ʼ��                      
��ڲ�������
����  ֵ����
*********************************************************/
void SI24R1_RX_Mode(void)
{
	CE = 0;
	SI24R1_Write_Buf(NRF_WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH);	// �����豸����ͨ��0ʹ�úͷ����豸��ͬ�ķ��͵�ַ
	SI24R1_NRF_WRITE_REG(NRF_WRITE_REG + EN_AA, 0x01);               						// ʹ�ܽ���ͨ��0�Զ�Ӧ��
	SI24R1_NRF_WRITE_REG(NRF_WRITE_REG + EN_RXADDR, 0x01);           						// ʹ�ܽ���ͨ��0
	SI24R1_NRF_WRITE_REG(NRF_WRITE_REG + RF_CH, 40);                 						// ѡ����Ƶͨ��0x40
	SI24R1_NRF_WRITE_REG(NRF_WRITE_REG + RX_PW_P0, TX_PLOAD_WIDTH);  						// ����ͨ��0ѡ��ͷ���ͨ����ͬ��Ч���ݿ��
	SI24R1_NRF_WRITE_REG(NRF_WRITE_REG + RF_SETUP, 0x0f);            						// ���ݴ�����2Mbps�����书��7dBm
	SI24R1_NRF_WRITE_REG(NRF_WRITE_REG + CONFIG, 0x0f);              						// CRCʹ�ܣ�16λCRCУ�飬�ϵ磬����ģʽ
	SI24R1_NRF_WRITE_REG(NRF_WRITE_REG + STATUS, 0xff);  												//������е��жϱ�־λ
	CE = 1;                                            									// ����CE���������豸
}						


/********************************************************
�������ܣ�SI24R1����ģʽ��ʼ��                      
��ڲ�������
����  ֵ����
*********************************************************/
void SI24R1_TX_Mode(void)
{
	CE = 0;
	SI24R1_Write_Buf(NRF_WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);     // д�뷢�͵�ַ
	SI24R1_Write_Buf(NRF_WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH);  // Ϊ��Ӧ������豸������ͨ��0��ַ�ͷ��͵�ַ��ͬ

	SI24R1_NRF_WRITE_REG(NRF_WRITE_REG + EN_AA, 0x00);       											// ʹ�ܽ���ͨ��0�Զ�Ӧ��(���Թ��̹ر����Զ�Ӧ�𣬵�����������)
	SI24R1_NRF_WRITE_REG(NRF_WRITE_REG + EN_RXADDR, 0x00);   											// ʹ�ܽ���ͨ��0(���Թ��̹ر��˽���ͨ��0��������������)
	SI24R1_NRF_WRITE_REG(NRF_WRITE_REG + SETUP_RETR, 0x00);  											// 0X0A�Զ��ط���ʱ�ȴ�250us+86us���Զ��ط�10��(���Թ��̹ر��˽���ͨ��0��������������)
	SI24R1_NRF_WRITE_REG(NRF_WRITE_REG + RF_CH, 40);         											// ѡ����Ƶͨ��0x40
	SI24R1_NRF_WRITE_REG(NRF_WRITE_REG + RF_SETUP, 0x0f);    											// ���ݴ�����2Mbps�����书��7dBm
	SI24R1_NRF_WRITE_REG(NRF_WRITE_REG + CONFIG, 0x0e);      											// CRCʹ�ܣ�16λCRCУ�飬�ϵ�
	CE = 1;
	delay_ms(10);
}


/********************************************************
�������ܣ���ȡ��������                       
��ڲ�����rxbuf:�������ݴ���׵�ַ
����  ֵ��0:���յ�����
          1:û�н��յ�����
*********************************************************/
u8 SI24R1_RxPacket(u8 *rxbuf)
{
	u8 state;
	state = SI24R1_Read_Reg(STATUS);  			                 //��ȡ״̬�Ĵ�����ֵ    	  
	SI24R1_NRF_WRITE_REG(NRF_WRITE_REG+STATUS,state);               //���RX_DS�жϱ�־

	if(state & RX_DR)								                           //���յ�����
	{
		SI24R1_Read_Buf(RD_RX_PLOAD,rxbuf,TX_PLOAD_WIDTH);     //��ȡ����
		SI24R1_NRF_WRITE_REG(FLUSH_RX,0xff);					              //���RX FIFO�Ĵ���
		return 0; 
	}	   
	return 1;                                                   //û�յ��κ�����
}


/********************************************************
�������ܣ�����һ�����ݰ�                      
��ڲ�����txbuf:Ҫ���͵�����
����  ֵ��0x10:�ﵽ����ط�����������ʧ�� 
          0x20:���ͳɹ�            
          0xff:����ʧ��                  
*********************************************************/
u8 SI24R1_TxPacket(u8 *txbuf)
{
	u8 state;
	CE=0;																										  //CE���ͣ�ʹ��SI24R1����
  SI24R1_Write_Buf(WR_TX_PLOAD, txbuf, TX_PLOAD_WIDTH);	    //д���ݵ�TX FIFO,32���ֽ�
 	CE=1;																										  //CE�øߣ�ʹ�ܷ���	   
	
//	while(IRQ == 1);																				  //�ȴ��������
	state = SI24R1_Read_Reg(STATUS);  											  //��ȡ״̬�Ĵ�����ֵ	   
	SI24R1_NRF_WRITE_REG(NRF_WRITE_REG+STATUS, state); 								//���TX_DS��MAX_RT�жϱ�־
	if(state&MAX_RT)																			    //�ﵽ����ط�����
	{
		SI24R1_NRF_WRITE_REG(FLUSH_TX,0xff);										    //���TX FIFO�Ĵ��� 
		return MAX_RT; 
	}
	if(state&TX_DS)																			      //�������
	{
		return TX_DS;
	}
	return 0XFF;																						  //����ʧ��
}
/**************************************************
������NRF_Check()

������
    �����Լ캯��
**************************************************/
u8 NRF_Check(void)  
{  
    u8 buf[5]={0xC2,0xC2,0xC2,0xC2,0xC2};  
    u8 buf1[5];  
    u8 i;   
       
    SI24R1_Write_Buf(NRF_WRITE_REG+ TX_ADDR,buf,5); /*д��5���ֽڵĵ�ַ.  */    
    SI24R1_Read_Buf( NRF_READ_REG+ TX_ADDR,buf1 ,5 ); /*����д��ĵ�ַ */                   
    for(i=0;i<5;i++)     /*�Ƚ�*/   
    {  
        if(buf1[i]!=0xC2)  
        break;  
    }     
    if(i==5)  
        return SUCCESS ;        //MCU��NRF�ɹ�����   
    else  
        return ERROR ;        //MCU��NRF����������  
} 
/**************************************************
������NRF_WriteTXPayload_Ack

������
    
**************************************************/
void NRF_WriteTXPayload_Ack( char *pBuff, u8 nBytes )
{
    u8 btmp;
    u8 length = ( nBytes > 32 ) ? 32 : nBytes;

   	SI24R1_NRF_WRITE_REG(FLUSH_TX,0xff);										    //���TX FIFO�Ĵ��� //���TX_FIFO�Ĵ���
    CSN = 0;
    SPI_RW( WR_TX_PLOAD );
    for( btmp = 0; btmp < length; btmp ++ )
    {
        SPI_RW( *( pBuff + btmp ) );
    }
     CSN = 1;
}