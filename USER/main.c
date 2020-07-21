#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "key.h"
#include "spi.h"
#include "spi_nrf.h"
#include "key.h"
#include "stm32f10x.h"
#include "string.h"
#include "timer.h"
#include "rc522.h"
//ALIENTEK Mini STM32开发板范例代码20
//SPI实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司
   	
/***************************************************/
extern u8 sta ;
extern u8 RX_BUF[4];
extern u8 TX_BUF[4];
extern u8 flag;
extern u8 USART_RX_REC_flag;
char *config_data;

 int atoi(char s[])
 {
	int i , n ;
	 n = 0 ;
	 for(i = 0 ; s[i] >='0' && s[i] <= '9'; ++i )
	 {
	 n = 10*n + (s[i] - '0') ;
	 }
	 return n ;
 }
/******
* str 目的字符串
* 出口参数： s_addr 扇区地址 ，c_addr 块地址
**
**
*/
static u8 Search_str( char *str ,  u8 *s_addr ,u8 *c_addr ) // serach config_data in serial buf 
 {
	 u8 i = 0 , str_len = 0 ; 
//	 u8 uart_rec[64]  ;
//	 memset ( uart_rec , 0 , 64) ;
	 
	 str_len = strlen(str) ;

	 for(i = 0 ; i < str_len ; i ++)
	 {
		 if(*(str+i) == 's' & *(str + i + 6) == ':')
		 {
			 *s_addr = *(str + i + 7) ;
		*s_addr = atoi(s_addr);
		 }
		 if(*(str+i) == 'c' & *(str + i + 6) == ':')
		 {
			 *c_addr = *(str + i + 7) ;
		 *c_addr = atoi(c_addr);
		 }
		//uart_rec [i] = *(str + i) ;
	 }
	 return 1 ; 
 }

 /**************************************************
函数：main()

描述：
    主函数
**************************************************/
int main()
{
    u8 tmp, x , temp,press_key ;
	u8 exchange_done ;
	int i = 0 ;
 u8 d1_addr, c1_addr ; 
    char *AskData = "How are you?\r\n";
    u8 testbuffer[32];
	u8 write_data[8] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08};
		u8 buf1[5] = {0} ;
		delay_init();
		KEY_Init();
		//GPIO_Init_24L01();
		//SI24R1_Init();
		LED_Init();		  	//初始化与LED连接的硬件接口
	//	TIM3_Int_Init(4999,7199);//10Khz的计数频率，计数到5000为500ms  
//    uart_init(9600);
			uart_init(115200);	
	printf( "nRF24NRFP RECEIVES tart！\r\n" );

			
	InitRc522();				//
  	while(1) 
	{		
//		Wirte_Block(1 , 1 , write_data);

		
		if(USART_RX_REC_flag)
		{
			USART_RX_REC_flag = 0 ;
			config_data = USART_RX_BUF ;
		exchange_done = Search_str(config_data , &d1_addr , &c1_addr );
			//memset(USART_RX_BUF ,0 , USART_REC_MAX_LEN);
			if(exchange_done)	
		{
			exchange_done = 0 ;
		RC522_Handel(d1_addr ,c1_addr);
			//memset(USART_RX_BUF ,0 , USART_REC_MAX_LEN);
		}
		}
		
		#if 0
		press_key = KEY_Scan(0);
		switch(press_key)
		{
			case KEY0_PRES:
				printf( "KEY0_PRES！\r\n" );
				break ;
			case KEY1_PRES:
				printf( "KEY1_PRES！\r\n" );
				break ;
			case WKUP_PRES:
				printf( "WKUP_PRES！\r\n" );
				break ;	
		}
		#endif
		
	
	}
		return 0;
}
 
/**************************************************/