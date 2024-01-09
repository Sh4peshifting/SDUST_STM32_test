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
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
uint8_t TIM5CH1_CAPTURE_STA = 0; // 输入捕获状态
uint16_t TIM5CH1_CAPTURE_VAL;    // 输入捕获值(TIM5 是 16 位)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
// 定时器更新中断（计数溢出）中断处理回调函数，
// 该函数在 HAL_TIM_IRQHandler 中会被调用
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
// 更新中断（溢出）发生时执行
{
  if ((TIM5CH1_CAPTURE_STA & 0X80) == 0) // 还未成功捕获
  {
    if (TIM5CH1_CAPTURE_STA & 0X40) // 已经捕获到高电平了
    {
      if ((TIM5CH1_CAPTURE_STA & 0X3F) == 0X3F) // 高电平太长了
      {
        TIM5CH1_CAPTURE_STA |= 0X80; // 标记成功捕获了一次
        TIM5CH1_CAPTURE_VAL = 0XFFFF;
      }
      else
        TIM5CH1_CAPTURE_STA++;
    }
  }
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) // 捕获中断发生时执行
{
  if ((TIM5CH1_CAPTURE_STA & 0X80) == 0) // 还未成功捕获
  {
    if (TIM5CH1_CAPTURE_STA & 0X40) // 捕获到一个下降沿
    {
      TIM5CH1_CAPTURE_STA |= 0X80;                                            // 标记成功捕获到一次高电平脉宽
      TIM5CH1_CAPTURE_VAL = HAL_TIM_ReadCapturedValue(&htim5, TIM_CHANNEL_1); // 获取当前的捕获值.
      TIM_RESET_CAPTUREPOLARITY(&htim5, TIM_CHANNEL_1);
      // 一定要先清除原来的设置！！
      TIM_SET_CAPTUREPOLARITY(&htim5, TIM_CHANNEL_1,
                              TIM_ICPOLARITY_RISING); // 配置 TIM5 通道 1 上升沿捕获
    }
    else // 还未开始,第一次捕获上升沿
    {
      TIM5CH1_CAPTURE_STA = 0; // 清空
      TIM5CH1_CAPTURE_VAL = 0;
      TIM5CH1_CAPTURE_STA |= 0X40; // 标记捕获到了上升沿
      __HAL_TIM_DISABLE(&htim5);   // 关闭定时器 5
      __HAL_TIM_SET_COUNTER(&htim5, 0);
      TIM_RESET_CAPTUREPOLARITY(&htim5, TIM_CHANNEL_1);
      // 一定要先清除原来的设置！！
      TIM_SET_CAPTUREPOLARITY(&htim5, TIM_CHANNEL_1,
                              TIM_ICPOLARITY_FALLING); // 定时器 5 通道 1 设置为下降沿捕获
      __HAL_TIM_ENABLE(&htim5);                        // 使能定时器 5
    }
  }
}

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
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
  MX_TIM5_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_IC_Start_IT(&htim5, TIM_CHANNEL_1);
  long long temp = 0;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    if (TIM5CH1_CAPTURE_STA & 0X80) // 成功捕获到了一次高电平
    {
      temp = TIM5CH1_CAPTURE_STA & 0X3F;
      temp *= 65536;                    // 溢出时间总和
      temp += TIM5CH1_CAPTURE_VAL;      // 得到总的高电平时间
      char send_data[20];//存放发送数据的数组
      sprintf(send_data, "HIGH:%lld us\r\n", temp); // 打印总的高点平时间
      HAL_UART_Transmit(&huart1, (uint8_t *)send_data, strlen(send_data), 50);
      TIM5CH1_CAPTURE_STA = 0;          // 开启下一次捕获
    }

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

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
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
