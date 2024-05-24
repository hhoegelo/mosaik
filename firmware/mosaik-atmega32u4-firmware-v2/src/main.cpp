#include <Arduino.h>
#include <SPI.h>
#include "coos.h"
#include "TimerOne.h"
#include "MIDIUSB.h"
#include "Adafruit_NeoPixel.h"

#define DEBUG 0

// colors
#define _WHT 0
#define _RED 1
#define _GRN 2
#define _BLU 3
#define N_COLORS 50 // see color table

// gpios
#define PIN_LED_HB 5
#define PIN_SYSTICK 6
#define PIN_CPU_IDLE 7
#define PIN_RGB 9

// spi pins
#define SPI_SCK 15
#define SPI_SAP  8
#define SPI_SDI 14

// shift registers
#define N_REG 13
#define N_REG_IO (N_REG*8)
#define N_SEQ_BTN 64
#define N_MENU_BTN 16
#define N_ENC_BTN 7
#define N_BTN (N_SEQ_BTN + N_MENU_BTN + N_ENC_BTN)
#define N_ENC 7

// rgb leds
#define N_RGB 108

// buttons
#define BTN_PRESSED 0
#define BTN_RELEASED 1
#define BTN_DEADTIME_MS 10

// buttons
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

bool systick = false;
uint8_t  usb_midi_tx_msg_cnt = 0;

void timer1_irq(void);
void process_hb(void);
void spi_read(void);
void spi_parse(void);
void enc_check(void);
void btn_check(void);
void process_midi_buf_rx_read(void);

Adafruit_NeoPixel rgb = Adafruit_NeoPixel( N_RGB, PIN_RGB, NEO_GRB + NEO_KHZ800 );

uint8_t col_table[6][3] = {
	{ 50, 0, 0}, // 0 red	relsub0
	{ 0, 0, 80}, // 1 blue	relsub1
	{ 0, 40, 0},	// 2 green	relsub2
	{ 20, 0, 20},	// 3 purple	relsub3
	{ 50, 50, 50},	// 4 white	stepled
	{ 0, 0, 0}	// 5 off
};


uint8_t col_table_new[N_COLORS][3] = {
	// full brightness
	{   0,   0,   0},	// off
	{ 255, 255, 255},	// white 
	{ 255, 255,   0}, 	// yellow
	{ 255,  80,   0},	// orange
	{ 255,   0,   0},	// red
	{ 248,   0,  70},	// magenta
	{   0,   0, 255},	// blue
	{   0, 255, 255},	// light blue
	{   0, 255, 120},	// light green
	{   0, 255,   0},	// green
	// 1/2 brightness
	{   0,   0,   0},	// off
	{ 127, 127, 127},	// white 
	{ 127, 127,   0}, 	// yellow
	{ 127,  40,   0},	// orange
	{ 127,   0,   0},	// red
	{ 124,   0,  35},	// magenta
	{   0,   0, 127},	// blue
	{   0, 127, 127},	// light blue
	{   0, 127,  60},	// light green
	{   0, 127,   0},	// green
	// 1/3 brightness
	{   0,   0,   0},	// off
	{  85,  85,  85},	// white 
	{  85,  85,   0}, 	// yellow
	{  85,  27,   0},	// orange
	{  85,   0,   0},	// red
	{  83,   0,  23},	// magenta
	{   0,   0,  85},	// blue
	{   0,  85,  85},	// light blue
	{   0,  85,  40},	// light green
	{   0,  85,   0},	// green
	// 1/4 brightness
	{   0,   0,   0},	// off
	{  64,  64,  64},	// white 
	{  64,  64,   0}, 	// yellow
	{  64,  20,   0},	// orange
	{  64,   0,   0},	// red
	{  62,   0,  18},	// magenta
	{   0,   0,  64},	// blue
	{   0,  64,  64},	// light blue
	{   0,  64,  30},	// light green
	{   0,  64,   0},	// green
	// 1/6 brightness
	{   0,   0,   0},	// off
	{  42,  42,  42},	// white 
	{  42,  42,   0}, 	// yellow
	{  42,  13,   0},	// orange
	{  50,   0,   0},	// red
	{  41,   0,  12},	// magenta
	{   0,   0,  60},	// blue
	{   0,  42,  42},	// light blue
	{   0,  42,  42},	// light green
	{   0,  42,   0}	// green
	/*
	// 10/th brightness
	{  0,  0,  0},	// off
	{ 25, 25, 25},	// white 
	{ 25, 25,  0}, 	// yellow
	{ 25,  8,  0},	// orange
	{ 25,  0,  0},	// red
	{ 25,  0,  7},	// magenta
	{  0,  0, 25},	// blue
	{  0, 25, 25},	// light blue
	{  0, 25, 12},	// light green
	{  0, 25,  0}	// green
	*/
};

void setup(void) 
{
	// init GPIOs
	pinMode( PIN_LED_HB, OUTPUT );
	pinMode( PIN_SYSTICK, OUTPUT );
	pinMode( PIN_CPU_IDLE, OUTPUT );
	pinMode( SPI_SAP, OUTPUT );
	pinMode( PIN_RGB, OUTPUT );

	// init buttons
	for( int id=0; id<N_BTN; id++ )
	{
		button[id].state = STATE_WAIT_FOR_PRESS;
		button[id].dcnt = 0;
		button[id].val = 1;
	}

	// init RGB leds
	rgb.begin();
	rgb.clear();
	rgb.show();

	// init SPI
	SPI.beginTransaction( SPISettings( 1000000, MSBFIRST, SPI_MODE3 ));  
	SPI.begin();

	// init scheduler
	coos_init();
	#if DEBUG
	coos_task_add( process_hb, 0, 500 ); //only for debugging
	#endif
	coos_task_add( spi_read, 0, 1 );
	coos_task_add( spi_parse, 0, 1 );
	coos_task_add( btn_check, 0, 1 );
	coos_task_add( enc_check, 0, 1 );

	// start scheduler
	Timer1.initialize(2000); // in µs
	Timer1.attachInterrupt(timer1_irq);
}


void loop(void)
{
	if( systick == true )
	{
		systick = false;
		coos_update();

		#if DEBUG
		digitalWrite( PIN_SYSTICK, !digitalRead( PIN_SYSTICK ));
		#endif
	}
	
	coos_dispatch();
	process_midi_buf_rx_read();

	#if DEBUG
	digitalWrite( PIN_CPU_IDLE, !digitalRead( PIN_CPU_IDLE ));
	#endif
}


void timer1_irq()
{
	systick = true;
}


void process_hb(void)
{
	digitalWrite( PIN_LED_HB, !digitalRead( PIN_LED_HB ));
}


void process_midi_buf_rx_read(void)
{
	midiEventPacket_t rx;
	rx = MidiUSB.read();

	if (rx.header == 0x9) // NOTE ON
	{
		uint8_t color = rx.byte1 & 0b00001111;
		uint8_t id = rx.byte2;
		uint8_t col_tab_pos = rx.byte3 & 0b01111111;
		uint8_t prevColor[3] = {};
		uint8_t brightness = rx.byte3 << 1;

		uint32_t prevCol = rgb.getPixelColor(id);
		prevColor[_BLU] = (uint8_t)  prevCol;
		prevColor[_GRN] = (uint8_t) (prevCol >> 8);
		prevColor[_RED] = (uint8_t) (prevCol >> 16);

		switch (color)
		{
			case 1: // 91: red channel
			{
				rgb.setPixelColor(id, rgb.Color(brightness, prevColor[_GRN], prevColor[_BLU]));
				rgb.show();
				break;
			}
			case 2: // 92: green channel
			{
				rgb.setPixelColor(id, rgb.Color(prevColor[_RED], brightness, prevColor[_BLU]));
				rgb.show();
				break;
			}
			case 3: // 93: blue channel
			{
				rgb.setPixelColor(id, rgb.Color(prevColor[_RED], prevColor[_GRN], brightness));
				rgb.show();
				break;
			}
			case 4: // 94: color table
			{
				if( rx.byte3 < N_COLORS )
				{
					//rgb.setPixelColor( id, rgb.Color( col_table[col_tab_pos][0], col_table[col_tab_pos][1], col_table[col_tab_pos][2] ));
					rgb.setPixelColor( id, rgb.Color( col_table_new[col_tab_pos][0], col_table_new[col_tab_pos][1], col_table_new[col_tab_pos][2] ));
				}
				break;
			}
			case 5: // 95: color table: update leds
			{
				rgb.show();
				break;
			}
			case 7: // 97: all leds off
			{
				for( uint8_t i = 0; i < N_RGB; i++ )
				{
					rgb.setPixelColor(i, rgb.Color( 0, 0, 0 ));
				}
				rgb.show();
				break;
			}
		}
	}
}


void spi_read(void)
{
	digitalWrite(SPI_SAP, LOW);
	digitalWrite(SPI_SAP, HIGH);
	
	for( int cnt=0; cnt<N_REG; cnt++ )
	{
		spi_data[ cnt ] = SPI.transfer( cnt );
	}
}


void spi_parse(void)
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


void enc_check(void)
{
	for( int cnt=0; cnt<N_ENC; cnt++)
	{
		int8_t val = encoder[cnt].delta_value;
		encoder[cnt].delta_value = val & 1;
		val >>= 1;

		if( val != 0 )
		{
			midiEventPacket_t event = {0x0B, 0xB0, cnt, 64+val};
			usb_midi_tx_msg_cnt++;
			MidiUSB.sendMIDI(event);
		}
	}
	if ( usb_midi_tx_msg_cnt > 0 )
	{
		MidiUSB.flush();
		usb_midi_tx_msg_cnt = 0;
	}
}


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
					midiEventPacket_t event = { 0x09, 0x90, btn_id, 1 };
					MidiUSB.sendMIDI( event );
					usb_midi_tx_msg_cnt++;
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
					midiEventPacket_t event = { 0x08, 0x80, btn_id, 0 };
					MidiUSB.sendMIDI( event );
					usb_midi_tx_msg_cnt++;
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
	if ( usb_midi_tx_msg_cnt > 0 )
	{
		MidiUSB.flush();
		usb_midi_tx_msg_cnt = 0;
	}
}


