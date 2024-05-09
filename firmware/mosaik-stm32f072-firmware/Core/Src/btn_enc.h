/*
 * btn_enc.h
 *
 *  Created on: May 4, 2024
 *      Author: dan
 */

#ifndef BTN_ENC_H_
#define BTN_ENC_H_

void btn_init( SPI_HandleTypeDef *hspi );

void read_spi(void);
void parse_spi(void);
void enc_check(void);
void btn_check(void);

#endif /* BTN_ENC_H_ */
