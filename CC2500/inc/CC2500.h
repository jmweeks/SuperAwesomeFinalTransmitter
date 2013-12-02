/***************************************************************
 *  SmartRF Studio(tm) Export
 *
 *  Radio register settings specifed with C-code
 *  compatible #define statements.
 *
 ***************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef CC2500
#define CC2500

#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "cmsis_os.h"

#include "project_receiver.h"
#include "project_transmitter.h"

/* CC2500 structs */



#define CC2500_FLAG_TIMEOUT         ((uint32_t)0x1000)

/**
  * @brief  CC2500 SPI Interface pins
  */
	
#define CC2500_SPI                       	SPI2									
#define CC2500_SPI_CLK                   	RCC_APB1Periph_SPI2

#define	CC2500_SPI_SCLK_PIN								GPIO_Pin_13
#define	CC2500_SPI_SCLK_GPIO_PORT					GPIOB
#define	CC2500_SPI_SCLK_GPIO_CLK					RCC_AHB1Periph_GPIOB
#define	CC2500_SPI_SCLK_SOURCE						GPIO_PinSource13
#define	CC2500_SPI_SCLK_AF								GPIO_AF_SPI2

#define CC2500_SPI_MISO_PIN              GPIO_Pin_14          
#define CC2500_SPI_MISO_GPIO_PORT        GPIOB               
#define CC2500_SPI_MISO_GPIO_CLK         RCC_AHB1Periph_GPIOB
#define CC2500_SPI_MISO_SOURCE           GPIO_PinSource14
#define CC2500_SPI_MISO_AF               GPIO_AF_SPI2

#define CC2500_SPI_MOSI_PIN              GPIO_Pin_15          
#define CC2500_SPI_MOSI_GPIO_PORT        GPIOB              
#define CC2500_SPI_MOSI_GPIO_CLK         RCC_AHB1Periph_GPIOB
#define CC2500_SPI_MOSI_SOURCE           GPIO_PinSource15
#define CC2500_SPI_MOSI_AF               GPIO_AF_SPI2

#define CC2500_SPI_CS_PIN                GPIO_Pin_11
#define CC2500_SPI_CS_GPIO_PORT          GPIOB
#define CC2500_SPI_CS_GPIO_CLK           RCC_AHB1Periph_GPIOB  


/******************************************************************************/
/*************************** START REGISTER MAPPING  **************************/
/******************************************************************************/


#define CC2500_REG_IOCFG2 			0x00 
/*IOCFG2
Bit		Description
7			Reserved		
6			Invert output, i.e. select active low (1) / high (0)
5:0		Default is CHIP_RDYn
*/
#define CC2500_REG_IOCFG1				0x01 

#define CC2500_REG_IOCFG0 			0x02 
/*IOCFG
Bit 			Field Name 					Description
7 				TEMP_SENSOR_ENABLE 	Enable analog temperature sensor. Write 0 in all other
															register bits when using temperature sensor.
6 				GDO0_INV 						Invert output, i.e. select active low (1) / high (0)
5:0 			GDO0_CFG[5:0] 			Default is CLK_XOSC/192 (see Table 33 on page 53 of Datasheet).
*/
#define CC2500_REG_FIFOTHR			0x03 
/*FIFOTHR
Bit		Description
7:4		Reserved
3:0		Set threshold for TX FIFO and RX FIFO
			Setting 						Bytes in TX FIFO 					Bytes in RX FIFO
				0 	(0000) 					61					 							4
				1 	(0001) 					57 												8
				2 	(0010) 					53 												12
				3 	(0011) 					49 												16
				4 	(0100) 					45 												20
				5 	(0101) 					41 												24
				6 	(0110) 					37 												28
				7 	(0111) 					33 												32
				8 	(1000) 					29 												36
				9 	(1001) 					25 												40
				10 	(1010) 					21 												44
				11 	(1011) 					17 												48
				12 	(1100) 					13 												52
				13 	(1101) 					9 												56
				14 	(1110) 					5 												60
				15 	(1111) 					1 												64

*/
#define CC2500_REG_SYNC1 				0x04 
#define CC2500_REG_SYNC0 				0x05 
#define CC2500_REG_PKTLEN 			0x06 
/*PKTLEN
Bit 		Field Name 			Description
7:0 		PACKET_LENGTH 	Indicates the packet length when fixed packet length is
												enabled. If variable length packets are used, this value
												indicates the maximum length packets allowed.
*/
#define CC2500_REG_PKTCTRL1			0x07
/*PKTCTRL1
Bit				Field Name			Description
7:5				PQT[2:0]				
4					Reserved				
3					CRC_AUTOFLUSH		
2					APPEND_STATUS		
1:0				ADR_CHK[1:0]		Setting  			Address check configuration
														0 (00) 			No address check
														1 (01) 			Address check, no broadcast
														2 (10) 			Address check and 0 (0x00) broadcast
														3 (11) 			Address check and 0 (0x00) and 255 (0xFF)
																				broadcast

*/
#define CC2500_REG_PKTCTRL0			0x08 
/*PKTCTRL0
Bit				Field Name				Description
7					Reserved
6					WHITE_DATA				Turn data whitening on / off
														0: Whitening off
														1: Whitening on
														Data whitening can only be used when
														PKTCTRL0.CC2400_EN=0 (default).					
5:4				PKT_FORMAT[1:0]		Format of RX and TX data
														Setting			Packet format
														0 (00) 			Normal mode, use FIFOs for RX and TX
														1 (01) 			Synchronous serial mode, used for backwards
																				compatibility. Data in on GDO0
														2 (10) 			Random TX mode; sends random data using PN9
																				generator. Used for test.
																				Works as normal mode, setting 0 (00), in RX.
														3 (11) 			Asynchronous serial mode. Data in on
3					CC2400_EN					Enable CC2400 support. Use same CRC implementation as
														CC2400.
														PKTCTRL1.CRC_AUTOFLUSH must be 0 if
														PKTCTRL0.CC2400_EN=1.
														PKTCTRL0.WHITE_DATA must be 0 if
														PKTCTRL0.CC2400_EN=1.
2					CRC_EN						1: CRC calculation in TX and CRC check in RX enabled
														0: CRC disabled for TX and RX
1:0				LENGTH_CONFIG[1:0]Configure the packet length
														Setting			Packet length configuration
														0 (00) 			Fixed packet length mode. Length configured in
																				PKTLEN register
														1 (01) 			Variable packet length mode. Packet length
																				configured by the first byte after sync word
														2 (10) 			Infinite packet length mode
														3 (11) 			Reserved


*/
#define CC2500_REG_ADDR					0x09 
/*ADDR
Bit			Field Name
7:0 		DEVICE_ADDR[7:0] 0    Address used for packet filtration. Optional
*/
#define CC2500_REG_CHANNR 			0x0A 

#define CC2500_REG_FSCTRL1			0x0B 
/* 		FSCTRL1 Register
Bit 	Field Name 				Description
7:5 	Reserved
4:0 	FREQ_IF[4:0]			The desired IF frequency to employ in RX. Subtracted from FS
												base frequency in RX and controls the digital complex mixer in
												the demodulator.
												The default value gives an IF frequency of 381 kHz, assuming
												a 26.0 MHz crystal.
*/
#define CC2500_REG_FSCTRL0			0x0C 
/*			FSCTRL1 Register
Bit 		Field Name 			Description
7:0 		FREQOFF[7:0]  	Frequency offset added to the base frequency before being
												used by the FS. (2’s complement).
												Resolution is FXTAL/214 (1.59 - 1.65 kHz); range is ±202 kHz to
												±210 kHz, dependent of XTAL frequency.
*/
#define CC2500_REG_FREQ2				0x0D 
/*FREQ2
Bit 		Field Name			Description
7:6 		FREQ[23:22] 		FREQ[23:22] is always binary 01 (the FREQ2 register is in the range 85 to
												95 with 26-27 MHz crystal)
5:0 		FREQ[21:16] 		FREQ[23:0] is the base frequency for the frequency synthesiser in
												increments of FXOSC/2^16.
*/
#define CC2500_REG_FREQ1 				0x0E 
/*FREQ1
Bit 		Field Name 			Description
7:0 		FREQ[15:8] 			FREQ2 register
*/
#define CC2500_REG_FREQ0				0x0F 
/*FREQ0
Bit 		Field Name 			Description
7:0 		FREQ[7:0] 			Ref. FREQ2 register
*/
#define CC2500_REG_MDMCFG4 			0x10 
#define CC2500_REG_MDMCFG3 			0x11 
#define CC2500_REG_MDMCFG2			0x12 
#define CC2500_REG_MDMCFG1 			0x13 
#define CC2500_REG_MDMCFG0 			0x14 
#define CC2500_REG_DEVIATN 			0x15 
#define CC2500_REG_MCSM2 				0x16 
#define CC2500_REG_MCSM1 				0x17 
#define CC2500_REG_MCSM0 				0x18
/*MCSM0
Main Radio Control State Machine
Bit 		Field Name 			Description
7:6 		Reserved
5:4 		FS_AUTOCAL[1:0] Automatically calibrate when going to RX or TX, or back to IDLE
												Setting 	When to perform automatic calibration
												0 (00) 		Never (manually calibrate using SCAL strobe)
												1 (01) 		When going from IDLE to RX or TX (or FSTXON)
												2 (10) 		When going from RX or TX back to IDLE
																	automatically
												3 (11)		Every 4th time when going from RX or TX to IDLE
																	automatically
																	In some automatic wake-on-radio (WOR) applications, using
																	setting 3 (11) can significantly reduce current consumption.
3:2 PO_TIMEOUT 					Programs the number of times the six-bit ripple counter must
												expire after XOSC has stabilized before CHP_RDYn goes low.
												If XOSC is on (stable) during power-down, PO_TIMEOUT
												should be set so that the regulated digital supply voltage has
												time to stabilize before CHP_RDYn goes low (PO_TIMEOUT=2
												recommended). Typical start-up time for the voltage regulator is
												50 us.
												If XOSC is off during power-down and the regulated digital
												supply voltage has sufficient time to stabilize while waiting for
												the crystal to be stable, PO_TIMEOUT can be set to 0. For
												robust operation it is recommended to use PO_TIMEOUT=2.
																	Setting Expire count 		Timeout after XOSC start
																	0 (00) 	1 							Approx. 2.3 – 2.4 µs
																	1 (01) 	16 							Approx. 37 – 39 µs
																	2 (10) 	64 							Approx. 149 – 155 µs
																	3 (11) 	256 						Approx. 597 – 620 µs
																	Exact timeout depends on crystal frequency.
1 PIN_CTRL_EN						Enables the pin radio control option
0 XOSC_FORCE_ON					Force the XOSC to stay on in the SLEEP state.

*/
#define CC2500_REG_FOCCFG 			0x19 
/*FOCCFG
Bit 		Field Name 			Description
7:6 		Reserved 
5 			FOC_BS_CS_GATE  If set, the demodulator freezes the frequency offset
												compensation and clock recovery feedback loops until the
												CARRIER_SENSE signal goes high.
4:3 		FOC_PRE_K[1:0] 	The frequency compensation loop gain to be used before a sync
												word is detected.
															Setting 			Freq. compensation loop gain before sync word
																	0 (00) 				K
																	1 (01) 				2K
																	2 (10) 				3K
																	3 (11) 				4K
2 FOC_POST_K 						The frequency compensation loop gain to be used after a sync
												word is detected.
															Setting 		Freq. compensation loop gain after sync word
																	0 					Same as FOC_PRE_K
																	1 					K/2
1:0 FOC_LIMIT[1:0]			The saturation point for the frequency offset compensation
												algorithm:
															Setting 		Saturation point (max compensated offset)
															0 (00) 						±0 (no frequency offset compensation)
															1 (01) 						±BWCHAN/8
															2 (10) 						±BWCHAN/4
															3 (11) 						±BWCHAN/2
												Frequency offset compensation is not supported for OOK;
												Always use FOC_LIMIT=0 with this modulation format.
*/
#define CC2500_REG_BSCFG				0x1A 
#define CC2500_REG_AGCCTRL2			0x1B 
#define CC2500_REG_AGCCTRL1 			0x1C 
#define CC2500_REG_AGCCTRL0			0x1D 
#define CC2500_REG_WOREVT1 			0x1E 
#define CC2500_REG_WOREVT0 			0x1F 
#define CC2500_REG_WORCTRL 			0x20 
#define CC2500_REG_FREND1 			0x21 
#define CC2500_REG_FREND0 			0x22 
#define CC2500_REG_FSCAL3				0x23 
#define CC2500_REG_FSCAL2 			0x24 
#define CC2500_REG_FSCAL1 			0x25 
#define CC2500_REG_FSCAL0 			0x26 
#define CC2500_REG_RCCTRL1 			0x27 
#define CC2500_REG_RCCTRL0 			0x28 
#define CC2500_REG_FSTEST 			0x29 
#define CC2500_REG_PTEST 				0x2A 
#define CC2500_REG_AGCTEST 			0x2B 
#define CC2500_REG_TEST2 				0x2C 
#define CC2500_REG_TEST1 				0x2D 
#define CC2500_REG_TEST0 				0x2E 

	
	

/******************************************************************************/
/*************************** START REGISTER CONFIGURATION  **************************/
/******************************************************************************/

//Frequency Synthesizer Control
#define CC2500_CFG_FSCTRL1    0x0C//0x12 //Frequency offset = 457kHz  Frequency Synthesizer Control

#define CC2500_CFG_FSCTRL0    0x00 //Frequency Synthesizer Control

//Frequency Control Word, High Byte
#define CC2500_CFG_FREQ2      0x5D // Carrier Frequency is 2.433GHz

//Frequency Control Word, Middle Byte
#define CC2500_CFG_FREQ1      0x93

//Frequency Control Word, Low Byte
#define CC2500_CFG_FREQ0      0xB1

//Modem Configuration
#define CC2500_CFG_MDMCFG4    0x0E //0x2D // BW of channel = 541.666kHz

#define CC2500_CFG_MDMCFG3    0x3B // Baud Rate = 125kb, Modem Config

#define CC2500_CFG_MDMCFG2    0x73 //0xF3 // Disable digital DC blocking filter before demodulator, MSK modulation, 16/16 sync word bits detected

#define CC2500_CFG_MDMCFG1    0x42 //0x22 Modem Config

#define CC2500_CFG_MDMCFG0    0xF8 // Default Channel Spacing of 200kHz

//Channel Number
#define CC2500_CFG_CHANNR     0x00 // Channel 0

//Modem Deviation Setting
#define CC2500_CFG_DEVIATN    0x00 //0x01 // 1785kHz

//Front End RX Configuration
#define CC2500_CFG_FREND1     0xB6

//Front End TX configuration
#define CC2500_CFG_FREND0     0x10

#define CC2500_CFG_MCSM0      0x18 // Automatically calibrate When going from IDLE to RX or TX (or FSTXON) check CC2500_CFG datasheet

// Frequency Offset Compensation Configuration
#define CC2500_CFG_FOCCFG     0x1D 

#define CC2500_CFG_BSCFG      0x1C

#define CC2500_CFG_AGCCTRL2   0xC7

#define CC2500_CFG_AGCCTRL1   0x40 //0x00

#define CC2500_CFG_AGCCTRL0   0xB0

#define CC2500_CFG_FSCAL3     0xEA

#define CC2500_CFG_FSCAL2     0x0A

#define CC2500_CFG_FSCAL1     0x00

#define CC2500_CFG_FSCAL0     0x19 //0x11

#define CC2500_CFG_FSTEST     0x59

#define CC2500_CFG_TEST2      0x88

#define CC2500_CFG_TEST1      0x31

#define CC2500_CFG_TEST0      0x0B

#define CC2500_CFG_FIFOTHR    0x07	//RX FIFO and TX FIFO Thresholds

//Input/Output Configuration
#define CC2500_CFG_IOCFG2     0x29

#define CC2500_CFG_IOCFG0D    0x06

#define CC2500_CFG_PKTCTRL1   0x00  //Packet Automation Control

#define CC2500_CFG_PKTCTRL0   0x0C //0x05 // Fixed Packet Length (0x05)

#define CC2500_CFG_ADDR       0x00 // Global Broadcast Address

#define CC2500_CFG_PKTLEN     0x01 // Packet Length of 10bytes (0xA) we could maybe set this to 2 for pitch & roll

#define CC2500_CFG_MCSM1      0x3E //page71 of reference manual


/******************************************************************************/
/*************************** END REGISTER CONFIGURATION  **************************/
/******************************************************************************/
/*************************DEFINE COMMAND STROBES***********************************/

//_T suffix to access TX FIFO register
#define SRES_T					0x30
#define SFSTXON_T				0x31
#define SXOFF_T					0x32
#define SCAL_T					0x33
#define SRX_T						0x34
#define STX_T						0x35
#define SIDLE_T					0x36
#define UNKNOWN_T				0x37
#define SWOR_T					0x38
#define SPWD_T					0x39
#define SFRX_T					0x3A
#define SFTX_T					0x3B
#define SWORRST_T				0x3C
#define SNOP_T					0x3D

//_R suffix to access RX FIFO register
#define SRES_R					0xB0
#define SFSTXON_R				0xB1
#define SXOFF_R					0xB2
#define SCAL_R					0xB3
#define SRX_R						0xB4
#define STX_R						0xB5
#define SIDLE_R					0xB6
#define UNKNOWN_R				0xB7
#define SWOR_R					0xB8
#define SPWD_R					0xB9
#define SFRX_R					0xBA
#define SFTX_R					0xBB
#define SWORRST_R				0xBC
#define SNOP_R					0xBD

#define FIFO_REG				0x3F

#define STROBE_DELAY		10









/** @defgroup CC2500_Exported_Macros
  * @{
  */
#define CC2500_CS_LOW()       GPIO_ResetBits(CC2500_SPI_CS_GPIO_PORT, CC2500_SPI_CS_PIN)
#define CC2500_CS_HIGH()      GPIO_SetBits(CC2500_SPI_CS_GPIO_PORT, CC2500_SPI_CS_PIN)



/** @defgroup STM32F4_DISCOVERY_CC2500_Exported_Functions
  * @{
  */ 
void CC2500_Init(void);
void CC2500_Write(uint8_t* pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite);
void CC2500_Read(uint8_t* pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead);
void CC2500_config_transmitter(void);
void CC2500_StrobeSend(uint8_t cmd, uint8_t* state, uint8_t* buffer_space);

void goToTX(uint8_t *state, uint8_t *buffer_space);
void goToRX(uint8_t *state, uint8_t *buffer_space);
void wireless_TX(struct Transmitter *transmitter);
void wireless_RX(struct Receiver *receiver);
	
	/* USER Callbacks: This is function for which prototype only is declared in
   MEMS accelerometre driver and that should be implemented into user applicaiton. */  
/* CC2500_TIMEOUT_UserCallback() function is called whenever a timeout condition 
   occure during communication (waiting transmit data register empty flag(TXE)
   or waiting receive data register is not empty flag (RXNE)).
   You can use the default timeout callback implementation by uncommenting the 
   define USE_DEFAULT_TIMEOUT_CALLBACK in stm32f4_discovery_lis302dl.h file.
   Typically the user implementation of this callback should reset MEMS peripheral
   and re-initialize communication or in worst case reset all the application. */
uint32_t CC2500_TIMEOUT_UserCallback(void);

#endif
