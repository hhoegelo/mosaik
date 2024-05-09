/*

### inputs
ID     | INPUT              | SPI-REG |
0..63  | seq_btn_1..64      | 0..7  |
64..71 | menu_btn_1..8      | 8     |
72..87 | encoder_1..7 (a/b) | 9, 10 |
88..94 | encoder_btn_1..7   | 11    |
95..103| menu_btn_9..16     | 12    |

## call sequence, every 1ms
- read_spi
- parse_spi
- btn_check
- enc_check
*/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "main.h"

#define BTN_PRESSED 0
#define BTN_RELEASED 1
#define BTN_DEADTIME_MS 20

#define N_REG 13
#define N_REG_IO (N_REG*8)
#define N_SEQ_BTN 64
#define N_MENU_BTN 16
#define N_ENC_BTN 7
#define N_BTN (N_SEQ_BTN + N_MENU_BTN + N_ENC_BTN)
#define N_ENC 7

typedef enum {
    STATE_WAIT_FOR_PRESS = 0,
	STATE_PRESS_DCNT = 1,
    STATE_WAIT_FOR_RELEASE = 2,
	STATE_RELEASE_DCNT = 3,
} btn_states_t;

typedef struct {
	uint8_t last_value;
	int8_t delta_value;
} enc_t;

typedef struct {
	uint8_t state;
	uint8_t dcnt;
	bool val;
} btn_t;

btn_t button[ N_BTN ] = {0};
enc_t encoder[ N_ENC ] = {0};
uint8_t spi_data[ N_REG ] = {0};

SPI_HandleTypeDef *spi_ptr;

void btn_init( SPI_HandleTypeDef *hspi )
{
	spi_ptr = hspi;
}


void read_spi(void)
{
	HAL_GPIO_WritePin( SPI2_SAP_GPIO_Port, SPI2_SAP_Pin, GPIO_PIN_RESET );
	HAL_GPIO_WritePin( SPI2_SAP_GPIO_Port, SPI2_SAP_Pin, GPIO_PIN_SET );
	HAL_SPI_Receive( spi_ptr, spi_data, N_REG, 3 );
}


void parse_spi(void)
{
	for( uint8_t reg = 0; reg < N_REG; reg++ )
	{
		switch( reg )
		{
			// sequencer
			case 0:		// btn seq 0..7
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:		// btn seq 55..63
			case 8:		// btn menu left 64..71
			{
				for( uint8_t btn = 0; btn < 8; btn++ )
				{
					button[ 8 * reg + btn ].val = spi_data[ reg ] & ( 0b00000001 << btn );
				}
				break;
			}
			case 12: // btn menu right 72..80
			{
				for( uint8_t btn = 0; btn < 8; btn++ )
				{
					uint8_t btn_id = 8*(reg-3) + btn; // reg 12 -> reg 9
					button[ btn_id ].val = spi_data[ reg ] & ( 0b00000001 << btn );
				}
				break;
			}
			case 11: // btn encoder 81..87
			{
				for( uint8_t btn = 0; btn < N_ENC_BTN; btn++ )
				{
					uint8_t btn_id = 8*(reg-1) + btn;	// reg 11 -> reg 10
					button[ btn_id ].val = spi_data[ reg ] & ( 0b00000001 << btn );
				}
				break;
			}
			case 9:	// encoder
			{
				for( uint8_t id=0; id < 4; id++ )
				{
					int8_t code, diff;
					uint8_t ph_a, ph_b;

					ph_b = spi_data[reg] & (0b00000001 << (id*2));
					ph_a = spi_data[reg] & (0b00000010 << (id*2));
					code = 0;

					if( ph_a )
						code = 3;
					if( ph_b )
						code ^= 1;

					diff = encoder[id].last_value - code;

					if( diff & 1 )
					{
						encoder[id].last_value = code;
						encoder[id].delta_value += (diff & 2) -1;
					}
				}
				break;
			}
			case 10:
			{
				for( uint8_t id=0; id < 3; id++ )
				{
					int8_t code, diff;
					uint8_t ph_a, ph_b;

					ph_b = spi_data[reg] & (0b00000001 << (id*2));
					ph_a = spi_data[reg] & (0b00000010 << (id*2));
					code = 0;

					if( ph_a )
						code = 3;
					if( ph_b )
						code ^= 1;

					diff = encoder[id+4].last_value - code;

					if( diff & 1 )
					{
						encoder[id+4].last_value = code;
						encoder[id+4].delta_value += (diff & 2) -1;
					}
				}
				break;
			}
			default:
				break;
		}
	}
}



uint8_t  usb_midi_tx_msg_cnt = 0;
void enc_check(void)
{
	for( int cnt=0; cnt<N_ENC; cnt++)
	{
		int8_t val = encoder[cnt].delta_value;

		//encoder[cnt].delta_value = 0; // 1 step
		encoder[cnt].delta_value = val & 1;
		val >>= 1;

		if( val != 0 )
		{
			//midiEventPacket_t event = {0x0B, 0xB0, cnt, 64+val};
			//MidiUSB.sendMIDI(event);
			usb_midi_tx_msg_cnt++;
		}
	}
}



char *usb_tx_buf;
uint8_t usb_buf_len;

// needs to be called every 1ms
void btn_check(void)
{
	for( uint8_t btn_id = 0; btn_id < N_BTN; btn_id++ )
	{
		switch( button[btn_id].state )
		{
			case STATE_WAIT_FOR_PRESS:
			{
				if( button[btn_id].val == BTN_PRESSED )
				{
					button[btn_id].dcnt = BTN_DEADTIME_MS;
					button[btn_id].state = STATE_PRESS_DCNT;

					//usb_buf_len = sprintf( usb_tx_buf, "b_%d \n", button[btn_id].state );
					//CDC_Transmit_FS( (uint8_t*)usb_tx_buf, usb_buf_len);
				}
				break;
			}
			case STATE_PRESS_DCNT:
			{
				button[btn_id].dcnt--;
				if( button[btn_id].dcnt == 0 )
				{
					button[btn_id].state = STATE_WAIT_FOR_RELEASE;
				}
				break;
			}
			case STATE_WAIT_FOR_RELEASE:
			{
				if( button[btn_id].val == BTN_RELEASED )
				{
					button[btn_id].dcnt = BTN_DEADTIME_MS;
					button[btn_id].state = STATE_RELEASE_DCNT;

					//usb_buf_len = sprintf( usb_tx_buf, "b_%d \n", button[btn_id].state );
					//CDC_Transmit_FS( (uint8_t*)usb_tx_buf, usb_buf_len);
					// send( btn_id, released )
				}
				break;
			}
			case STATE_RELEASE_DCNT:
			{
				button[btn_id].dcnt--;
				if( button[btn_id].dcnt == 0 )
				{
					button[btn_id].state = STATE_WAIT_FOR_PRESS;
				}
				break;
			}
			default:
				break;
		}
	}
}



#if 0
					if( ( spi_data[ reg ] & ( 0b00000001 << btn ) ) == 0 )
					{
						//uint8_t btn_id = 8 * reg + ( 7 - btn );	//0..63
						uint8_t btn_id = 8 * reg + btn;	//0..63, more!

						if( btn_dbc_array[ btn_id ] == 0 )
						{
							//midiEventPacket_t event = { 0x09, 0x90, btn_id, 1 };
							// midi is not send here
							//MidiUSB.sendMIDI( event );
							//usb_midi_tx_msg_cnt++;
							btn_dbc_array[ btn_id ] = BTN_DEADTIME_MS;
						}
					}
#endif
