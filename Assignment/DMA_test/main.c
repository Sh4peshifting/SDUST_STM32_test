/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include "lcd.h"
#include "stdbool.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
#define ARRAY_SIZE 1000

uint32_t array[ARRAY_SIZE];
uint32_t destination[ARRAY_SIZE];
uint32_t start, end;
uint32_t dma_time, cpu_time;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void LED_Disp(unsigned char dsLED)
{
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_All, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOC, dsLED << 8, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
}

#define LONG_PRESS_TIME 500

struct keys
{
  GPIO_TypeDef *key_port;
  uint16_t key_pin;
  bool pressed_flag;
  unsigned int press_time;
  unsigned int press_start_time;
};

struct keys key[4] = {0};

void key_init(void)
{
  key[0].key_port = GPIOB;
  key[1].key_port = GPIOB;
  key[2].key_port = GPIOB;
  key[3].key_port = GPIOA;
  key[0].key_pin = GPIO_PIN_0;
  key[1].key_pin = GPIO_PIN_1;
  key[2].key_pin = GPIO_PIN_2;
  key[3].key_pin = GPIO_PIN_0;
}

void key_scan(void)
{

  // 扫描按键
  for (int i = 0; i < 1; i++)
  {
    if (HAL_GPIO_ReadPin(key[i].key_port, key[i].key_pin) == 0)
    {
      HAL_Delay(10); // 按键消抖
      if (HAL_GPIO_ReadPin(key[i].key_port, key[i].key_pin) == 0)
      {
        key[i].press_start_time = HAL_GetTick();
        while (HAL_GPIO_ReadPin(key[i].key_port, key[i].key_pin) == 0)
          ;
        key[i].pressed_flag = 1; // 按键按下
        // 计算按下时间
        key[i].press_time = HAL_GetTick() - key[i].press_start_time;
      }
    }
  }
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void fill_array(void)
{
  for (int i = 0; i < ARRAY_SIZE; i++)
  {
    array[i] = i;
  }
}

void dma_transfer(void)
{
  TIM1->CNT = 0;
  start = TIM1->CNT;
  for (int i = 0; i < 1000; i++)
  {
    HAL_DMA_Start(&hdma_memtomem_dma1_channel1, (uint32_t)array, (uint32_t)destination, ARRAY_SIZE);
    HAL_DMA_PollForTransfer(&hdma_memtomem_dma1_channel1, HAL_DMA_FULL_TRANSFER, HAL_MAX_DELAY);
  }
  end = TIM1->CNT;
  dma_time = end - start;
}

void cpu_transfer(void)
{
  TIM1->CNT = 0;
  start = TIM1->CNT;
  for (int j = 0; j < 1000; j++)
  {
    for (int i = 0; i < ARRAY_SIZE; i++)
    {
      destination[i] = array[i];
    }
  }
  end = TIM1->CNT;
  cpu_time = end - start;
}

void key_proc(void)
{
  if (key[0].pressed_flag == 1) // 按键按下
  {
    char buffer[50];
    sprintf(buffer, "Transferring %.2f MB\r\n", (ARRAY_SIZE * 4 * 1000) / (1024.0 * 1024.0)); // 4 bytes per uint32_t
    HAL_UART_Transmit(&huart1, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY);

    dma_transfer();
    cpu_transfer();

    sprintf(buffer, "DMA time: %d us, CPU time: %d us\r\n", dma_time, cpu_time);
    HAL_UART_Transmit(&huart1, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY);

    sprintf(buffer, "Transfer Complete\r\n");
    HAL_UART_Transmit(&huart1, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY);

    sprintf(buffer, "DMA is %.2f times faster than CPU\r\n", cpu_time / (float)dma_time);
    HAL_UART_Transmit(&huart1, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY);

    key[0].pressed_flag = 0;
  }
}
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
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
  LCD_Init();
  LCD_Clear(Black); // 清屏函数
  LCD_SetBackColor(Black);
  LCD_SetTextColor(White);

  LED_Disp(0x00);

  HAL_TIM_Base_Start(&htim1);

  key_init();

  fill_array();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    key_scan(); // 调用按键扫描
    key_proc();
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
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV3;
  RCC_OscInitStruct.PLL.PLLN = 20;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

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

#ifdef USE_FULL_ASSERT
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
