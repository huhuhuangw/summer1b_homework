/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
	
	
	//用了pwm，需要将 PB8 与 PC13 短接
	//单击：在1s内只按一次按钮。每单击一次按钮，调节LED亮度。
	//双击：在1s内按两次按钮，进入呼吸灯。如需关闭，单击一次按钮，等待一个呼吸灯周期，关闭
	//长按：长按按钮在2s以上，进入闪烁灯。如需关闭，单击一次按钮，关闭
	
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define key2_flag 1
#define key2 HAL_GPIO_ReadPin(Key2_GPIO_Port, Key2_Pin)
uint8_t key_flag;
int key_mode = -1;//0单击 1双击 2长按
int begin_flag = 0;
int times = 0;//计算按键次数 
int times_ok = 0;//存放次数
int t = 0;
int counts = 0;
int temp;//计时按键按下的时间
int sign = 0;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */


/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int lasttime = 0;
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	
	if(HAL_GetTick() - lasttime > 200)
	{
		if(times == 0)
		counts = (counts + 1)%4;
		
		if(times > 1)
				times = 0;
		
		if(times == 1)
		{
			__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_3,0);
			key_mode = 0;
			counts = 0;
			times = 0;
		}
		
		
		if(key_mode == 2)
		{
			__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_3,0);
			temp = 0;
			counts  = 0;
			key_mode = 0;
		}
		if(HAL_GetTick() - lasttime < 1000)
		{
			//times = (times + 1)%2;
			times++;
		}
		
	}
	lasttime = HAL_GetTick();
	
}

int Fact(int num)
{
	int sum = 1;
	for(int i = 0; i < num; i++)
	{
		sum = sum*10;
	}
	return sum;
}

void key2_scan(void)
{
	
	
/*	if(HAL_GPIO_ReadPin(Key2_GPIO_Port, Key2_Pin)==GPIO_PIN_RESET)
	{
		HAL_Delay(5);
		if(begin_flag == 0)
		{
			HAL_TIM_Base_Start_IT(&htim4);
			__HAL_TIM_SetCounter(&htim4,0);
		  begin_flag = 1;
		}
	  if(begin_flag == 1)
		{
				if(HAL_GPIO_ReadPin(Key2_GPIO_Port, Key2_Pin) == GPIO_PIN_RESET)
					times++;
		}	
	}*/

}//不知道为什么不能正常调用该函数，可能时钟没调好？？？？

int Key2_Scan(void)
{
//	static uint8_t flag = 1;//是否被按下
	
//	if(flag == 1)
//	{
		HAL_Delay(100);
		if(key2 == 0)
		{
//			flag = 0;
			temp = 0;
			while(key2 == 0 && temp < 100)
			{
				temp++;
				HAL_Delay(50);
			}
			key_flag = key2_flag;
			
		}
		return temp;
//	}
	/*else
	{
		flag = 1;
		key_flag = 0;
	}
	return 0;
	*/
}

void Key2_Mode(void)
{
	int key_t;
	key_t = Key2_Scan();
	
	if(key_t >= 0 && key_t<= 20)
	{
		
		//	key2_scan();
			if(times == 0)
			{
				key_mode = 0;
			}//单击
			
			if(times == 1)
			{
				key_mode = 1;
			}//双击
		
	}
	else if(key_t > 38)
	{
		key_mode = 2;
	}//长按
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM4)
    {
       begin_flag=0; 
			 times_ok=times;
			 times=0;
			
    }
   HAL_TIM_Base_Stop_IT(&htim4);

}


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
int nums = 0;
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
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
	__HAL_TIM_CLEAR_FLAG(&htim4, TIM_IT_UPDATE);
   HAL_TIM_Base_Stop_IT(&htim4);
	 HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_3);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		Key2_Mode();
		switch(key_mode)
		{
			case 0:
				__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_3,Fact(counts)-1);//0 9 99 999
			HAL_Delay(200);
			break;
			
			
			case 1:
				while(nums< 800)
				{
					nums++;
					__HAL_TIM_SetCompare(&htim4,TIM_CHANNEL_3,nums);
					HAL_Delay(2);
					
				}
				while(nums)
				{
					nums--;
					__HAL_TIM_SetCompare(&htim4,TIM_CHANNEL_3,nums);
					HAL_Delay(2);
					
				}
				HAL_Delay(200);
				break;
			
				
			case 2:
				
				__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_3,125);
				HAL_Delay(temp*25);
				__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_3,0);
				HAL_Delay(temp*25);
			break;
			
			
			default:
				break;
			
		}
	}
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		

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
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
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
