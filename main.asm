.include "m328pdef.inc"
.include "delay.inc"
.include "UART.inc"
.include "LCD_1602.inc"

.cseg
.org 0x00
	LCD_init			; initilize the 16x2 LCD
	LCD_backlight_ON	; Turn On the LCD Backlight
	ldi r16, (1 << 3)
	sbi DDRB, 3
	
	Serial_begin ; initilize UART serial communication
	
loop:
	call getSerialData
	SBIC PINB, 3
		call displayCountDown
rjmp loop

getSerialData:
	Serial_read
	cpi r20, 0
	brne displayString
	endSerial:
	ret
displayString:
	cpi r20, 0x01
	brne displayIt
	rjmp clearScreen
	displayIt:
	LCD_send_a_character_register r20
	rjmp endSerial

clearScreen:
	LCD_Clear
	rjmp endSerial

displayCountDown:
	LCD_Clear
	LDI ZL, LOW (2 * count_string)
	LDI ZH, HIGH (2 * count_string)
	LCD_send_a_string
	LCD_send_a_command 0xC0
	LDI ZL, LOW (2 * empty_string)
	LDI ZH, HIGH (2 * empty_string)
	LCD_send_a_string
	ldi r16, 9
	countDownStart:
		LCD_send_a_register r16
		dec r16
		LCD_send_a_command 0x10 ; Move Cursor Back
		delay 2000
		cpi r16, 0xFF
		breq countDownEnd
		rjmp countDownStart
	countDownEnd:
		LCD_Clear
		ret

displayRandomString:
	LDI ZL, LOW (2 * random_string)
	LDI ZH, HIGH (2 * random_string)
	LCD_send_a_string

empty_string:	.db	"       ", 0
count_string:	.db	"Counting Down: ", 0
random_string:	.db	"randomString", 0
