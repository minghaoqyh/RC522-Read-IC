#include "sys.h"
//#include "rc522.h"
#include "delay.h"
#include "usart.h"
#include "string.h"
#include "rc522_config.h"

//////////////////////////////////////////////////////////
// M1???16???,????????(?0??1??2??3)??
// ?16????64??????????:0~63
// ?0?????0(?????0?),????????,????????
// ??????0??1??2????,???????
// ??????3????(?????:?3??7??11.....)????A,???????B?

/*******************************
*????:
*1--SDA  <----->PA4
*2--SCK  <----->PA5
*3--MOSI <----->PA7
*4--MISO <----->PA6
*5--??
*6--GND <----->GND
*7--RST <----->PB0
*8--VCC <----->VCC
************************************/

/*????*/
unsigned char CT[2];//???
unsigned char SN[4]; //??
unsigned char RFID[16];			//??RFID
unsigned char lxl_bit=0;
unsigned char card1_bit=0;
unsigned char card2_bit=0;
unsigned char card3_bit=0;
unsigned char card4_bit=0;
unsigned char total=0;
unsigned char lxl[4]= {137,240,42,131};
unsigned char card_1[4]= {83,106,11,1};
unsigned char card_2[4]= {208,121,31,57};
unsigned char card_3[4]= {176,177,143,165};
unsigned char card_4[4]= {5,158,10,136};
u8 KEY[6]= {0xff,0xff,0xff,0xff,0xff,0xff};
u8 AUDIO_OPEN[6] = {0xAA, 0x07, 0x02, 0x00, 0x09, 0xBC};
unsigned char RFID1[16]= {0x00,0x00,0x00,0x00,0x00,0x00,0xff,0x07,0x80,0x29,0xff,0xff,0xff,0xff,0xff,0xff};
/*????*/
unsigned char status;
unsigned char s=0x08;


#define   RC522_DELAY()  delay_us( 20 )


void RC522_Handel(void)
{

    status = PcdRequest(PICC_REQALL,CT);//??

    //printf("\r\nstatus>>>>>>%d\r\n", status);

    if(status==MI_OK)// ????
    {
        status=MI_ERR;
        status = PcdAnticoll(SN);// ???
    }

    if (status==MI_OK)// ?????
    {
        status=MI_ERR;
        ShowID(SN); // ??????ID?

        if((SN[0]==lxl[0])&&(SN[1]==lxl[1])&&(SN[2]==lxl[2])&&(SN[3]==lxl[3]))
        {
            lxl_bit=1;
            printf("\r\nThe User is:card_0\r\n");

        }
        if((SN[0]==card_1[0])&&(SN[1]==card_1[1])&&(SN[2]==card_1[2])&&(SN[3]==card_1[3]))
        {
            card1_bit=1;
            printf("\r\nThe User is:card_1\r\n");

        }
        if((SN[0]==card_2[0])&&(SN[1]==card_2[1])&&(SN[2]==card_2[2])&&(SN[3]==card_2[3]))
        {
            card2_bit=1;
            printf("\r\nThe User is:card_2\r\n");

        }

        if((SN[0]==card_3[0])&&(SN[1]==card_3[1])&&(SN[2]==card_3[2])&&(SN[3]==card_3[3]))
        {
            card3_bit=1;
            printf("\r\nThe User is:card_3\r\n");

        }
        if((SN[0]==card_4[0])&&(SN[1]==card_4[1])&&(SN[2]==card_4[2])&&(SN[3]==card_4[3]))
        {
            card4_bit=1;
            printf("\r\nThe User is:card_4\r\n");

        }
        //total=card1_bit+card2_bit+card3_bit+card4_bit+lxl_bit;
        status =PcdSelect(SN);

    }

    if(status==MI_OK)//????
    {

        status=MI_ERR;
        status =PcdAuthState(0x60,0x09,KEY,SN);
    }
    if(status==MI_OK)//????
    {
        status=MI_ERR;
        status=PcdRead(s,RFID);
    }

    if(status==MI_OK)//????
    {
        status=MI_ERR;
        delay_ms(100);
    }

}

void RC522_Init ( void )
{
    SPI1_Init();

    RC522_Reset_Disable();

    RC522_CS_Disable();

    PcdReset ();

    M500PcdConfigISOType ( 'A' );//??????

}

void SPI1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(	RC522_SPI_GPIO_ENABLED | RC522_SPI_GPIO_RST_ENABLED, ENABLE );//PORTA?B????

    // CS
    GPIO_InitStructure.GPIO_Pin = RC522_SPI_GPIO_PIN_CS;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //????
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO????50MHz
    GPIO_Init(RC522_SPI_GPIO, &GPIO_InitStructure);					 //?????????PF0?PF1

    // SCK
    GPIO_InitStructure.GPIO_Pin = RC522_SPI_GPIO_PIN_SCK;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //????
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO????50MHz
    GPIO_Init(RC522_SPI_GPIO , &GPIO_InitStructure);

    // MISO
    GPIO_InitStructure.GPIO_Pin = RC522_SPI_GPIO_PIN_MISO;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 		 //????
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO????50MHz
    GPIO_Init(RC522_SPI_GPIO , &GPIO_InitStructure);

    // MOSI
    GPIO_InitStructure.GPIO_Pin = RC522_SPI_GPIO_PIN_MOSI;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //????
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO????50MHz
    GPIO_Init(RC522_SPI_GPIO , &GPIO_InitStructure);

    // RST
    GPIO_InitStructure.GPIO_Pin = RC522_SPI_GPIO_PIN_RST;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //????
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO????50MHz
    GPIO_Init(RC522_SPI_GPIO_RST, &GPIO_InitStructure);

}


/*
 * ???:SPI_RC522_SendByte
 * ??  :?RC522??1 Byte ??
 * ??  :byte,??????
 * ??  : RC522?????
 * ??  :????
 */
void SPI_RC522_SendByte ( u8 byte )
{
    u8 counter;

    for(counter=0; counter<8; counter++)
    {
        if ( byte & 0x80 )
            RC522_MOSI_1 ();
        else
            RC522_MOSI_0 ();

        RC522_DELAY();

        RC522_SCK_0 ();

        RC522_DELAY();

        RC522_SCK_1();

        RC522_DELAY();

        byte <<= 1;

    }

}


/*
 * ???:SPI_RC522_ReadByte
 * ??  :?RC522??1 Byte ??
 * ??  :?
 * ??  : RC522?????
 * ??  :????
 */
u8 SPI_RC522_ReadByte ( void )
{
    u8 counter;
    u8 SPI_Data;


    for(counter=0; counter<8; counter++)
    {
        SPI_Data <<= 1;

        RC522_SCK_0 ();

        RC522_DELAY();

        if ( RC522_MISO_GET() == 1)
            SPI_Data |= 0x01;

        RC522_DELAY();

        RC522_SCK_1 ();

        RC522_DELAY();

    }


//	printf("****%c****",SPI_Data);
    return SPI_Data;
}


/*
 * ???:ReadRawRC
 * ??  :?RC522???
 * ??  :ucAddress,?????
 * ??  : ???????
 * ??  :????
 */
u8 ReadRawRC ( u8 ucAddress )
{
    u8 ucAddr, ucReturn;


    ucAddr = ( ( ucAddress << 1 ) & 0x7E ) | 0x80;

    RC522_CS_Enable();

    SPI_RC522_SendByte ( ucAddr );

    ucReturn = SPI_RC522_ReadByte ();

    RC522_CS_Disable();

    return ucReturn;
}


/*
 * ???:WriteRawRC
 * ??  :?RC522???
 * ??  :ucAddress,?????
 *         ucValue,???????
 * ??  : ?
 * ??  :????
 */
void WriteRawRC ( u8 ucAddress, u8 ucValue )
{
    u8 ucAddr;

    ucAddr = ( ucAddress << 1 ) & 0x7E;

    RC522_CS_Enable();

    SPI_RC522_SendByte ( ucAddr );

    SPI_RC522_SendByte ( ucValue );

    RC522_CS_Disable();
}


/*
 * ???:SetBitMask
 * ??  :?RC522?????
 * ??  :ucReg,?????
 *         ucMask,???
 * ??  : ?
 * ??  :????
 */
void SetBitMask ( u8 ucReg, u8 ucMask )
{
    u8 ucTemp;

    ucTemp = ReadRawRC ( ucReg );

    WriteRawRC ( ucReg, ucTemp | ucMask );         // set bit mask

}


/*
 * ???:ClearBitMask
 * ??  :?RC522?????
 * ??  :ucReg,?????
 *         ucMask,???
 * ??  : ?
 * ??  :????
 */
void ClearBitMask ( u8 ucReg, u8 ucMask )
{
    u8 ucTemp;

    ucTemp = ReadRawRC ( ucReg );

    WriteRawRC ( ucReg, ucTemp & ( ~ ucMask) );  // clear bit mask

}


/*
 * ???:PcdAntennaOn
 * ??  :????
 * ??  :?
 * ??  : ?
 * ??  :????
 */
void PcdAntennaOn ( void )
{
    u8 uc;

    uc = ReadRawRC ( TxControlReg );

    if ( ! ( uc & 0x03 ) )
        SetBitMask(TxControlReg, 0x03);

}


/*
 * ???:PcdAntennaOff
 * ??  :????
 * ??  :?
 * ??  : ?
 * ??  :????
 */
void PcdAntennaOff ( void )
{

    ClearBitMask ( TxControlReg, 0x03 );

}


/*
 * ???:PcdRese
 * ??  :??RC522
 * ??  :?
 * ??  : ?
 * ??  :????
 */
void PcdReset ( void )
{
    RC522_Reset_Disable();

    delay_us ( 1 );

    RC522_Reset_Enable();

    delay_us ( 1 );

    RC522_Reset_Disable();

    delay_us ( 1 );

    WriteRawRC ( CommandReg, 0x0f );

    while ( ReadRawRC ( CommandReg ) & 0x10 );

    delay_us ( 1 );

    WriteRawRC ( ModeReg, 0x3D );            //??????????? ?Mifare???,CRC???0x6363

    WriteRawRC ( TReloadRegL, 30 );          //16??????
    WriteRawRC ( TReloadRegH, 0 );			 //16??????

    WriteRawRC ( TModeReg, 0x8D );		      //??????????

    WriteRawRC ( TPrescalerReg, 0x3E );			 //?????????

    WriteRawRC ( TxAutoReg, 0x40 );				   //???????100%ASK


}


/*
 * ???:M500PcdConfigISOType
 * ??  :??RC522?????
 * ??  :ucType,????
 * ??  : ?
 * ??  :????
 */
void M500PcdConfigISOType ( u8 ucType )
{
    if ( ucType == 'A')                     //ISO14443_A
    {
        ClearBitMask ( Status2Reg, 0x08 );

        WriteRawRC ( ModeReg, 0x3D );//3F

        WriteRawRC ( RxSelReg, 0x86 );//84

        WriteRawRC( RFCfgReg, 0x7F );   //4F

        WriteRawRC( TReloadRegL, 30 );//tmoLength);// TReloadVal = 'h6a =tmoLength(dec)

        WriteRawRC ( TReloadRegH, 0 );

        WriteRawRC ( TModeReg, 0x8D );

        WriteRawRC ( TPrescalerReg, 0x3E );

        delay_us ( 2 );

        PcdAntennaOn ();//???

    }

}


/*
 * ???:PcdComMF522
 * ??  :??RC522?ISO14443???
 * ??  :ucCommand,RC522???
 *         pInData,??RC522????????
 *         ucInLenByte,?????????
 *         pOutData,??????????
 *         pOutLenBit,????????
 * ??  : ???
 *         = MI_OK,??
 * ??  :????
 */
char PcdComMF522 ( u8 ucCommand, u8 * pInData, u8 ucInLenByte, u8 * pOutData, u32 * pOutLenBit )
{
    char cStatus = MI_ERR;
    u8 ucIrqEn   = 0x00;
    u8 ucWaitFor = 0x00;
    u8 ucLastBits;
    u8 ucN;
    u32 ul;

    switch ( ucCommand )
    {
    case PCD_AUTHENT:		//Mifare??
        ucIrqEn   = 0x12;		//????????ErrIEn  ??????IdleIEn
        ucWaitFor = 0x10;		//???????? ?????????
        break;

    case PCD_TRANSCEIVE:		//???? ????
        ucIrqEn   = 0x77;		//??TxIEn RxIEn IdleIEn LoAlertIEn ErrIEn TimerIEn
        ucWaitFor = 0x30;		//?????? ?????????? ???????
        break;

    default:
        break;

    }

    WriteRawRC ( ComIEnReg, ucIrqEn | 0x80 );		//IRqInv????IRQ?Status1Reg?IRq?????
    ClearBitMask ( ComIrqReg, 0x80 );			//Set1?????,CommIRqReg??????
    WriteRawRC ( CommandReg, PCD_IDLE );		//?????
    SetBitMask ( FIFOLevelReg, 0x80 );			//??FlushBuffer????FIFO????????ErrReg?BufferOvfl??????

    for ( ul = 0; ul < ucInLenByte; ul ++ )
        WriteRawRC ( FIFODataReg, pInData [ ul ] );    		//????FIFOdata

    WriteRawRC ( CommandReg, ucCommand );					//???


    if ( ucCommand == PCD_TRANSCEIVE )
        SetBitMask(BitFramingReg,0x80);  				//StartSend???????? ?????????????

    ul = 1000;//????????,??M1???????25ms

    do 														//?? ???????
    {
        ucN = ReadRawRC ( ComIrqReg );							//??????
        ul --;
    } while ( ( ul != 0 ) && ( ! ( ucN & 0x01 ) ) && ( ! ( ucN & ucWaitFor ) ) );		//????i=0,?????,??????

    ClearBitMask ( BitFramingReg, 0x80 );					//????StartSend?

    if ( ul != 0 )
    {
        if ( ! (( ReadRawRC ( ErrorReg ) & 0x1B )) )			//????????BufferOfI CollErr ParityErr ProtocolErr
        {
            cStatus = MI_OK;

            if ( ucN & ucIrqEn & 0x01 )					//?????????
                cStatus = MI_NOTAGERR;

            if ( ucCommand == PCD_TRANSCEIVE )
            {
                ucN = ReadRawRC ( FIFOLevelReg );			//?FIFO???????

                ucLastBits = ReadRawRC ( ControlReg ) & 0x07;	//?????????????

                if ( ucLastBits )
                    * pOutLenBit = ( ucN - 1 ) * 8 + ucLastBits;   	//N??????1(??????)+??????? ?????????
                else
                    * pOutLenBit = ucN * 8;   					//??????????????

                if ( ucN == 0 )
                    ucN = 1;

                if ( ucN > MAXRLEN )
                    ucN = MAXRLEN;

                for ( ul = 0; ul < ucN; ul ++ )
                    pOutData [ ul ] = ReadRawRC ( FIFODataReg );
            }
        }
        else
            cStatus = MI_ERR;
//			printf(ErrorReg);
    }

    SetBitMask ( ControlReg, 0x80 );           // stop timer now
    WriteRawRC ( CommandReg, PCD_IDLE );

    return cStatus;

}


/*
 * ???:PcdRequest
 * ??  :??
 * ??  :ucReq_code,????
 *                     = 0x52,?????????14443A????
 *                     = 0x26,??????????
 *         pTagType,??????
 *                   = 0x4400,Mifare_UltraLight
 *                   = 0x0400,Mifare_One(S50)
 *                   = 0x0200,Mifare_One(S70)
 *                   = 0x0800,Mifare_Pro(X))
 *                   = 0x4403,Mifare_DESFire
 * ??  : ???
 *         = MI_OK,??
 * ??  :????
 */
char PcdRequest ( u8 ucReq_code, u8 * pTagType )
{
    char cStatus;
    u8 ucComMF522Buf [ MAXRLEN ];
    u32 ulLen;

    ClearBitMask ( Status2Reg, 0x08 );	//????MIFARECyptol????????????????????
    WriteRawRC ( BitFramingReg, 0x07 );	//	?????????? ??
    SetBitMask ( TxControlReg, 0x03 );	//TX1,TX2???????????????13.56???????

    ucComMF522Buf [ 0 ] = ucReq_code;		//?? ?????

    cStatus = PcdComMF522 ( PCD_TRANSCEIVE,	ucComMF522Buf, 1, ucComMF522Buf, & ulLen );	//??

    if ( ( cStatus == MI_OK ) && ( ulLen == 0x10 ) )	//?????????
    {
        * pTagType = ucComMF522Buf [ 0 ];
        * ( pTagType + 1 ) = ucComMF522Buf [ 1 ];
    }

    else
        cStatus = MI_ERR;

    return cStatus;

}


/*
 * ???:PcdAnticoll
 * ??  :???
 * ??  :pSnr,?????,4??
 * ??  : ???
 *         = MI_OK,??
 * ??  :????
 */
char PcdAnticoll ( u8 * pSnr )
{
    char cStatus;
    u8 uc, ucSnr_check = 0;
    u8 ucComMF522Buf [ MAXRLEN ];
    u32 ulLen;

    ClearBitMask ( Status2Reg, 0x08 );		//?MFCryptol On? ??????MFAuthent???,??????
    WriteRawRC ( BitFramingReg, 0x00);		//????? ????
    ClearBitMask ( CollReg, 0x80 );			//?ValuesAfterColl?????????????

    ucComMF522Buf [ 0 ] = 0x93;	//???????
    ucComMF522Buf [ 1 ] = 0x20;

    cStatus = PcdComMF522 ( PCD_TRANSCEIVE, ucComMF522Buf, 2, ucComMF522Buf, & ulLen);//?????

    if ( cStatus == MI_OK)		//????
    {
        for ( uc = 0; uc < 4; uc ++ )
        {
            * ( pSnr + uc )  = ucComMF522Buf [ uc ];			//??UID
            ucSnr_check ^= ucComMF522Buf [ uc ];
        }

        if ( ucSnr_check != ucComMF522Buf [ uc ] )
            cStatus = MI_ERR;

    }

    SetBitMask ( CollReg, 0x80 );

    return cStatus;

}


/*
 * ???:CalulateCRC
 * ??  :?RC522??CRC16
 * ??  :pIndata,??CRC16???
 *         ucLen,??CRC16???????
 *         pOutData,????????????
 * ??  : ?
 * ??  :????
 */
void CalulateCRC ( u8 * pIndata, u8 ucLen, u8 * pOutData )
{
    u8 uc, ucN;

    ClearBitMask(DivIrqReg,0x04);

    WriteRawRC(CommandReg,PCD_IDLE);

    SetBitMask(FIFOLevelReg,0x80);

    for ( uc = 0; uc < ucLen; uc ++)
        WriteRawRC ( FIFODataReg, * ( pIndata + uc ) );

    WriteRawRC ( CommandReg, PCD_CALCCRC );

    uc = 0xFF;

    do
    {
        ucN = ReadRawRC ( DivIrqReg );
        uc --;
    } while ( ( uc != 0 ) && ! ( ucN & 0x04 ) );

    pOutData [ 0 ] = ReadRawRC ( CRCResultRegL );
    pOutData [ 1 ] = ReadRawRC ( CRCResultRegM );

}


/*
 * ???:PcdSelect
 * ??  :????
 * ??  :pSnr,?????,4??
 * ??  : ???
 *         = MI_OK,??
 * ??  :????
 */
char PcdSelect ( u8 * pSnr )
{
    char ucN;
    u8 uc;
    u8 ucComMF522Buf [ MAXRLEN ];
    u32  ulLen;

    ucComMF522Buf [ 0 ] = PICC_ANTICOLL1;
    ucComMF522Buf [ 1 ] = 0x70;
    ucComMF522Buf [ 6 ] = 0;

    for ( uc = 0; uc < 4; uc ++ )
    {
        ucComMF522Buf [ uc + 2 ] = * ( pSnr + uc );
        ucComMF522Buf [ 6 ] ^= * ( pSnr + uc );
    }

    CalulateCRC ( ucComMF522Buf, 7, & ucComMF522Buf [ 7 ] );

    ClearBitMask ( Status2Reg, 0x08 );

    ucN = PcdComMF522 ( PCD_TRANSCEIVE, ucComMF522Buf, 9, ucComMF522Buf, & ulLen );

    if ( ( ucN == MI_OK ) && ( ulLen == 0x18 ) )
        ucN = MI_OK;
    else
        ucN = MI_ERR;

    return ucN;

}


/*
 * ???:PcdAuthState
 * ??  :??????
 * ??  :ucAuth_mode,??????
 *                     = 0x60,??A??
 *                     = 0x61,??B??
 *         u8 ucAddr,???
 *         pKey,??
 *         pSnr,?????,4??
 * ??  : ???
 *         = MI_OK,??
 * ??  :????
 */
char PcdAuthState ( u8 ucAuth_mode, u8 ucAddr, u8 * pKey, u8 * pSnr )
{
    char cStatus;
    u8 uc, ucComMF522Buf [ MAXRLEN ];
    u32 ulLen;

    ucComMF522Buf [ 0 ] = ucAuth_mode;
    ucComMF522Buf [ 1 ] = ucAddr;

    for ( uc = 0; uc < 6; uc ++ )
        ucComMF522Buf [ uc + 2 ] = * ( pKey + uc );

    for ( uc = 0; uc < 6; uc ++ )
        ucComMF522Buf [ uc + 8 ] = * ( pSnr + uc );

    cStatus = PcdComMF522 ( PCD_AUTHENT, ucComMF522Buf, 12, ucComMF522Buf, & ulLen );

    if ( ( cStatus != MI_OK ) || ( ! ( ReadRawRC ( Status2Reg ) & 0x08 ) ) )

    {
//			if(cStatus != MI_OK)
//					printf("666")	;
//			else
//				printf("888");
        cStatus = MI_ERR;
    }

    return cStatus;

}


/*
 * ???:PcdWrite
 * ??  :????M1???
 * ??  :u8 ucAddr,???
 *         pData,?????,16??
 * ??  : ???
 *         = MI_OK,??
 * ??  :????
 */
char PcdWrite ( u8 ucAddr, u8 * pData )
{
    char cStatus;
    u8 uc, ucComMF522Buf [ MAXRLEN ];
    u32 ulLen;

    ucComMF522Buf [ 0 ] = PICC_WRITE;
    ucComMF522Buf [ 1 ] = ucAddr;

    CalulateCRC ( ucComMF522Buf, 2, & ucComMF522Buf [ 2 ] );

    cStatus = PcdComMF522 ( PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, & ulLen );

    if ( ( cStatus != MI_OK ) || ( ulLen != 4 ) || ( ( ucComMF522Buf [ 0 ] & 0x0F ) != 0x0A ) )
        cStatus = MI_ERR;

    if ( cStatus == MI_OK )
    {
        memcpy(ucComMF522Buf, pData, 16);
        for ( uc = 0; uc < 16; uc ++ )
            ucComMF522Buf [ uc ] = * ( pData + uc );

        CalulateCRC ( ucComMF522Buf, 16, & ucComMF522Buf [ 16 ] );

        cStatus = PcdComMF522 ( PCD_TRANSCEIVE, ucComMF522Buf, 18, ucComMF522Buf, & ulLen );

        if ( ( cStatus != MI_OK ) || ( ulLen != 4 ) || ( ( ucComMF522Buf [ 0 ] & 0x0F ) != 0x0A ) )
            cStatus = MI_ERR;

    }
    return cStatus;
}


/*
 * ???:PcdRead
 * ??  :??M1?????
 * ??  :u8 ucAddr,???
 *         pData,?????,16??
 * ??  : ???
 *         = MI_OK,??
 * ??  :????
 */
char PcdRead ( u8 ucAddr, u8 * pData )
{
    char cStatus;
    u8 uc, ucComMF522Buf [ MAXRLEN ];
    u32 ulLen;

    ucComMF522Buf [ 0 ] = PICC_READ;
    ucComMF522Buf [ 1 ] = ucAddr;

    CalulateCRC ( ucComMF522Buf, 2, & ucComMF522Buf [ 2 ] );

    cStatus = PcdComMF522 ( PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, & ulLen );

    if ( ( cStatus == MI_OK ) && ( ulLen == 0x90 ) )
    {
        for ( uc = 0; uc < 16; uc ++ )
            * ( pData + uc ) = ucComMF522Buf [ uc ];
    }

    else
        cStatus = MI_ERR;

    return cStatus;

}


/*
 * ???:PcdHalt
 * ??  :??????????
 * ??  :?
 * ??  : ???
 *         = MI_OK,??
 * ??  :????
 */
char PcdHalt( void )
{
    u8 ucComMF522Buf [ MAXRLEN ];
    u32  ulLen;

    ucComMF522Buf [ 0 ] = PICC_HALT;
    ucComMF522Buf [ 1 ] = 0;

    CalulateCRC ( ucComMF522Buf, 2, & ucComMF522Buf [ 2 ] );
    PcdComMF522 ( PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, & ulLen );

    return MI_OK;

}


void IC_CMT ( u8 * UID, u8 * KEY, u8 RW, u8 * Dat )
{
    u8 ucArray_ID [ 4 ] = { 0 };//????IC?????UID(IC????)

    PcdRequest ( 0x52, ucArray_ID );//??

    PcdAnticoll ( ucArray_ID );//???

    PcdSelect ( UID );//???

    PcdAuthState ( 0x60, 0x10, KEY, UID );//??

    if ( RW )//????,1??,0??
        PcdRead ( 0x10, Dat );

    else
        PcdWrite ( 0x10, Dat );

    PcdHalt ();

}

void ShowID(u8 *p)	 //??????,???????
{
    u8 num[9];
    u8 i;

    for(i=0; i<4; i++)
    {
        num[i*2]=p[i]/16;
        num[i*2]>9?(num[i*2]+='7'):(num[i*2]+='0');
        num[i*2+1]=p[i]%16;
        num[i*2+1]>9?(num[i*2+1]+='7'):(num[i*2+1]+='0');
    }
    num[8]=0;
    printf("ID>>>%s\r\n", num);
}


