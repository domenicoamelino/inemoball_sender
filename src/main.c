#include "stm32f10x.h"
#include <stdio.h>
#include "LSM303DLHC/inc/LSM303DLHC.h"
#include "iNemo_Led/iNemoLed.h"
#include "STM32F1_VirtualCom/Source/inc/STM32F1_VC_General.h"
#include "iNEMO_Button.h"
#include "L3Gx/inc/L3Gx.h"
#include "iNEMO_AHRS_v121/inc/iNEMO_AHRS.h"
#include "tm_inemo_nrf24l01.h"
//------------------------ Definizione Variabili Globali ------------------------------


/*
typedef struct {
		float	time;
        float   accx;
        float   accy;
        float   accz;
        float   roll;
        float   pitch;
        float   yaw;
} Frame;
 */

typedef struct{
		float time;
		float q0;
		float q1;
		float q2;
		float q3;
} Frame;
Frame send_data;


uint8_t dataOut[32], dataRec[64];
uint8_t endofdata[10];

TM_NRF24L01_Transmit_Status_t transmissionStatus;

long timer = 0;
int i = 0;
/**
 * @brief Accelerometer sensor init structure
 */
LSMAccInit LSMAccInitStructure;


/**
* @brief Accelerometer high pass filter init structure
*/
LSMAccFilterInit LSMAccFilterInitStructure;


/**
* @brief Magnetometer sensor init structure
*/
LSMMagInit LSMMagInitStructure;


/**
 * @brief Gyroscopic sensor init structure
 */
L3GInit L3GInitStructure;


/**
 * @brief Acceleration and Magnetic field values
 */
float fAccXYZ[3], fMagXYZ[3];


/**
 * @brief Gyroscope data
 */
float fGyroXYZ[3];

/**
 * @brief AHRS filter input data structure
 */
iNEMO_SENSORDATA xSensorData;

iNEMO_EULER_ANGLES xEulerAngles={0};

iNEMO_QUAT  xQuat={0};

FlagStatus xTim2Raised = RESET;

uint8_t iter=0;
// ----------------------------------------------------------------------------

// -----------------------------Definizione Funzioni---------------------------
void prvFindFactors(u32 n, uint16_t *a, uint16_t *b)
{
	/** This function is copied from the ST STR7 library and is
	 * copyright STMicroelectronics.  Reproduced with permission.
	*/

	uint16_t b0;
	uint16_t a0;
	long err, err_min=n;


	*a = a0 = ((n-1)/0xffff) + 1;
	*b = b0 = n / *a;

	for (; *a < 0xffff-1; (*a)++)
	{
		*b = n / *a;
		err = (long)*a * (long)*b - (long)n;
		if (abs(err) > (*a / 2))
		{
			(*b)++;
			err = (long)*a * (long)*b - (long)n;
		}
		if (abs(err) < abs(err_min))
		{
			err_min = err;
			a0 = *a;
			b0 = *b;
			if (err == 0) break;
		}
	}

	*a = a0;
	*b = b0;
}
/**
 * @brief Configures the timer 2
 */
void iNemoTimerConfig(void)
{
  unsigned short a;
  unsigned short b;
  unsigned long n;
  /* This value is the frequency interrupts in Hz */
  unsigned char frequency = 50;
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable timer clocks */
  RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM2, ENABLE );

  TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

  TIM_TimeBaseStructInit( &TIM_TimeBaseStructure );

  /* Time base configuration for timer 2 - which generates the interrupts. */
  n = SystemCoreClock/frequency;

  prvFindFactors( n, &a, &b );
  TIM_TimeBaseStructure.TIM_Period = b - 1;
  TIM_TimeBaseStructure.TIM_Prescaler = a - 1;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit( TIM2, &TIM_TimeBaseStructure );
  TIM_ARRPreloadConfig( TIM2, ENABLE );

  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 13;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init( &NVIC_InitStructure );
}


/**
 * @brief Start and stop the timer used by the iNemo Data Task.
 * @param command start the timer if ENABLE, stop the timer if DISABLE.
 * @retval None.
 */
void Enable_Timer(FunctionalState command) {
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	TIM_ITConfig( TIM2, TIM_IT_Update, command );
	TIM_Cmd(TIM2, command);
}


/**
 * @brief This function handles TIM2 global interrupt request by resuming the
 * iNemoData task.
 */
void TIM2_IRQHandler(void)
{
  if(TIM_GetITStatus(TIM2, TIM_IT_Update))
  {
    xTim2Raised=SET;

    /* Clear the IRQ bit */
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
  }

}

void EXTI15_10_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line10)!= RESET)
  {
	  printf("Pulsante Premuto \n");
	  i++;
	  // inserire routine che fermi il ciclo al click del pulsante.
	  if(i==2)
		  {
		  	  int j;
		      for(j=0;j<10;j++) endofdata[j] = 0;
		      TM_NRF24L01_SendPacket(endofdata, 10);
		  	  iNEMO_Led_Off(LED1);
		  	  i=0;
		  	  timer = 0;
		  }
  }

  EXTI_ClearITPendingBit(EXTI_Line10);

}

// --------------------------------------------------------------------------------------
int main(void)
{
	 // Stm32f1VCInit();

		  //printf("VCP Inizializzato \n");


		//Initialize system
		  SystemInit();
			TM_NRF24L01_Init(15, 32);

			//Set 2MBps data rate and -18dBm output power
			TM_NRF24L01_SetRF(TM_NRF24L01_DataRate_2M, TM_NRF24L01_OutputPower_0dBm);

			 uint8_t MyAddress[] = {
				0xE7,
				0xE7,
				0xE7,
				0xE7,
				0xE7
			  };
			  // Receiver address
			  uint8_t TxAddress[] = {
				0x7E,
				0x7E,
				0x7E,
				0x7E,
				0x7E
			  };

			//Set my address, 5 bytes
			TM_NRF24L01_SetMyAddress(MyAddress);
			//Set TX address, 5 bytes
			TM_NRF24L01_SetTxAddress(TxAddress);

	  float temp[2];

	  for(uint32_t i=0; i<0xFFFFF;i++);

	  /* Initialize the iNemo LED */
	  iNEMO_Led_Init(LED1);

	  iNEMO_Button_Init(BUTTON_KEY, BUTTON_MODE_EXTI);
	  iNEMO_Button_Interrupt_Cmd(BUTTON_KEY,ENABLE);


	  printf("Led e Bottone Inizializzato \n");

	  iNemoTimerConfig();

	  printf("Timer Inizializzato \n");

	  /* Filter references for Acceleration and Magnetic field */
	  xSensorData.m_fAccRef[0]=0;
	  xSensorData.m_fAccRef[1]=0;
	  xSensorData.m_fAccRef[2]=-9.81f;

	  xSensorData.m_fMagRef[0]=0.37f;
	  xSensorData.m_fMagRef[1]=0;
	  xSensorData.m_fMagRef[2]=-0.25f;

	  xSensorData.m_fDeltaTime=0.02f;

	  xSensorData.m_fVarAcc=5.346e-6;
	  xSensorData.m_fVarMag=5.346e-6;


	  iNEMO_AHRS_Init(&xSensorData, &xEulerAngles, &xQuat);

	  printf("AHRS Inizializzato \n");

	  /* Initialize the MCU digital interface to communicate with the sensor */
	  Lsm303dlhcI2CInit();
	  /* Initialize the MCU digital interface to communicate with the sensor */
	  L3gd20CommInit();

	  printf("Inizializzati I2C e COM \n");

	  /* Fill the accelerometer structure */
	  LSMAccInitStructure.xPowerMode = LSM_NORMAL_MODE;
	  LSMAccInitStructure.xOutputDataRate = LSM_ODR_400_HZ;
	  LSMAccInitStructure.xEnabledAxes= LSM_ALL_AXES_EN;
	  LSMAccInitStructure.xFullScale = LSM_FS_2G;
	  LSMAccInitStructure.xDataUpdate = LSM_CONTINUOS_UPDATE;
	  LSMAccInitStructure.xEndianness=LSM_BIG_ENDIAN;
	  LSMAccInitStructure.xHighResolution=LSM_ENABLE;

	  /* Fill the accelerometer LPF structure */
	  LSMAccFilterInitStructure.xHPF=LSM_DISABLE;
	  LSMAccFilterInitStructure.xHPF_Mode=LSM_HPFM_NORMAL;
	  LSMAccFilterInitStructure.cHPFReference=0x00;
	  LSMAccFilterInitStructure.xHPFCutOff=LSM_HPCF_16;
	  LSMAccFilterInitStructure.xHPFClick=LSM_DISABLE;
	  LSMAccFilterInitStructure.xHPFAOI2=LSM_DISABLE;
	  LSMAccFilterInitStructure.xHPFAOI1=LSM_DISABLE;


	  /* Fill the magnetometer structure */
	  LSMMagInitStructure.xOutputDataRate = LSM_ODR_30_HZ;
	  LSMMagInitStructure.xFullScale = LSM_FS_1_9_GA;
	  LSMMagInitStructure.xWorkingMode = LSM_CONTINUOS_CONVERSION;
	  LSMMagInitStructure.xTemperatureSensor = LSM_ENABLE ;


	  /* Configure the accelerometer main parameters */
	  Lsm303dlhcAccConfig(&LSMAccInitStructure);

	  /* Configure the accelerometer LPF main parameters */
	  Lsm303dlhcAccFilterConfig(&LSMAccFilterInitStructure);

	  /* Configure the magnetometer main parameters */
	  Lsm303dlhcMagConfig(&LSMMagInitStructure);

	  /* Fill the gyro structure */
	  L3GInitStructure.xPowerMode = L3G_NORMAL_SLEEP_MODE;
	  L3GInitStructure.xOutputDataRate = L3G_ODR_190_HZ_CUTOFF_12_5;
	  L3GInitStructure.xEnabledAxes = L3G_ALL_AXES_EN;
	  L3GInitStructure.xFullScale = L3G_FS_500_DPS;
	  L3GInitStructure.xDataUpdate = L3G_BLOCK_UPDATE;
	  L3GInitStructure.xEndianness = L3G_BIG_ENDIAN;

	  /* Configure the gyro main parameters */
	  L3gd20Config(&L3GInitStructure);

	  printf("Inizializzati tutti i Sensori \n");

	  printf("Entro nel ciclone \n");

	  while(1)
	  {
		  if(i==1)
		  {
		     /* Read data */
		     Lsm303dlhcAccReadAcc(fAccXYZ);
		     Lsm303dlhcMagReadMag(fMagXYZ);
		     L3gd20ReadAngRate(fGyroXYZ);

		     temp[0]= -fGyroXYZ[1];
		     temp[1] = fGyroXYZ[0];
		     fGyroXYZ[0] = temp[0];
		     fGyroXYZ[1] = temp[1];

		     xSensorData.m_fAcc[0]=fAccXYZ[0]*9.8f/1000.0f;
		     xSensorData.m_fMag[0]=fMagXYZ[0]/1000.0;
		     xSensorData.m_fGyro[0]=fGyroXYZ[0]*3.141592f/180.0f;


		       xSensorData.m_fAcc[1]=-fAccXYZ[1]*9.8f/1000.0f;
		       xSensorData.m_fMag[1]=-fMagXYZ[2]/1000.0;
		       xSensorData.m_fGyro[1]=-fGyroXYZ[1]*3.141592f/180.0f;

		       xSensorData.m_fAcc[2]=-fAccXYZ[2]*9.8f/1000.0f;
		       xSensorData.m_fMag[2]=-fMagXYZ[1]/1000.0;
		       xSensorData.m_fGyro[2]=-fGyroXYZ[2]*3.141592f/180.0f;



		     iNEMO_AHRS_Update(&xSensorData, &xEulerAngles, &xQuat);

		     /* Inizio stampa su VCOM per adeguarsi al programma MATLAB */
		     timer = timer + 51;
		     send_data.time = timer;
		     send_data.q0 = xQuat[0];
		     send_data.q1 = xQuat[1];
		     send_data.q2 = xQuat[2];
		     send_data.q3 = xQuat[3];

		     uint8_t * frameblock = (uint8_t*) &send_data;

		     /* Modifica forse provvisoria, ora non invio più 28 byte..bensì 20 */
		     TM_NRF24L01_SendPacket(frameblock, 20);

		     /* Toggle the iNemo led */
		     delay_ms(50);
		     iNEMO_Led_Toggle(LED1);


		  }

	  }

}

// ----------------------------------------------------------------------------
void delay_ms(uint32_t ms)
{
 	ms *= 75000000UL / 1000 / 6;

 	asm volatile(" mov r0, %[ms] \n\t"
 			"1: subs r0, #1 \n\t"
 			" bhi 1b \n\t"
 			:
 			: [ms] "r" (ms)
 			: "r0");
 }

