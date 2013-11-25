#include "CC2500.h"
#include "cmsis_os.h"

/** @defgroup STM32F4_DISCOVERY_CC2500_Private_Defines
  * @{
  */
	
__IO uint32_t  CC2500Timeout = CC2500_FLAG_TIMEOUT;

/* Read/Write command */
#define READWRITE_CMD              ((uint8_t)0x80) 
/* Multiple byte read/write command */ 
#define MULTIPLEBYTE_CMD           ((uint8_t)0x40)
/* Dummy Byte Send by the SPI Master device in order to generate the Clock to the Slave device */
#define DUMMY_BYTE                 ((uint8_t)0x00)

// RF_SETTINGS rfSettings{
// 	
// };


//Local functions
static void CC2500_LowLevel_Init(void);
static uint8_t CC2500_SendByte(uint8_t byte);
static uint8_t tmp_data[12];
static uint8_t tmp_data_RX[3];
static uint8_t FSM_state;
static uint8_t FSM_buffer_space;
static int i;

/**
  * @brief  Set CC2500 Initialization.
  * @param  None
  * @retval None
  */
void CC2500_Init()
{
  uint8_t ctrl = 0x00;
  
  /* Configure the low level interface ---------------------------------------*/
  CC2500_LowLevel_Init();
  
  /* Configure MEMS: data rate, power mode, full scale, self test and axes 
  ctrl = (uint8_t) (LIS302DL_InitStruct->Output_DataRate | LIS302DL_InitStruct->Power_Mode | \
                    LIS302DL_InitStruct->Full_Scale | LIS302DL_InitStruct->Self_Test | \
                    LIS302DL_InitStruct->Axes_Enable);
  

  LIS302DL_Write(&ctrl, LIS302DL_CTRL_REG1_ADDR, 1);  */
}


void CC2500_config_transmitter(){
	uint8_t tmp;
	
	// FSCTRL1   Frequency synthesizer control.
	//Frequency offset = 457kHz
	tmp = CC2500_CFG_FSCTRL1;
	CC2500_Write(&tmp, CC2500_REG_FSCTRL1, 1);
	
	// FSCTRL0   Frequency synthesizer control.
	tmp = CC2500_CFG_FSCTRL0;
	CC2500_Write(&tmp, CC2500_REG_FSCTRL0, 1);
	
	// FREQ2     Frequency control word, high byte.
	tmp = CC2500_CFG_FREQ2;
	CC2500_Write(&tmp, CC2500_REG_FREQ2, 1);	
	
	// FREQ1     Frequency control word, middle byte.
	tmp = CC2500_CFG_FREQ1;
	CC2500_Write(&tmp, CC2500_REG_FREQ1, 1);	
	
	// FREQ0     Frequency control word, low byte.
	tmp = CC2500_CFG_FREQ0;
	CC2500_Write(&tmp, CC2500_REG_FREQ0, 1);
	
	// MDMCFG4   Modem configuration.
	//BW of channel = 541.666kHz
	tmp = CC2500_CFG_MDMCFG4;
	CC2500_Write(&tmp, CC2500_REG_MDMCFG4, 1);
	
	// MDMCFG3   Modem configuration.
	// Baud Rate = 125kb,
	tmp = CC2500_CFG_MDMCFG3;
	CC2500_Write(&tmp, CC2500_REG_MDMCFG3, 1);
	
	// MDMCFG2   Modem configuration.
	//Disable digital DC blocking filter before demodulator, MSK modulation, 16/16 sync word bits detected
	tmp = CC2500_CFG_MDMCFG2;
	CC2500_Write(&tmp, CC2500_REG_MDMCFG2, 1);
	
	// MDMCFG1   Modem configuration.
	tmp = CC2500_CFG_MDMCFG1;
	CC2500_Write(&tmp, CC2500_REG_MDMCFG1, 1);
	
	// MDMCFG0   Modem configuration.
	// Default Channel Spacing of 200kHz
	tmp = CC2500_CFG_MDMCFG0;
	CC2500_Write(&tmp, CC2500_REG_MDMCFG0, 1);
	
	// CHANNR    Channel number.
	//Channel 0
	tmp = CC2500_CFG_CHANNR;
	CC2500_Write(&tmp, CC2500_REG_CHANNR, 1);
	
	// DEVIATN   Modem deviation setting (when FSK modulation is enabled).
	// 1785kHz
	tmp = CC2500_CFG_DEVIATN;
	CC2500_Write(&tmp, CC2500_REG_DEVIATN, 1);
	
	// FREND1    Front end RX configuration.
	tmp = CC2500_CFG_FREND1;
	CC2500_Write(&tmp, CC2500_REG_FREND1, 1);
	
	// FREND0    Front end RX configuration.
	tmp = CC2500_CFG_FREND0;
	CC2500_Write(&tmp, CC2500_REG_FREND0, 1);

	// MCSM0     Main Radio Control State Machine configuration.
	// Automatically calibrate When going from IDLE to RX or TX (or FSTXON) check CC2500_CFG datasheet
	tmp = CC2500_CFG_MCSM0;
	CC2500_Write(&tmp, CC2500_REG_MCSM0, 1);
	
	// FOCCFG    Frequency Offset Compensation Configuration.
	tmp = CC2500_CFG_FOCCFG;
	CC2500_Write(&tmp, CC2500_REG_FOCCFG, 1);	
	
	// BSCFG     Bit synchronization Configuration.
	tmp = CC2500_CFG_BSCFG;
	CC2500_Write(&tmp, CC2500_REG_BSCFG, 1);	
	
	// AGCCTRL2  AGC control.
	tmp = CC2500_CFG_AGCCTRL2;
	CC2500_Write(&tmp, CC2500_REG_AGCCTRL2, 1);
	
	// AGCCTRL1  AGC control.
	tmp = CC2500_CFG_AGCCTRL1;
	CC2500_Write(&tmp, CC2500_REG_AGCCTRL1, 1);
	
	// AGCCTRL0  AGC control.
	tmp = CC2500_CFG_AGCCTRL0;
	CC2500_Write(&tmp, CC2500_REG_AGCCTRL0, 1);
	
	// FSCAL3    Frequency synthesizer calibration.
	tmp = CC2500_CFG_FSCAL3;
	CC2500_Write(&tmp, CC2500_REG_FSCAL3, 1);
	
	// FSCAL2    Frequency synthesizer calibration.
	tmp = CC2500_CFG_FSCAL2;
	CC2500_Write(&tmp, CC2500_REG_FSCAL2, 1);
	
	// FSCAL1    Frequency synthesizer calibration.
	tmp = CC2500_CFG_FSCAL1;
	CC2500_Write(&tmp, CC2500_REG_FSCAL1, 1);
	
	// FSCAL0    Frequency synthesizer calibration.
	tmp = CC2500_CFG_FSCAL0;
	CC2500_Write(&tmp, CC2500_REG_FSCAL0, 1);
	
	// FSTEST    Frequency synthesizer calibration.
	tmp = CC2500_CFG_FSTEST;
	CC2500_Write(&tmp, CC2500_REG_FSTEST, 1);
	
	
	// TEST2     Various test settings.
	tmp = CC2500_CFG_TEST2;
	CC2500_Write(&tmp, CC2500_REG_TEST2, 1);
	
	// TEST1     Various test settings.
	tmp = CC2500_CFG_TEST1;
	CC2500_Write(&tmp, CC2500_REG_TEST1, 1);
	
	// TEST0     Various test settings.
	tmp = CC2500_CFG_TEST0;
	CC2500_Write(&tmp, CC2500_REG_TEST0, 1);
	
	// FIFOTHR   RXFIFO and TXFIFO thresholds.
	tmp = CC2500_CFG_FIFOTHR;
	CC2500_Write(&tmp, CC2500_REG_FIFOTHR, 1);
	
	/*---- I/O Config ----*/
	// IOCFG2    GDO2 output pin configuration.
	tmp = CC2500_CFG_IOCFG2;
	CC2500_Write(&tmp, CC2500_REG_IOCFG2, 1);
	
	// IOCFG0D   GDO0 output pin configuration.
	tmp = CC2500_CFG_IOCFG0D;
	CC2500_Write(&tmp, CC2500_REG_IOCFG0, 1);
	
	
	// PKTCTRL1  Packet automation control.
	tmp = CC2500_CFG_PKTCTRL1;
	CC2500_Write(&tmp, CC2500_REG_PKTCTRL1, 1);
	
	// PKTCTRL0  Packet automation control.
	// Fixed Packet Length (0x05)
	tmp = CC2500_CFG_PKTCTRL0;
	CC2500_Write(&tmp, CC2500_REG_PKTCTRL0, 1);
	
	// ADDR      Device address.
	// Global Broadcast Address
	tmp = CC2500_CFG_ADDR;
	CC2500_Write(&tmp, CC2500_REG_ADDR, 1);
	
	// PKTLEN    Packet length.
	 // Packet Length of 10bytes
	tmp = CC2500_CFG_PKTLEN;
	CC2500_Write(&tmp, CC2500_REG_PKTLEN, 1);
	
	// PKTLEN    Packet length.
	 // Packet Length of 10bytes
	tmp = CC2500_CFG_MCSM1;
	CC2500_Write(&tmp, CC2500_REG_MCSM1, 1);
}




/**
  * @brief  Writes one byte to the CC2500.
  * @param  pBuffer : pointer to the buffer  containing the data to be written to the CC2500.
  * @param  WriteAddr : CC2500's internal address to write to.
  * @param  NumByteToWrite: Number of bytes to write.
  * @retval None
  */
void CC2500_Write(uint8_t* pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite)
{
  /* Configure the MS bit: 
       - When 0, the address will remain unchanged in multiple read/write commands (single).
       - When 1, the address will be auto incremented in multiple read/write commands (burst).
  */
  if(NumByteToWrite > 0x01)
  {
    WriteAddr |= (uint8_t)MULTIPLEBYTE_CMD;
  }
  /* Set chip select Low at the start of the transmission */
  CC2500_CS_LOW();
  
  /* Send the Address of the indexed register */
  CC2500_SendByte(WriteAddr);
  /* Send the data that will be written into the device (MSB First) */
  while(NumByteToWrite >= 0x01)
  {
    CC2500_SendByte(*pBuffer);
    NumByteToWrite--;
    pBuffer++;
  }
  
  /* Set chip select High at the end of the transmission */ 
  CC2500_CS_HIGH();
}

/**
  * @brief  Reads a block of data from the CC2500.
  * @param  pBuffer : pointer to the buffer that receives the data read from the CC2500.
  * @param  ReadAddr : CC2500's internal address to read from.
  * @param  NumByteToRead : number of bytes to read from the CC2500.
  * @retval None
  */
void CC2500_Read(uint8_t* pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead)
{  
  if(NumByteToRead > 0x01)
  {
		//differentiate command strobes from status registers using burst mode
		//ie. must use burst mode to read status registers however chip will
		//only return single register (single byte access for status registers)
		//as a result, we check to see if the address is in the status register range,
		//if so, keep the burst bit so the chip interprets the right instruction,
		//but modify NumByteToRead to only expect single byte return
		if((ReadAddr >= 0x30) && (ReadAddr <= 0x3D)){  //address limits for status registers
			NumByteToRead = 0x01;
		}
		
		//0 is single op, 1 is burst
    ReadAddr |= (uint8_t)(READWRITE_CMD | MULTIPLEBYTE_CMD);

  }
  else
  {
    ReadAddr |= (uint8_t)READWRITE_CMD;
  }
  /* Set chip select Low at the start of the transmission */
  CC2500_CS_LOW();
  
  /* Send the Address of the indexed register */
  *pBuffer = CC2500_SendByte(ReadAddr);
  
  /* Receive the data that will be read from the device (MSB First) */
  while(NumByteToRead > 0x00)
  {
    /* Send dummy byte (0x00) to generate the SPI clock to LIS302DL (Slave device) */
    *pBuffer = CC2500_SendByte(DUMMY_BYTE);
    NumByteToRead--;
    pBuffer++;
  }
  
  /* Set chip select High at the end of the transmission */ 
  CC2500_CS_HIGH();
}



/**
  * @brief  Initializes the low level interface used to drive the CC2500
  * @param  None
  * @retval None
  */
static void CC2500_LowLevel_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  SPI_InitTypeDef  SPI_InitStructure;

  /* Enable the SPI periph */
  RCC_APB1PeriphClockCmd(CC2500_SPI_CLK, ENABLE);

  /* Enable SCK, MOSI and MISO GPIO clocks */
  RCC_AHB1PeriphClockCmd(CC2500_SPI_SCLK_GPIO_CLK | CC2500_SPI_MISO_GPIO_CLK | CC2500_SPI_MOSI_GPIO_CLK, ENABLE);

  /* Enable CS  GPIO clock */
  RCC_AHB1PeriphClockCmd(CC2500_SPI_CS_GPIO_CLK, ENABLE);
  
  /* Enable INT1 GPIO clock */
  //RCC_AHB1PeriphClockCmd(LIS302DL_SPI_INT1_GPIO_CLK, ENABLE);
  
  /* Enable INT2 GPIO clock */
  //RCC_AHB1PeriphClockCmd(LIS302DL_SPI_INT2_GPIO_CLK, ENABLE);

  GPIO_PinAFConfig(CC2500_SPI_SCLK_GPIO_PORT, CC2500_SPI_SCLK_SOURCE, CC2500_SPI_SCLK_AF);
  GPIO_PinAFConfig(CC2500_SPI_MISO_GPIO_PORT, CC2500_SPI_MISO_SOURCE, CC2500_SPI_MISO_AF);
  GPIO_PinAFConfig(CC2500_SPI_MOSI_GPIO_PORT, CC2500_SPI_MOSI_SOURCE, CC2500_SPI_MOSI_AF);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
  //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  /* SPI SCK pin configuration */
  GPIO_InitStructure.GPIO_Pin = CC2500_SPI_SCLK_PIN;
  GPIO_Init(CC2500_SPI_SCLK_GPIO_PORT, &GPIO_InitStructure);

  /* SPI  MOSI pin configuration */
  GPIO_InitStructure.GPIO_Pin =  CC2500_SPI_MOSI_PIN;
  GPIO_Init(CC2500_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

  /* SPI MISO pin configuration */
  GPIO_InitStructure.GPIO_Pin = CC2500_SPI_MISO_PIN;
  GPIO_Init(CC2500_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);
	
	  /* Configure GPIO PIN for Lis Chip select */
  GPIO_InitStructure.GPIO_Pin = CC2500_SPI_CS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	//GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_Init(CC2500_SPI_CS_GPIO_PORT, &GPIO_InitStructure);

  /* Deselect : Chip Select high */
  GPIO_SetBits(CC2500_SPI_CS_GPIO_PORT, CC2500_SPI_CS_PIN);

  /* SPI configuration -------------------------------------------------------*/
	SPI_I2S_DeInit(CC2500_SPI);
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
 // SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_Init(CC2500_SPI, &SPI_InitStructure);

  /* Enable SPI1  */
  SPI_Cmd(CC2500_SPI, ENABLE);

  
  /* Configure GPIO PINs to detect Interrupts (Not applicable in the case of CC2500 */
/*  GPIO_InitStructure.GPIO_Pin = LIS302DL_SPI_INT1_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(LIS302DL_SPI_INT1_GPIO_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = LIS302DL_SPI_INT2_PIN;
  GPIO_Init(LIS302DL_SPI_INT2_GPIO_PORT, &GPIO_InitStructure);   */
}
/**
  * @brief  Sends command strobe to the CC2500.
  * @param  cmd representing command strobe to be issued (see CC2500 UM page 60, table 37).
  * @param  Pointer to the current state of the CC2500(before strobe was issued).
	* @param  Pointer to number of free bytes in the CC2500 FIFO(before strobe was issued).
  * @retval None
  */
void CC2500_StrobeSend(uint8_t cmd, uint8_t* state, uint8_t* buffer_space)
{
	uint8_t chip_status =0;
	
  /* Set chip select Low at the start of the transmission */
  CC2500_CS_LOW();
  
  /* Send the command and receive chip ststus byte, and extract the state (bytes 6:4) */
  chip_status = CC2500_SendByte(cmd);

  /* Set chip select High at the end of the transmission */ 
  CC2500_CS_HIGH();
	
 //	osDelay(10);
 //	CC2500_CS_LOW();
//   
//   /* Send the command and receive chip ststus byte, and extract the state (bytes 6:4) */
  // chip_status = CC2500_SendByte(DUMMY_BYTE);
//   
//   /* Set chip select High at the end of the transmission */ 
  // CC2500_CS_HIGH();
	
	*state = chip_status & 0x70;
	*buffer_space = chip_status & 0x0F;
	
}

/**
  * @brief  Sends a Byte through the SPI interface and return the Byte received 
  *         from the SPI bus.
  * @param  Byte : Byte send.
  * @retval The received byte value
  */
static uint8_t CC2500_SendByte(uint8_t byte)
{
  /* Loop while DR register in not emplty */
  CC2500Timeout = CC2500_FLAG_TIMEOUT;
  while (SPI_I2S_GetFlagStatus(CC2500_SPI, SPI_I2S_FLAG_TXE) == RESET)
  {
    if((CC2500Timeout--) == 0) return CC2500_TIMEOUT_UserCallback();
  }
  
  /* Send a Byte through the SPI peripheral */
  SPI_I2S_SendData(CC2500_SPI, byte);
	
	while(SPI_I2S_GetFlagStatus(CC2500_SPI, SPI_I2S_FLAG_TXE) == RESET); // wait until transmit complete
  
  /* Wait to receive a Byte */
  CC2500Timeout = CC2500_FLAG_TIMEOUT;
  while (SPI_I2S_GetFlagStatus(CC2500_SPI, SPI_I2S_FLAG_RXNE) == RESET)
  {
    if((CC2500Timeout--) == 0) return CC2500_TIMEOUT_UserCallback();
  }
  while(SPI_I2S_GetFlagStatus(CC2500_SPI, SPI_I2S_FLAG_BSY) == SET); // wait until not busy
  /* Return the Byte read from the SPI bus */
  return (uint8_t)SPI_I2S_ReceiveData(CC2500_SPI);
}

//#ifdef USE_DEFAULT_TIMEOUT_CALLBACK
/**
  * @brief  Basic management of the timeout situation.
  * @param  None.
  * @retval None.
  */
uint32_t CC2500_TIMEOUT_UserCallback(void)
{
  /* Block communication and all processes */
  while (1)
  {   
  }
}

void Wireless_TX(uint8_t * data){
	
// 	//go to idle state
// 	CC2500_StrobeSend(SIDLE_T,&FSM_state,&FSM_buffer_space);
// 	osDelay(1000);
// 	CC2500_StrobeSend(SNOP_T,&FSM_state,&FSM_buffer_space);
	
	//Copy data for TMR
	for(i = 0; i < 4; i++){
		tmp_data[3*i] = data[i];
		tmp_data[3*i + 1] = data[i];
		tmp_data[3*i + 2] = data[i];
	}
	 
	
// 	//Flush TX buffer
// 	CC2500_StrobeSend(SFTX_T,&FSM_state,&FSM_buffer_space);
// 	osDelay(1000);
// 	CC2500_StrobeSend(SNOP_T,&FSM_state,&FSM_buffer_space);
	
	CC2500_Write(tmp_data, 0x3F, 12);
	
// 	osDelay(1000);
// 	CC2500_StrobeSend(STX_T,&FSM_state,&FSM_buffer_space);
// 	osDelay(1000);
// 	CC2500_StrobeSend(SNOP_T,&FSM_state,&FSM_buffer_space);
// 	osDelay(1000);
}

void Wireless_RX(uint8_t *data){
	
	CC2500_StrobeSend(SNOP_R,&FSM_state,&FSM_buffer_space);
	
	//wait until theres a packet to be read
	while (FSM_buffer_space < 12){
		CC2500_StrobeSend(SNOP_R,&FSM_state,&FSM_buffer_space);	
	}
	
	//read packet one byte at a time and do TMR
	for(i = 0; i < 4; i++){
		CC2500_Read(tmp_data_RX, 0x3F, 3);
		data[i] = ((tmp_data_RX[0]& tmp_data_RX[1])|(tmp_data_RX[0]& tmp_data_RX[2])|(tmp_data_RX[1]&tmp_data_RX[2]));		
	}
	
}

void goToRX(void) {
	CC2500_StrobeSend(SIDLE_R,&FSM_state,&FSM_buffer_space);
	osDelay(CONFIGURE_SLEEP_TIME);
	
	CC2500_StrobeSend(SFRX_R,&FSM_state,&FSM_buffer_space);
	osDelay(CONFIGURE_SLEEP_TIME);
	
	CC2500_StrobeSend(SIDLE_R,&FSM_state,&FSM_buffer_space);
	osDelay(CONFIGURE_SLEEP_TIME);
	
	CC2500_StrobeSend(SCAL_R,&FSM_state,&FSM_buffer_space);
	osDelay(CONFIGURE_SLEEP_TIME);

	CC2500_StrobeSend(SRX_R,&FSM_state,&FSM_buffer_space);
	osDelay(CONFIGURE_SLEEP_TIME);
}

void goToTX(void) {
	CC2500_StrobeSend(SRES_T,&FSM_state,&FSM_buffer_space);
 	osDelay(CONFIGURE_SLEEP_TIME);
	
	CC2500_StrobeSend(SFTX_T,&FSM_state,&FSM_buffer_space);
	osDelay(CONFIGURE_SLEEP_TIME);
	
	CC2500_StrobeSend(SIDLE_T,&FSM_state,&FSM_buffer_space);
	osDelay(CONFIGURE_SLEEP_TIME);
	
	CC2500_StrobeSend(SIDLE_T,&FSM_state,&FSM_buffer_space);
	osDelay(CONFIGURE_SLEEP_TIME);
	
	CC2500_StrobeSend(SCAL_T,&FSM_state,&FSM_buffer_space);
	osDelay(CONFIGURE_SLEEP_TIME);
}