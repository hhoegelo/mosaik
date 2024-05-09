/*
 * rgb_leds.c
 *
 *  Created on: May 4, 2024
 *      Author: dan
 */

#include "main.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

TIM_HandleTypeDef *tim_ptr;

#define N_LEDS 108

uint16_t pwm_data[ (24*N_LEDS)+50 ] = {};
//uint8_t rgb_data[ N_LEDS ][ 3 ] = {};



void rgb_reset();


void rgb_init( TIM_HandleTypeDef *htim )
{
	tim_ptr = htim;
	rgb_reset();
}


void rgb_reset()
{
	uint32_t cnt = 0;

	for( int led=0; led<N_LEDS; led++ )
	{
		for (int i=23; i>=0; i--)
		{
			pwm_data[cnt] = 20;  // reset all leds
			cnt++;
		}
	}
	for( int i=0; i<50; i++ )
	{
		pwm_data[cnt] = 0;
		cnt++;
	}
}


void rgb_set_led( uint8_t id, uint8_t r, uint8_t g, uint8_t b )
{
	uint32_t bit = 0;
	uint32_t color = (g<<16) | (r<<8) | b;

	for( int i=23; i>=0; i-- )
	{
		if( color & (1<<i) )
			pwm_data[ bit + id*24 ] = 40;  // 2/3 => 1
		else
			pwm_data[ bit + id*24 ] = 20;  // 1/3 => 0

		bit++;
	}
}


void rgb_update()
{
	HAL_TIM_PWM_Start_DMA( tim_ptr, TIM_CHANNEL_1, (uint32_t *)pwm_data, (24*N_LEDS)+50 );
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
	HAL_TIM_PWM_Stop_DMA( tim_ptr, TIM_CHANNEL_1 );
}







#if 0 // old rgb test functions

void Set_LED (int LEDnum, int Red, int Green, int Blue)
{
	LED_Data[LEDnum][0] = LEDnum;
	LED_Data[LEDnum][1] = Green;
	LED_Data[LEDnum][2] = Red;
	LED_Data[LEDnum][3] = Blue;
}

void RGBW_Set (uint8_t id, uint8_t red, uint8_t green, uint8_t blue, uint8_t white)
{
	RGBW_Data[id][0] = id;
	RGBW_Data[id][1] = green;
	RGBW_Data[id][2] = red;
	RGBW_Data[id][3] = blue;
	RGBW_Data[id][4] = white;
}

void RGBW_init()
{
	uint32_t indx=0;

	for (int led= 0; led<MAX_LED; led++)
	{
		for (int i=31; i>=0; i--)
		{
			rgbw_pwm_data[indx] = 20;  // 1/3 => 0
			indx++;
		}
	}

	for (int i=0; i<50; i++)
	{
		rgbw_pwm_data[indx] = 0;
		indx++;
	}
}

void RGBW_set_led (uint8_t id, uint8_t red, uint8_t green, uint8_t blue, uint8_t white)
{
	uint32_t bit = 0;
	uint32_t color = (green<<24) | (red<<16) | (blue<<8) | white;

	for( int i=31; i>=0; i-- )
	{
		if( color & (1<<i) )
			rgbw_pwm_data[ bit + id*32 ] = 40;  // 2/3 => 1
		else
			rgbw_pwm_data[ bit + id*32 ] = 20;  // 1/3 => 0

		bit++;
	}
}

void RGBW_Send (void)
{
	uint32_t indx=0;
	uint32_t color=0;

	HAL_GPIO_WritePin( SPI2_SAP_GPIO_Port, SPI2_SAP_Pin, GPIO_PIN_SET );
	for (int led= 0; led<MAX_LED; led++)
	{
		color = (RGBW_Data[led][1]<<24) | (RGBW_Data[led][2]<<16) | (RGBW_Data[led][3]<<8) | (RGBW_Data[led][4]);

		for (int i=31; i>=0; i--)
		{
			if (color&(1<<i))
			{
				rgbw_pwm_data[indx] = 40;  // 2/3 => 1
			}
			else rgbw_pwm_data[indx] = 20;  // 1/3 => 0

			indx++;
		}
	}
	HAL_GPIO_WritePin( SPI2_SAP_GPIO_Port, SPI2_SAP_Pin, GPIO_PIN_RESET );

	for (int i=0; i<50; i++)
	{
		rgbw_pwm_data[indx] = 0;
		indx++;
	}

	HAL_TIM_PWM_Start_DMA(&htim1, TIM_CHANNEL_1, (uint32_t *)rgbw_pwm_data, indx);
}



void WS2812_Send (void)
{
	uint32_t indx=0;
	uint32_t color=0;

	for (int i= 0; i<MAX_LED; i++)
	{
		color = ((LED_Data[i][1]<<16) | (LED_Data[i][2]<<8) | (LED_Data[i][3]));

		for (int i=23; i>=0; i--)
		{
			if (color&(1<<i))
			{
				pwmData[indx] = 40;  // 2/3 of
			}
			else pwmData[indx] = 20;  // 1/3 of 90
			indx++;
		}
	}
	for (int i=0; i<50; i++)
	{
		pwmData[indx] = 0;
		indx++;
	}

	HAL_TIM_PWM_Start_DMA(&htim1, TIM_CHANNEL_1, (uint32_t *)pwmData, indx);
	//while (!datasentflag){};
	//datasentflag = 0;
}




#endif
