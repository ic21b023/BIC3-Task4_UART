/**
  ******************************************************************************
  * @file    mylib_serialprot.h
  * @author  Reiter Roman
  * @brief   Header file MYLIB_SERIALPROT

  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef INC_MYLIB_SERIALPROT_H_
#define INC_MYLIB_SERIALPROT_H_

#ifdef __cplusplus
 extern "C" {
#endif

 /* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
 /* Exported macros -----------------------------------------------------------*/
 /** @defgroup TIM_Exported_Macros TIM Exported Macros
   * @{
   */

 /**
   * @brief  SERIALPROT Kommando-Abfragen
   * @param  __HANDLE__ SERIALPROT handle.
   * @param  __COMMANDNAME__ SERIALPROT Kommandoname
   * @param  __PARAMETER1__ SERIALPROT Parameter1 des Kommandos
   * @param  __PARAMETER2__ SERIALPROT Parameter2 des Kommandos
   * @retval None
   */
#define __SERIALPROT_IS_COMMAND(__HANDLE__, __COMMANDNAME__, __PARAMETER1__, __PARAMETER2__) ((!strcmp((__HANDLE__)->CommandName,(__COMMANDNAME__))) && (!strcmp((__HANDLE__)->Parameter1,(__PARAMETER1__))) && (!strcmp((__HANDLE__)->Parameter2,(__PARAMETER2__))))
#define __SERIALPROT_IS_COMMANDNAME(__HANDLE__, __COMMANDNAME__)  (!strcmp((__HANDLE__)->CommandName,(__COMMANDNAME__)))

 /**
   * @}
   */
 /* End of exported macros ----------------------------------------------------*/


 /* Exported types ------------------------------------------------------------*/
 /** @defgroup TIM_Exported_Types TIM Exported Types
   * @{
   */

 /**
   * @brief  SERIALPROT Message Status definition
   */
 typedef enum
 {
	 SERIALPROT_OK = 0x00,					/*!< Nachricht OK */
	 SERIALPROT_ERROR   = 0x01				/*!< Nachricht nicht OK */
 } SERIALPROTCOL_StatusTypeDef;


 /**
   * @brief  SERIALPROT Message Kind definition
   */
 typedef enum
 {
	 MESSAGEKIND_TEXT_NUMBER = 0x00,		/*!< Text-Nummer Nachricht */
	 MESSAGEKIND_NUMBER_NUMBER   = 0x01, 	/*!< Nummer-Nummer Nachricht */
	 MESSAGEKIND_TEXT_TEXT = 0x02,			/*!< Text-Text Nachricht */
	 MESSAGEKIND_NUMBER_TEXT   = 0x03		/*!< Nummer-Text Nachricht */
 } SERIALPROTOCOL_MessageKindTypeDef;


 /**
   * @brief  SERIALPROT Status structures definition
   */
 typedef struct
 {
   uint8_t CommandName[5];        /*!< Kommandoname */

   SERIALPROTOCOL_MessageKindTypeDef MessageKind; /*!< Nachrichtentyp */

   uint8_t Parameter1[15];       /*!< Parameter1 des Kommandos */

   uint8_t Parameter2[15];       /*!< Parameter2 des Kommandos */
 }SERIALPROTOCOL_TypeDef;

 /**
   * @}
   */
 /* End of exported types -----------------------------------------------------*/


 /* Exported functions --------------------------------------------------------*/
 /** @addtogroup UART_Exported_Functions UART Exported Functions
   * @{
   */

 /* IO operation functions *****************************************************/
void MYLIB_SERIALPROT_XCHANGE(SERIALPROTOCOL_TypeDef *hserialprot,uint8_t * RxBuffer, uint8_t * last );

/* Callbacks Register/UnRegister functions  ***********************************/
uint8_t SERIALPROT_Command_GPO_Callback(SERIALPROTOCOL_TypeDef *hserialprot);

/**
  * @}
  */
/* End of exported functions -------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* INC_MYLIB_SERIALPROT_H_ */
