/*
 * rgb_leds.h
 *
 *  Created on: May 4, 2024
 *      Author: dan
 */

#ifndef RGB_LEDS_H_
#define RGB_LEDS_H_

void rgb_init( TIM_HandleTypeDef *htim );
void rgb_reset();
void rgb_set_led( uint8_t id, uint8_t r, uint8_t g, uint8_t b );
void rgb_update();

#endif /* RGB_LEDS_H_ */
