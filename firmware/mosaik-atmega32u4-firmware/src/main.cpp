#include <SPI.h>
#include "MIDIUSB.h"
#include <Arduino.h>
#include <stdlib.h>
#include "coos.h"
#include "TimerOne.h"
#include "Adafruit_NeoPixel.h"
#include <avr/power.h>


/**
 * @brief mosaik controller firmware for arduino pro micro 5V, 16MHz
 * @todo add serial print via uart
 * @todo separate buttons and encoder into two array
	amidi -l
 	aseqdump -l
 	aseqdump -p 20
 	aconnect 24:0 20:0
 	aconnect -d 24:0 20:0
	amidi -p hw:1,0,0 -S '92 25 00'

                             id val
	amidi -p hw:1,0,0 -S '90 05 01'
	90 red-channel
	91 green-channel
	92 blue-channel
	93 white-channel
	94 color table
	95 update
	96 all leds on
	97 all leds off  

	# color table (94)       ID color
	amidi -p hw:1,0,0 -S '94 6A 00'
	{ 2, 0, 0}, // 0 red	relsub0
	{ 0, 2, 0}, // 1 green	relsub1
	{ 0, 0, 2},	// 2 blue	relsub2
	{ 2, 0, 2},	// 3 purple	relsub3
	{ 2, 2, 2},	// 4 white	stepled
	{ 0, 0, 0},	// 5 off  
	*/


#define _ID  0x04	// mosaik23 id

#define SPI_SCK 15  // 165_P2
#define SPI_SAP 8  // 165_P1
#define SPI_SDI 14  // 165_P9

#define NUM_REGS 13 //
#define NUM_BTNS 104 // 72 / 104 @todo btn and enc need to be separated
#define NUM_ENC 7
#define BTN_DEAD_TIME 100

#define PIN_LED_HB 5
#define PIN_LED_SYSTICK 6
#define PIN_RGB 9
#define NUMPIXELS 108
#define _RED 1
#define _GRN 2
#define _BLU 3
#define _WHT 0

#define N_SEQ_BTN 64
#define N_MENU_BTN 16
#define N_ENC 7
#define N_ENC_BTN 7



typedef struct main
{
	uint8_t last_value;
	int8_t delta_value;
} enc_t;

enc_t encoder[N_ENC] = {};

bool rgb_leds_update_flag = 0;

uint8_t col_table[6][3] = {
	{ 5, 0, 0}, // 0 red	relsub0
	{ 0, 0, 5}, // 1 blue	relsub1
	{ 0, 5, 0},	// 2 green	relsub2
	{ 5, 5, 5},	// 3 purple	relsub3
	{ 2, 2, 2},	// 4 white	stepled
	{ 0, 0, 0},	// 5 off
};

uint8_t  spi_data[ NUM_REGS ] = {};
uint8_t  btn_dbc_array[ NUM_BTNS ] = {};
uint8_t  usb_midi_tx_msg_cnt = 0;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel( NUMPIXELS, PIN_RGB, NEO_GRB + NEO_KHZ800 );

void process_hb(void);
void process_systick(void);

void process_spi_read(void);
void process_spi_parse(void);
void process_btn_debounce(void);

void process_midi_buf_rx_read(void);
void process_midi_buf_tx_send(void);
void process_rgb_leds_update(void);
void process_read_encoder(void);

void print_states(void);
void timer1_irq(void);


void setup()
{
	Serial1.begin( 115200 );
	Serial1.println( "\n## Mosaik Midi Controller | Version: " __DATE__ "-" __TIME__ " ##");
	Serial1.println( );

	/*  - 1MHz: 1µs/bit 
		- 13 shift register
		- 104 inputs
		- => 104µs per transfer	 */ 
	SPI.beginTransaction( SPISettings( 1000000, MSBFIRST, SPI_MODE3 ));  
	SPI.begin();

	// init GPIOs
	pinMode( SPI_SAP, OUTPUT );
	pinMode( PIN_LED_HB, OUTPUT );
	pinMode( PIN_LED_SYSTICK, OUTPUT );
	digitalWrite( SPI_SAP, HIGH );

	// init neopixel
	pixels.begin();
	for( int id = 0; id < NUMPIXELS; id++ )
	{
		pixels.setPixelColor(id, pixels.Color(0,0,25));
	}
	pixels.show();

	// init scheduler
	coos_init();
	coos_task_add( process_systick, 0, 1 );
	coos_task_add( process_hb, 0, 500 );
	coos_task_add( process_spi_read, 0, 1 );
	coos_task_add( process_spi_parse, 0, 1 );
	coos_task_add( process_btn_debounce, 0, 1 );
	coos_task_add( process_midi_buf_tx_send, 1, 4 );
	coos_task_add( process_midi_buf_rx_read, 2, 4 );
	coos_task_add( process_read_encoder, 50, 10 );

	// start scheduler
	Timer1.initialize(2000); // in µs
	Timer1.attachInterrupt(timer1_irq);
}


bool systick = false;
void loop()
{
	if( systick == true )
	{
		systick = false;
		coos_update();
	}
	coos_dispatch();
}


void timer1_irq()
{
	systick = true;
}

void process_hb()
{
	digitalWrite( PIN_LED_HB, !digitalRead( PIN_LED_HB ));	
	//Serial1.println("heartbeat");
}

void process_systick()
{
	digitalWrite( PIN_LED_SYSTICK, !digitalRead( PIN_LED_SYSTICK ));
}


/* 	read all SPI shift register 74xx165 (buttons + encoder) 
	@todo replace digital write by fast gpio 
	SPI data array:
	ID     | INPUT
	0..63  | seq_btn_1..64
	64..71 | menu_btn_1..8
	72..87 | encoder_1..7 (a/b)
	88..94 | encoder_btn_1..7
	95..103| menu_btn_9..16  */
void process_spi_read(void)
{
	digitalWrite(SPI_SAP, LOW);
	digitalWrite(SPI_SAP, HIGH);
	
	for( int cnt=0; cnt<NUM_REGS; cnt++ )
	{
		spi_data[ cnt ] = SPI.transfer( cnt );
	}
}

void process_rgb_leds_update(void)
{
	if( rgb_leds_update_flag == 1 )
	{
		pixels.show();
	}
	rgb_leds_update_flag = false;
}


void print_states(void)
{
	for( int cnt = 0; cnt < NUM_REGS; cnt++ )
	{
		Serial.print( spi_data[ cnt ] );
		Serial.print( " " );
	}
	Serial.println();
}


void process_read_encoder(void)
{
	for( int cnt=0; cnt<N_ENC; cnt++)
	{ 
		int8_t val = encoder[cnt].delta_value; 

		//encoder[cnt].delta_value = 0; // 1 step
		encoder[cnt].delta_value = val & 1; 
		val >>= 1;

		if( val != 0 )
		{
			midiEventPacket_t event = {0x0B, 0xB0, cnt, 64+val}; 
			MidiUSB.sendMIDI(event);
			usb_midi_tx_msg_cnt++;
		}
	}
}


void process_spi_parse(void)
{
	for( uint8_t reg = 0; reg < NUM_REGS; reg++ )
	{
		switch( reg )
		{
			// sequencer
			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
			case 8:		// menu left
			case 11:	// encoder
			case 12:	// menu right
				for( uint8_t btn = 0; btn < 8; btn++ )
				{
					if( ( spi_data[ reg ] & ( 0b00000001 << btn ) ) == 0 )
					{
						//uint8_t btn_id = 8 * reg + ( 7 - btn );	//0..63
						uint8_t btn_id = 8 * reg + btn;	//0..63

						if( btn_dbc_array[ btn_id ] == 0 )
						{
							midiEventPacket_t event = { 0x09, 0x90, btn_id, 1 };
							// midi is not send here
							MidiUSB.sendMIDI( event );
							usb_midi_tx_msg_cnt++;
							btn_dbc_array[ btn_id ] = BTN_DEAD_TIME;
						}
					}
				}
				break;

			// encoder 
			case 9:
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
			case 10:
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

			default:
				break;
		}
	}
}


// button dead time count down 
void process_btn_debounce(void)
{
	for( uint8_t btn = 0; btn < NUM_BTNS; btn++ )
	{
		if( btn_dbc_array[ btn ] > 0)
		{
			btn_dbc_array[ btn ]--;
		}
	}
}


// send midi buffer if new data available
void process_midi_buf_tx_send(void)
{
	if ( usb_midi_tx_msg_cnt > 0 )
	{
		MidiUSB.flush();
		usb_midi_tx_msg_cnt = 0;
	}
}

/*                           id val
	amidi -p hw:1,0,0 -S '90 05 01'
	90 red
	91 green
	92 blue
	93 white
	94 color table
	95 update
	96 all leds on
	97 all leds off    */
void process_midi_buf_rx_read(void)
{
	midiEventPacket_t rx;
	rx = MidiUSB.read();

		if (rx.header == 0x0A)  // system message
		{
			if ( rx.byte2 == 0x01)  // hwui identifier message
			{
				midiEventPacket_t event = {0x0A, 0xA0, 0, _ID};
				MidiUSB.sendMIDI(event);
				usb_midi_tx_msg_cnt++;
			}
		}

	if (rx.header == 0x9) // NOTE ON
	{
		uint8_t color = rx.byte1 & 0b00001111;
		uint8_t id = rx.byte2;
		uint8_t brightness = rx.byte3 << 1;
		uint8_t col_tab_pos = rx.byte3 & 0b01111111;
		uint8_t prevColor[4] = {};

		uint32_t prevCol = pixels.getPixelColor(id);
		prevColor[_BLU] = (uint8_t)  prevCol;
		prevColor[_GRN] = (uint8_t) (prevCol >> 8);
		prevColor[_RED] = (uint8_t) (prevCol >> 16);
		prevColor[_WHT] = (uint8_t) (prevCol >> 24);

		// set pixel color
		switch (color)
		{
			case 0: // red
			{
				//pixels.setPixelColor(id, pixels.Color(brightness, prevColor[_GRN], prevColor[_BLU], prevColor[_WHT]));
				pixels.setPixelColor(id, pixels.Color(brightness, prevColor[_GRN], prevColor[_BLU]));
				pixels.show();
				break;
			}
			case 1: // green
			{
				//pixels.setPixelColor(id, pixels.Color(prevColor[_RED], brightness, prevColor[_BLU], prevColor[_WHT]));
				pixels.setPixelColor(id, pixels.Color(prevColor[_RED], brightness, prevColor[_BLU]));
				//rgb_leds_update_flag = true;
				pixels.show();
				break;
			}
			case 2: // blue
			{
				//pixels.setPixelColor(id, pixels.Color(prevColor[_RED], prevColor[_GRN], brightness, prevColor[_WHT]));
				pixels.setPixelColor(id, pixels.Color(prevColor[_RED], prevColor[_GRN], brightness));
				//rgb_leds_update_flag = true;
				pixels.show();
				break;
			}
			case 3: // white
			{
				//pixels.setPixelColor(id, pixels.Color(prevColor[_RED], prevColor[_GRN], prevColor[_BLU], brightness));
				pixels.setPixelColor(id, pixels.Color( brightness, brightness, brightness));
				//rgb_leds_update_flag = true;
				pixels.show();
				break;
			}
			case 4: // color table
			{
				if( rx.byte3 < 6 )
				{
					//pixels.setPixelColor( id, pixels.Color( col_table[col_tab_pos][0], col_table[col_tab_pos][1], col_table[col_tab_pos][2], col_table[col_tab_pos][3] ));
					pixels.setPixelColor( id, pixels.Color( col_table[col_tab_pos][0], col_table[col_tab_pos][1], col_table[col_tab_pos][2] ));
				}
				//rgb_leds_update_flag = true;
				pixels.show();
				break;
			}
			case 5: // update
			{
				//rgb_leds_update_flag = true;
				pixels.show();
				break;
			}
			case 6: // all RGBWs
			{
				for( uint8_t i = 0; i < NUMPIXELS; i++ )
				{
					pixels.setPixelColor(i, pixels.Color( 1, 0, 1 ));
				}
				//rgb_leds_update_flag = true;
				pixels.show();
				break;
			}
			case 7: // ALL OFF
			{
				for( uint8_t i = 0; i < NUMPIXELS; i++ )
				{
					pixels.setPixelColor(i, pixels.Color( 0, 0, 0 ));
				}
				//rgb_leds_update_flag = true;
				pixels.show();
				break;
			}
		}
	}
}


#if 0
int rgb_tgl = 0;
	for( int id=0; id<NUMPIXELS; id++)
	{
		pixels.setPixelColor(id, pixels.Color(rgb_tgl,0,0));
	}
	pixels.show();

	if( rgb_tgl == 1 )
	{
		rgb_tgl = 0;
	}
	else
	{
		rgb_tgl = 1;
	}
#endif



#if 0
void init_hwui()
{
	// sequencer buttons
	for( int i=0; i<N_SEQ_BTN; i++ )
	{
		seq_btn[i].id = i;
		seq_btn[i].cur_state = 0;
		seq_btn[i].prev_state = 0;
		seq_btn[i].debounce_ticks = 0;
	}
	// menu buttons L
	for( int i=0; i<(N_MENU_BTN/2); i++ )
	{
		//menu_btn[i].id = MENU_BTN1_OFS + i;
		menu_btn[i].cur_state = 0;
		menu_btn[i].prev_state = 0;
		menu_btn[i].debounce_ticks = 0;
	}
	// menu buttons R
	for( int i=0; i<(N_MENU_BTN/2); i++ )
	{
		//menu_btn[i+8].id = MENU_BTN2_OFS + i;
		menu_btn[i].cur_state = 0;
		menu_btn[i].prev_state = 0;
		menu_btn[i].debounce_ticks = 0;
	}
	// encoder + encoder buttons
	for( int i=0; i<N_ENC; i++ )
	{
		//enc_btn[i].id = ENC_BTN_OFS + i;
		enc_btn[i].cur_state = 0;
		enc_btn[i].prev_state = 0;
		enc_btn[i].debounce_ticks = 0;
		//encoder[i].id = ENC_OFS + i;
		encoder[i].ph_a = 0;
		encoder[i].ph_b = 0;
	}
}
#endif



/*
// @todo check button order on 165
// simple debouncing mechanism; only button pressing, not releasing - btn 01234567
void btn_check_process(void)
{
	for( uint8_t reg = 0; reg < NUM_REGS; reg++ )
	{
		for( uint8_t btn = 0; btn < 8; btn++ )
		{
			if( ( spi_data[reg] & ( 0b00000001 << btn ) ) == 0 )
			{
				//uint8_t btn_id = 8 * reg + ( 7 - btn );	//0..63
				uint8_t btn_id = 8 * reg + btn;	//0..63

				if( btn_dbc_array[ btn_id ] == 0 )
				{
					midiEventPacket_t event = { 0x09, 0x90, btn_id, 1 };
					// midi is not send here
					MidiUSB.sendMIDI( event );
					usb_midi_tx_msg_cnt++;
					btn_dbc_array[ btn_id ] = BTN_DEAD_TIME;
				}
			}
		}
	}
}
*/


/*
void buttons_check(void)
{
	for( uint8_t id = 0; id < 12; id++ )
	{
		ba[id].cur_state = digitalRead(ba[id].hw_pin);

		if ( ba[id].cur_state == 0)
		{
			if ( ba[id].debounce_ticks == 0 )
			{
				midiEventPacket_t event = {0x09, 0x90, id, 1};
				MidiUSB.sendMIDI(event);
				usb_midi_msg_cnt++;

				ba[id].debounce_ticks = BTN_DEAD_TIME;
			}
		}
	}
} */


/*
void buttons_debounce(void)
{
	for( uint8_t id = 0; id < 12; id++ )
	{
		if( ba[id].debounce_ticks > 0 )
		{
			ba[id].debounce_ticks--;
		}
	}
}*/



/* rgbw 
uint8_t col_table[6][4] = {
	{ 5,  0,  0,  0},	// 0 red	relsub0
	{  0, 5,  0,  0},	// 1 green	relsub1
	{  0,  0, 5,  0},	// 2 blue	relsub2
	{ 4,  0, 4,  0},	// 3 purple	relsub3
	{  0,  0,  0, 50},	// 4 white	stepled
	{  0,  0,  0,  0},	// 5 off
};
*/


