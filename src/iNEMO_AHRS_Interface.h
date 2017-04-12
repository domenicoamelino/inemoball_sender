/**
  * @file    iNEMO_AHRS_Interface.h
  * @author  ART Team IMS-Systems Lab
  * @version V1.0.0
  * @date    1 February 2013
  * @brief   This file is the header of the AHRS of iNEMO. This is the interface for your application.
  * @details
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * THIS SOURCE CODE IS PROTECTED BY A LICENSE.
  * FOR MORE INFORMATION PLEASE CAREFULLY READ THE LICENSE AGREEMENT FILE LOCATED
  * IN THE ROOT DIRECTORY OF THIS FIRMWARE PACKAGE.
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  */

#ifndef __INEMO_AHRS
#define __INEMO_AHRS

#ifdef __cplusplus
 extern "C" {
#endif


#include <stddef.h>

/** @addtogroup iNEMO_AHRS                      iNemo AHRS
  * @brief This header file is an interface to access the AHRS algorithms consisting of the three functions: AHRSInit, AHRSUpdate and AHRSDeInit.
  * @note  The AHRS library must be previously flashed starting from the AHRS_FUNC_FLASH_ADDR of your code memory.<br>
  *         A padding RAM memory array of bytes of dimension @ref AHRS_RESERVED_RAM_SIZE must be allocated in the user application starting from the
  *          address @ref AHRS_RESERVED_RAM_START_ADDR.
  * @details
  *        The code for these functions is located in the addesses written in the flash location pointed by AHRS_FUNC_FLASH_ADDR, AHRS_FUNC_FLASH_ADDR+4
  *        and AHRS_FUNC_FLASH_ADDR+8 respectively.
  *        In his application the user should declare three function pointers of type <i>pAHRSFunction</i>  and then call the @ref AHRS_POINTERS_INIT macro
  *         with these arguments (as documented after).
  *       After that the AHRS module will be available and usable. The AHRSInit function should be called in order to allocate memory, the AHRSDeInit will deallocate it,
  *       and the AHRSUpdate will perform an algorithm iteration returning the values into the passed variable of type iNEMO_EULER_ANGLES and iNEMO_QUAT[].
  * <br><br><b>Example:</b>
  * @code
  *
  *       #include "iNEMO_AHRS_Interface.h"
  *
  *       //AHRS filter input data structure
  *       iNEMO_EULER_ANGLES xEulerAngles={0};
  *       iNEMO_QUAT  xQuat={0};
  *       iNEMO_SENSORDATA xSensorData;
  *
  *       xSensorData.m_fVarAcc=5e-6;
  *       xSensorData.m_fVarMag=5.6e-6;
  *
  *       xSensorData.m_nCount=0;
  *       xSensorData->m_fDeltaTime=0.02;
  *
  *       xSensorData.m_fAccRef[0]=0;
  *   	  xSensorData.m_fAccRef[1]=0;
  *   	  xSensorData.m_fAccRef[2]=-9.81f;
  *
  *   	  xSensorData.m_fMagRef[0]=0.37f;
  *    	  xSensorData.m_fMagRef[1]=0;
  *    	  xSensorData.m_fMagRef[2]=-0.25f;
  *
  *       // function pointers for AHRS
  *       pAHRSFunction iNEMO_AHRS_Init,iNEMO_AHRS_DeInit,iNEMO_AHRS_Update;
  *
  *       // RAM padding to reserve the necessary space to the AHRS ensuring that our application does not use it.
  *       volatile uint8_t pcDummyAHRSReserved[AHRS_RESERVED_RAM_SIZE]@ AHRS_RESERVED_RAM_START_ADDR;
  *
  *       ...
  *
  *       // just touch the reserved memory at the beginning of the program to make sure the compiler allocates it.
  *       pcDummyAHRSReserved[0]=0;
  *
  *       // Set the AHRS functions pointers
  *       AHRS_POINTERS_INIT(iNEMO_AHRS_Init,iNEMO_AHRS_DeInit,iNEMO_AHRS_Update);
  *
  *       ...
  *       // Init the all AHRS parameters
  *       iNEMO_AHRS_Init(&xSensorData, &xEulerAngles, &xQuat);
  *
  *       for(int i=0;i<100;i++)
  *       {
  *          // wait 100 ms for example
  *          DelayMs(100);
  *          iNEMO_AHRS_Update(&xSensorData, &xEulerAngles, &xQuat);
  *
  *          ...
  *       }
  *       // Deallocate memory
  *       iNEMO_AHRS_DeInit(&xSensorData, &xEulerAngles, &xQuat);
  *       ....
  * @endcode
  * @{
  */


/** @defgroup iNEMO_AHRS_Structures           iNemo AHRS Structures
  * @{
  */

/**
 * @struct
 * @brief iNEMO Sensor Data Struct
 */
typedef struct
{
  unsigned int m_nCount;  /*!< It is used to perform correction at different frequencies */
  float   m_fDeltaTime;   /*!< It should trace the time difference */

  float m_fAcc[3];		/*!< The Acceleration Measurement */
  float m_fGyro[3];		/*!< The Gyroscope Measurement */
  float m_fMag[3];		/*!< The Magnitude Measurement */

  float m_fScaleFactor[9];      /*!< The Scale Factor Vector for each Measurement */
  float m_fOffset[9];		/*!< The Offset Vector for each Measurement */

  float m_fAccRef[3];           /*!< The gravitational vector refer */
  float m_fMagRef[3];           /*!< The magnetic vector refer */

  float m_fVarAcc;              /*!< The accelerometer variance */
  float m_fVarMag;              /*!< The magnetometer variance */
  float m_fVarGyro;             /*!< The gyroscope variance */

} iNEMO_SENSORDATA;



/**
 * @struct
 * @brief iNEMO Euler Angle Struct
 */
typedef struct
{
  float m_fRoll;          /*!< Roll Angle */
  float m_fPitch;         /*!< Pitch Angle */
  float m_fYaw;           /*!< Yaw Angle */
} iNEMO_EULER_ANGLES;



/**
 * @typedef iNEMO Quaternion Type
 */
typedef float iNEMO_QUAT[4];

/**
 * @}
 */


/**
 * @addtogroup iNEMO_AHRS_Function_pointers          iNemo AHRS Function pointers
 * @{
 */

/** iNEMO AHRS function type */
typedef  void (*pAHRSFunction)(iNEMO_SENSORDATA*    pSensorData,
                      iNEMO_EULER_ANGLES*  pAngle,
                      iNEMO_QUAT*          pQuat);


#define AHRS_FUNC_FLASH_ADDR                     0x0807F000
#define AHRS_RESERVED_RAM_SIZE                   0x3000
#define AHRS_RESERVED_RAM_START_ADDR             0x20000000

#define iNEMO_AHRS_VERSION_N		(*((uint8_t*)0x0807F00C))
#define iNEMO_AHRS_VERSION_SUBN		(*((uint8_t*)0x0807F00D))
#define iNEMO_AHRS_VERSION_SUBMIN	(*((uint8_t*)0x0807F00E))

/**
 * @brief Initialize all the 3 AHRS function pointers.
 * @param pInit  : Pointer to the initialization function. Dinamically allocates all the variables needed by the AHRS algorithm.
 * @param pDeInit: Pointer to the delete function. Delete all the AHRS variables allocated dinamically.
 * @param pUpdate: Pointer to the update function. Update AHRS variables.
 * @retval None.
 */
#define AHRS_POINTERS_INIT(pInit,pDeInit,pUpdate)   {\
     pInit=(pAHRSFunction)(*(uint32_t*)AHRS_FUNC_FLASH_ADDR);\
     pDeInit=(pAHRSFunction)(*(uint32_t*)(AHRS_FUNC_FLASH_ADDR+4));\
     pUpdate=(pAHRSFunction)(*(uint32_t*)(AHRS_FUNC_FLASH_ADDR+8));}

/**
 * @}
 */

/**
 * @}
 */

#endif

/******************* (C) COPYRIGHT 2013 STMicroelectronics *****END OF FILE****/


