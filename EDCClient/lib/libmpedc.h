#ifndef __LIBMPEDC__
#define	__LIBMPEDC__

#include <stdio.h>
#include <stdlib.h>
#include <wctype.h>
#include <wchar.h>
#include <locale.h>
#include <string.h>
#include <unistd.h>

#ifndef __out
	#define		__out
#endif

#ifndef __in
	#define		__in
#endif

#define		MAX_ROW		(128)

typedef struct
{
	unsigned int	size;
	unsigned int	col;
	unsigned long long context[ 0 ][ 6 ];
} TMPCONTEXT, *PTMPCONTEXT;

typedef struct
{
	unsigned int	size;
	unsigned int	row;
	unsigned char context[3][ MAX_ROW * 8 ];
} LKPCONTEXT, *PLKPCONTEXT;

typedef struct
{
	unsigned short		u16_no_color_a4;
	unsigned short		u16_no_color_a3;
	unsigned short		u16_color_a4;
	unsigned short		u16_color_a3;
	unsigned short		u16_dbl_no_color_a4;
	unsigned short		u16_dbl_no_color_a3;
	unsigned short		u16_dbl_color_a4;
	unsigned short		u16_dbl_color_a3;
	unsigned char		u8_work_status;
} __attribute__((packed)) PRINTERCOUNT, *PPRINTERCOUNT;

typedef struct
{
	// 單面
	unsigned short u16_gray_scale_a[11];
	unsigned short u16_gray_scale_b[11];
	unsigned short u16_color_a[11];
	unsigned short u16_color_b[11];

	// 雙面
	unsigned short u16_double_gray_scale_a[11];
	unsigned short u16_double_gray_scale_b[11];
	unsigned short u16_double_color_a[11];
	unsigned short u16_double_color_b[11];

} __attribute__((packed)) PRINTERTYPE, *PPRINTERTYPE;

typedef struct
{
	// 影印
	PRINTERTYPE photocopy;
	// 列印
	PRINTERTYPE print;
	// 傳真
	PRINTERTYPE fax;
	// 掃描
	PRINTERTYPE scan;

	unsigned char u8_work_status;
} __attribute__((packed)) PRINTERCOUNT_V2, *PPRINTERCOUNT_V2;


enum DEVICE
{
	DEVICE_CARD_LED			= 1,	// Card entry LED
	DEVICE_CARD_HOLDER		= 6,	// Card holder lock/unlock
	DEVICE_GPRS_KEY			= 8,	// GPRS ACM8066 ON Key
	DEVICE_GPRS_POWER		= 10,	// GPRS power
	DEVICE_TMP_POWER		= 12,	// Thermo printer power

	DEVICE_BUZZER_2			= 19,	// Buzzer 2
	DEVICE_CARD_LED_BLUE	= 20,	// Card entry LED Blue
	DEVICE_CARD_LED_RED		= 21,	// Card entry LED Red
	DEVICE_CARD_LED_YELLOW	= 22,	// Card entry LED Yellow
	DEVICE_CARD_LED_GREEN	= 23,	// Card entry LED Green
};

enum ERR
{
	ERR_SAM_COVER_OPEN		=  4,	// SAM Cover Open
	ERR_CHASSIS_OPEN		=  2,	// Chassis Open
	ERR_GPRS_SD_COVER_OPEN	=  1,	// GPRS/SD Cover Open
	ERR_NO_PAPER			=  1,	// No Paper
	ERR_SUCCESS				=  0,	// Success
	ERR_SYSTEM				= -1,	// Unknown error
	ERR_DEVICE				= -2,	// Device Uninitialized
	ERR_TMP					= -3,	// TMPConterxt not created
	ERR_LKP					= -4,	// LKPConterxt not created
	ERR_TMP_NRES			= -5,	// Thermal printer no response
	ERR_RANGE				= -6,	// Length, width is out of Range
	ERR_FILE				= -7,	// File Error
	ERR_LCD_NRES			= -8,	// LCD no response
	ERR_PTR_NRES			= -9,	// Photostat no response
	ERR_NO_FONT				= -10,	// No font
	ERR_PARAM				= -11,	// Param error
};

enum BUTTON
{
	BUTTON_FN			= 0x10,	// Fn
	BUTTON_UP			= 0x11,	// Up
	BUTTON_DOWN			= 0x12,	// Down
	BUTTON_POWER		= 0x13,	// Power
	BUTTON_CLEAR		= 0x14,	// 清除
	BUTTON_CANCEL		= 0x15,	// 取消
	BUTTON_ENTER		= 0x16,	// 確定
	BUTTON_EJECT		= 0x17,	// 退卡
	BUTTON_ALT			= 0x18,	// 切換
};

#ifdef __cplusplus
extern "C"{
#endif

/*
	explain	| Creating new context of the thermal-printer
	-------------------------------------
	param	| i32_column - mallocing the column number
	-------------------------------------
	return	| true  = not NULL
			| false = NULL
*/
PTMPCONTEXT tmp_create( int i32_column );

/*
	explain	| releasing the context of the thermal-printer
	-------------------------------------
	param	| tmp_ctx - the context is malloced
	-------------------------------------
	return	| nothing
*/
void tmp_release( __in PTMPCONTEXT tmp_ctx );

/*
	explain	| Read thermal-printer's status
	-------------------------------------
	param	| tmp_ctx - the context is malloced
	-------------------------------------
	return	| 1   = no paper 
			| 0   = normal
			| <0  = fail
*/
int tmp_read_status( __in PTMPCONTEXT tmp_ctx );

/*
	explain	| Printing out of the context of the thermal-printer
	-------------------------------------
	param	| tmp_ctx - the context is malloced
	-------------------------------------
	return	| true	: >= zero
			| false : < zero
*/
int tmp_print_out( __in PTMPCONTEXT tmp_ctx );

/*
	explain	| The motor of the thermal-printer
	-------------------------------------
	param	| tmp_ctx - the context is malloced
			| u16_times - the motor's times
	-------------------------------------
	return	| true	: >= zero
			| false : < zero
*/
int tmp_motor_out( __in PTMPCONTEXT tmp_ctx, unsigned short u16_times );

/*
	explain	| Set the color-degree of the thermal-printer
	-------------------------------------
	param	| tmp_ctx - the context is malloced
			| u8_u8_degree - the degree 0~255
	-------------------------------------
	return	| true	: >= zero
			| false : < zero
*/
int tmp_set_color_degree( __in PTMPCONTEXT tmp_ctx, unsigned char u8_degree );

/*
	explain	| Cleaning of the context of the thermal-printer
	-------------------------------------
	param	| tmp_ctx - the context is malloced
	-------------------------------------
	return	| true	: >= zero
			| false : < zero
*/
int tmp_clean( __in PTMPCONTEXT tmp_ctx );

/*
	explain	| Drawing the string
	-------------------------------------
	param	| tmp_ctx - the context is malloced
			| font_file - the file's name which is the font
			| x - position-x starts printing
			| y - position-y starts printing
			| width - the width of per words
			| height - the height of per words
			| str_text - the text will print out
	-------------------------------------
	return	| true	: >= zero
			| false : < zero
*/
int tmp_draw_text( __in PTMPCONTEXT tmp_ctx, char *str_font_file, unsigned int u32_x, unsigned int u32_y, unsigned int u32_width, unsigned int u32_height, __in const wchar_t *str_text );

/*
	explain | Drawing the bitmap
	-------------------------------------
	param	| tmp_ctx - the context is malloced
			| u32_x - position-x starts printing
			| u32_y - position-y starts printing
			| u32_width - the width of per words
			| u32_height - the height of per words
			| bmp - the bitmap will print out
	-------------------------------------
	return	| true	: >= zero
			| false : < zero
*/
int tmp_draw_bmp( __in PTMPCONTEXT tmp_ctx, unsigned int u32_x, unsigned int u32_y, unsigned int u32_width, unsigned int u32_height, __in void *bmp );

/*
	explain | Get Ttermal printer's version.
	-------------------------------------
	param	| pu8_version = version number.
	-------------------------------------
	return	| true	: >= zero
			| false : < zero
*/
int tmp_get_version( __in PTMPCONTEXT tmp_ctx, unsigned char *pu8_version );

/*
	explain	| Creating new context of the lkp
	-------------------------------------
	return	| true  = not NULL
			| false = NULL
*/
PLKPCONTEXT lkp_create( void );

/*
	explain	| releasing the context of the lkp
	-------------------------------------
	param	| lkp_ctx - the context is malloced
	-------------------------------------
	return	| nothing
*/
void lkp_release( __in PLKPCONTEXT lkp_ctx );

/*
	explain	| Printing out of the context of the lkp
	-------------------------------------
	param	| lkp_ctx - the context is malloced
	-------------------------------------
	return	| true	: >= zero
			| false : < zero
*/
int lcd_print_out( __in PLKPCONTEXT lkp_ctx );

/*
	explain	| Cleaning of the context of the lkp
	-------------------------------------
	param	| lkp_ctx - the context is malloced
	-------------------------------------
	return	| true	: >= zero
			| false : < zero
*/
int lcd_clean( __in PLKPCONTEXT lkp_ctx );

/*
	explain	| Cleaning of the context of the specified lkp
	-------------------------------------
	param	| lkp_ctx - the context is malloced
			| u32_x - position-x starts clear
			| u32_y - position-y starts clear
			| u32_width - the width of buffer
			| u32_height - the height of buffer
	-------------------------------------
	return	| true	: >= zero
			| false : < zero
*/
int lcd_clean_buffer( __in PLKPCONTEXT lkp_ctx, unsigned int u32_x, unsigned int u32_y, unsigned int u32_width, unsigned int u32_height );
/*
	explain	| Cleaning of the screen of the LCD
	-------------------------------------
	param	| lkp_ctx - the context is malloced
	-------------------------------------
	return	| true	: >= zero
			| false : < zero
*/
int lcd_clean_scr(  __in PLKPCONTEXT lkp_ctx );

/*
	explain	| Drawing the string
	-------------------------------------
	param	| lkp_ctx - the context is malloced
			| font_file - the file's name which is the font
			| u32_x - position-x starts printing
			| u32_y - position-y starts printing
			| u32_width - the width of per words
			| u32_height - the height of per words
			| str_text - the text will print out
			| u8_hightlight - hightlight word
	-------------------------------------
	return	| true	: >= zero
			| false : < zero
*/
int lcd_draw_text( __in PLKPCONTEXT lkp_ctx, char *str_font_file, unsigned int u32_x, unsigned int u32_y, unsigned int u32_width, unsigned int u32_height, __in const wchar_t *str_text, unsigned char u8_hightlight );

/*
	explain	| Drawing the string with 16x16
	-------------------------------------
	param	| lkp_ctx - the context is malloced
			| u32_x - position-x starts printing
			| u32_y - position-y starts printing
			| str_text - the text will print out
			| u8_hightlight - hightlight word
	-------------------------------------
	return	| true	: >= zero
			| false : < zero
*/
int lcd_draw_text_16f( __in PLKPCONTEXT lkp_ctx, unsigned int u32_x, unsigned int u32_y, __in const char *str_tex, unsigned char u8_hightlightt );

/*
	explain	| Drawing the string with 16x16
	-------------------------------------
	param	| lkp_ctx - the context is malloced
			| u32_x - position-x starts printing
			| u32_y - position-y starts printing
			| str_text - the text will print out
			| u8_hightlight - hightlight word
			| font_file - set the font file path.
	-------------------------------------
	return	| true	: >= zero
			| false : < zero
*/
int lcd_draw_text_16f_Path(  __in PLKPCONTEXT lkp_ctx, unsigned int u32_x, unsigned int u32_y, __in const char *str_text, unsigned char u8_hightlight, char *font_file );

/*
	explain | Drawing the bitmap
	-------------------------------------
	param	| lkp_ctx - the context is malloced
			| u32_x - position-x starts printing
			| u32_y - position-y starts printing
			| width - the width of per words
			| height - the height of per words
			| bmp - the bitmap will print out
	-------------------------------------
	return	| true	: >= zero
			| false : < zero
*/
int lcd_draw_bmp( __in PLKPCONTEXT lkp_ctx, unsigned int u32_x, unsigned int u32_y, unsigned int u32_width, unsigned int u32_height, __in void *bmp );

/*
	explain | Get key value of the keypad
	-------------------------------------
	param	| lkp_ctx - the context is malloced
			| pu8_key - the key's value
	-------------------------------------
	return	| true	: >= zero
			| false : < zero
*/
int kpd_get_keypad( __in PLKPCONTEXT lkp_ctx, unsigned char *pu8_key );

/*
	explain | Set the backlight of the lcd or keypad
	-------------------------------------
	param	| lkp_ctx - the context is malloced
			| u8_type - the backlight type of the lcd or keypad
	-------------------------------------
	return	| true	: >= zero
			| false : < zero
*/
int lkp_set_backlight( __in PLKPCONTEXT lkp_ctx, unsigned char u8_type );

/*
	explain | Power off after the second
	-------------------------------------
	param	| lkp_ctx - the context is malloced
			| u8_sec - power off after this moment
	-------------------------------------
	return	| true	: >= zero
			| false : < zero
*/
int lkp_poweroff( __in PLKPCONTEXT lkp_ctx, unsigned char u8_sec );

/*
	explain | Get LCD-Keypad-printer board's version.
	-------------------------------------
	param	| pu8_version = version number.
	-------------------------------------
	return	| true	: >= zero
			| false : < zero
*/
int lkp_get_version( __in PLKPCONTEXT lkp_ctx, unsigned char *pu8_version );

/*
	explain | When power off the last state of the cover.
	-------------------------------------
	param	| pu8_status = cover state.
			|
			| 7 6 5 4 3 2 1 0 bit (1Byte)
			| R R R R R X X X 
			|
			|	bit 0 : GPRS / SD Cover
			|	bit 1 : Chassis
			|	bit 2 : SAM Cover
			|
			|	X: 0 - Close
			|	   1 - Open
			|	R: Reserve
	-------------------------------------
	return	| true	: >= zero
			| false : < zero
*/
int lkp_cover_last_status( __in PLKPCONTEXT lkp_ctx, unsigned char *pu8_status );

/*
	explain | Clean last state of the cover.
	-------------------------------------
	return	| true	: >= zero
			| false : < zero
*/
int lkp_clear_last_status( __in PLKPCONTEXT lkp_ctx );

/*
	explain | Printer get count init
	-------------------------------------
	param	| lkp_ctx - the context is malloced
	-------------------------------------
	return	| true	: >= zero
			| false : < zero
*/
int ptr_count_init( __in PLKPCONTEXT lkp_ctx );

/*
	explain | Printer get paper count
	-------------------------------------
	param	| lkp_ctx - the context is malloced
			| pc_ppc  - PRINTERCOUNT of paper count
	-------------------------------------
	return	| true	: >= zero
			| false : < zero
*/
int ptr_count_get( __in PLKPCONTEXT lkp_ctx, PPRINTERCOUNT_V2 pc_ppc );

/*
	explain | Printer get count stop
	-------------------------------------
	param	| lkp_ctx - the context is malloced
	-------------------------------------
	return	| true	: >= zero
			| false : < zero
*/
int ptr_count_stop( __in PLKPCONTEXT lkp_ctx );

/*
	explain | Printer select
	-------------------------------------
	param	| u8_select - select use printer.
	-------------------------------------
	return	| true	: >= zero
			| false : < zero
*/
int ptr_select( unsigned char u8_com_select );

/*
	explain | Set DO status
	-------------------------------------
	param	| lkp_ctx - the context is malloced
			| u8_status - Set IO Status
			|
			| 7 6 5 4 3 2 1 0 bit (1Byte)
			| R R R R R R X X 
			| bit 0 : DO 1
			|     1 : DO 2
			|
			|  X: 0 - Low
			|     1 - Hi
			|  R: Reserve
	-------------------------------------
	return	| true	: >= zero
			| false : < zero
*/
int io_set_out( __in PLKPCONTEXT lkp_ctx, unsigned char u8_status );

/*
	explain | Read DI status
	-------------------------------------
	param	| lkp_ctx - the context is malloced
	-------------------------------------
	return	| 7 6 5 4 3 2 1 0 bit (1Byte)
			| R R R X X X X X 
			|
			| bit 0 : DI 1
			|     1 : DI 2 (影印機定義為)市電狀態 0-有電 1-斷電
			|     2 : DI 3
			|     3 : DI 4
			|     4 : DI 5
			|
			|  X: 0 - Low
			|     1 - Hi
			|  R: Reserve
*/
int io_read_in( __in PLKPCONTEXT lkp_ctx );

/*
	explain | Open the device's service.
	-------------------------------------
	return	| true	: >= zero
			| false : < zero
*/
int device_open( void );

/*
	explain | Close the device's service.
	-------------------------------------
	return	| none
*/
void device_close( void );

/*
	explain | Control the device's power.
	-------------------------------------
	param	| u8_device - the device number.
			| u8_status - set power status.
	-------------------------------------
	return	| true	: >= zero
			| false : < zero
*/
int device_power( unsigned char u8_device, unsigned char u8_status );

/*
	explain | Get the card-holder's status.
	-------------------------------------
	param	| pu8_status - get status.
			|	open = 1;
			|	close = 0;
	-------------------------------------
	return	| true	: >= zero
			| false : < zero
*/
int device_cardholder_get( unsigned char *pu8_status );

/*
	explain | Detect the EDC is opened or not.
	-------------------------------------
	param	| u8_status - get status.
			|	bit 0 - GPRS+SD Cover sensor
			|	bit 1 - Chassis sensor
			|	bit 2 - SAM Cover sensor
			|		1 : open
			|		0 : close
	-------------------------------------
	return	| true	: >= zero.
			| false : < zero.
*/
int device_detect( unsigned char *pu8_status );

/*
	explain | Eject Card
	-------------------------------------
	return	| true	: >= zero
			| false : < zero
*/
int device_eject_card( void );

/*
	explain | Get Api Version
	-------------------------------------
	param	| str_version - api version
	-------------------------------------
	return	| true	: >= zero
			| false : < zero
*/
int lnx_get_api_version( char *str_version );

#ifdef __cplusplus
}
#endif

#endif
