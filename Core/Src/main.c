/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "bh1750.h"
#include "mpu6050.h"
#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MPU6050_ADDR 0xD0
    // Continuously H-Resolution Mode (1 lx res)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t rxData[64];

uint8_t ble_rx_byte;
char ble_rx_buffer[128];
uint16_t ble_rx_index = 0;

char ble_tx_buffer[64];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
bool mpu6050_platform_i2c_write(I2C_HandleTypeDef *handle,
								uint8_t dev_addr,
								uint8_t reg_addr,
								const uint8_t *data,
								uint8_t len);
bool mpu6050_platform_i2c_read(I2C_HandleTypeDef *handle,
							   uint8_t dev_addr,
							   uint8_t reg_addr,
							   const uint8_t *data,
							   uint8_t len);

int8_t platform_i2c_write(I2C_HandleTypeDef *handle,
						uint8_t dev_addr,
						const uint8_t *data,
						uint8_t len);

int8_t platform_i2c_read(I2C_HandleTypeDef *handle,
						uint8_t dev_addr,
						const uint8_t *data,
						uint8_t len);

void platform_delay_ms(uint32_t ms);

void BLE_SendData(const char* data);
void BLE_SendSensorData(int16_t Ax, int16_t Ay, int16_t Az);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

int _write(int file, char *ptr, int len)
{
    for (int DataIdx = 0; DataIdx < len; DataIdx++)
    {
        ITM_SendChar(*ptr++);
    }
    return len;
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
  bh1750_t light_sensor;
  mpu6050_t sensor;
  mpu6050_config_t config = { .device_address = MPU6050_ADDR };
  bool mpu6050_driver_initialized = false;

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
  MX_I2C1_Init();
  MX_I2C2_Init();
  MX_UART4_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  mpu6050_driver_initialized = mpu6050_init(&sensor,
		  	  	  &config,
				  &hi2c1,
				  mpu6050_platform_i2c_write,
				  mpu6050_platform_i2c_read);

  if (mpu6050_driver_initialized)
  {
	  printf("--- accelerometer & gyroscope sensor initialized.\r\n");
  }

  if (bh1750_init(&light_sensor,
		  	  	  &hi2c2,
				  (BH1750_ADDR_LOW << 1),
				  platform_i2c_write,
				  platform_i2c_read,
				  platform_delay_ms) == BH1750_OK)
  {
	  printf("--- light sensor initialized.\r\n");
  }


  platform_delay_ms(500);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1)
  {
	if (mpu6050_driver_initialized)
	{
		if (mpu6050_read_sensors(&sensor))
		{
		  mpu6050_data_t data;
		  mpu6050_get_data(&sensor, &data);

		  HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
		  // for logic analyzer
		  HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_8);
		  printf("--- mpu6050 accelerometer data: %d %d  %d\r\n", data.ax, data.ay, data.az);
		  BLE_SendSensorData(data.ax, data.ay, data.az);
		}
	}

	if (bh1750_read_lux(&light_sensor) == BH1750_OK)
	{
		printf("--- BH1750 Lux value: %f\r\n", light_sensor.lux);

		if (light_sensor.lux > 250)
		{
		  printf("--- turn led on!\r\n");
		  HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, SET);
		  // for logic analyzer
		  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, SET);
		}
		else
		{
			printf("--- turn led off!\r\n");
			HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, RESET);
			// for logic analyzer
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, RESET);
		}
	}


	HAL_Delay(500);
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
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
/**
  * @brief Sends a generic string buffer over UART to the RYB2340_Lite.
  *        In transparent mode, this text pushes directly to the BLE notification characteristic.
  */
void BLE_SendData(const char* data)
{
  HAL_UART_Transmit(&huart4, (uint8_t*)data, strlen(data), HAL_MAX_DELAY);
}

/**
  * @brief Formats and transmits sensor data to the BLE module.
  */
void BLE_SendSensorData(int16_t Ax, int16_t Ay, int16_t Az)
{
  // Formats data into a clean, human-readable string or CSV format
  snprintf(ble_tx_buffer, sizeof(ble_tx_buffer), "Ax:%d, Ay: %d, Ay: %d\r\n", Ax, Ay, Az);
  BLE_SendData(ble_tx_buffer);
  printf(ble_tx_buffer);
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
