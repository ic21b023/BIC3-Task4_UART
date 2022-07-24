/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h"
#include "mylib_serialprot.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define RxBuffer_SIZE 1
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
SERIALPROTOCOL_TypeDef hserialprot;

// Deklarieren und initialisieren des Ein-Zeichen-Empfangsbuffers
uint8_t RxBuffer[RxBuffer_SIZE]={0};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_UART_Receive_IT(&huart2, RxBuffer, RxBuffer_SIZE);

  HAL_GPIO_WritePin(RGB_BL_GPIO_Port, RGB_BL_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(RGB_RT_GPIO_Port, RGB_RT_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(RGB_GN_GPIO_Port, RGB_GN_Pin, GPIO_PIN_SET);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, RGB_BL_Pin|RGB_RT_Pin|RGB_GN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : RGB_BL_Pin RGB_RT_Pin RGB_GN_Pin */
  GPIO_InitStruct.Pin = RGB_BL_Pin|RGB_RT_Pin|RGB_GN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* UART-Callback wird nach jedem Zeichen aufgerufen */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	/* Nachrichtenbuffer für die Konsolen-Nachricht */
	uint8_t exchangedMessage[50] ={0};

	/*
	 * MYLIB_SERIALPROT_XCHANGE -> Verarbeitet die eingegebenen Zeichen des UART und gibt das demenstspechende Ergebnis/Nachricht zurück
	 * hserialprot -> Objekt des Seriellen Protokolls
	 * RxBuffer -> Ein-Zeichen-Empfangspuffer
	 * exchangedMessage -> zurückgegebene Nachricht aufgrund der Eingaben von Rx bzw. auf der Konsole
	 * Die Verarbeitung erfolgt in der MyLibrary/mylib_serialprot-Bibliothek
	 */
	MYLIB_SERIALPROT_XCHANGE(&hserialprot,RxBuffer,exchangedMessage);

	/* exchangedMessage an Putty/Konsole senden */
	if(HAL_UART_Transmit(&huart2, exchangedMessage,(uint16_t)strlen(exchangedMessage), 100)!= HAL_OK){Error_Handler();}

	/* UART_Receive Interrupt aktivieren */
	if(HAL_UART_Receive_IT(&huart2, RxBuffer, RxBuffer_SIZE)!= HAL_OK){Error_Handler();}
}

/* Callback für GPIO-Commands, welche der User selbst definieren kann */
uint8_t SERIALPROT_Command_GPO_Callback(SERIALPROTOCOL_TypeDef *hserialprot)
{
	uint8_t ret=1;

	/* Prüfen ob das Kommando "gpo" mit dem Parameter 1 "gn" und dem Parameter 2 "on" eingegeben wurde um die blaue LED einzuschalten */
	if(__SERIALPROT_IS_COMMAND(hserialprot,"gpo","gn","on")){
		HAL_GPIO_WritePin(RGB_GN_GPIO_Port, RGB_GN_Pin, GPIO_PIN_RESET);
		ret=0;

	/* Prüfen ob das Kommando "gpo" mit dem Parameter 1 "gn" und dem Parameter 2 "on" eingegeben wurde um die blaue LED auszuschalten */
	}else if(__SERIALPROT_IS_COMMAND(hserialprot,"gpo","gn","off") ){
		HAL_GPIO_WritePin(RGB_GN_GPIO_Port, RGB_GN_Pin, GPIO_PIN_SET);
		ret=0;
	}


	/* Prüfen ob das Kommando "gpo" mit dem Parameter 1 "gn" und dem Parameter 2 "on" eingegeben wurde um die grüne LED einzuschalten */
	if(__SERIALPROT_IS_COMMAND(hserialprot,"gpo","bl","on")){
		HAL_GPIO_WritePin(RGB_BL_GPIO_Port, RGB_BL_Pin, GPIO_PIN_RESET);
		ret=0;

	/* Prüfen ob das Kommando "gpo" mit dem Parameter 1 "gn" und dem Parameter 2 "on" eingegeben wurde um die grüne LED auszuschalten */
	}else if(__SERIALPROT_IS_COMMAND(hserialprot,"gpo","bl","off")){
		HAL_GPIO_WritePin(RGB_BL_GPIO_Port, RGB_BL_Pin, GPIO_PIN_SET);
		ret=0;
	}


	/* Prüfen ob das Kommando "gpo" mit dem Parameter 1 "gn" und dem Parameter 2 "on" eingegeben wurde um die rote LED einzuschalten */
	if(__SERIALPROT_IS_COMMAND(hserialprot,"gpo","rt","on")){
		HAL_GPIO_WritePin(RGB_RT_GPIO_Port, RGB_RT_Pin, GPIO_PIN_RESET);
		ret=0;

	/* Prüfen ob das Kommando "gpo" mit dem Parameter 1 "gn" und dem Parameter 2 "on" eingegeben wurde um die rote LED auszuschalten */
	}else if(__SERIALPROT_IS_COMMAND(hserialprot,"gpo","rt","off")){
		HAL_GPIO_WritePin(RGB_RT_GPIO_Port, RGB_RT_Pin, GPIO_PIN_SET);
		ret=0;
	}

	/* Rückgabe des Abfrageergebnisses */
	return ret;
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
