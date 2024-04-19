
# mosaik firmware
- Atmega32U4 (Arduino Micro)
- 
## hw-ui
- 80 RGBW buttons
    -- 64 sequencer buttons
    -- 16 menu buttons
- 7 rotary encoder, including:
    -- switch
    -- 4x RGBW 

## in/out
- 108 RGBW LEDS (Neopixel)
- 13 shift register 74HC165
- 104 inputs total

### inputs
ID     | INPUT              | SPI-REG |
0..63  | seq_btn_1..64      | 0..7  |
64..71 | menu_btn_1..8      | 8     |
72..87 | encoder_1..7 (a/b) | 9, 10 |
88..94 | encoder_btn_1..7   | 11    |
95..103| menu_btn_9..16     | 12    |

### outputs
ID     | OUTPUT RGBW
0..63  | seq_led_1..64
64..79 | menu_led_1..16
80..86 | encoder_led_1..7

## MIDI Messages:
90	Note On			Buttons, LEDs
B0	Control Change	Analog Inputs
F0	Channel Mode	System

## RGBW Commands:
Ch	Cmd
0	R
1	G
2	B
3	W
4	Color Table (val: 0..127)
5	Update
6	All RGBWs
7	All off
NOTE:	Commands via Ch 0..4 need the update command to be shown
        Ch 7 is updated automaticly

# links:
https://www.mikrocontroller.net/articles/Drehgeber
https://www.arduino.cc/reference/en/language/functions/communication/spi/spisettings/

