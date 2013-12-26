#include "EDC_Sample.h"

int main(void)
{
	char choose;

	do
	{
		system("clear");
		printf(	"\
1.LCD\n\
2.Keypad\n\
3.Thermal printer\n\
4.Device\n\
5.Printer\n\
Q.Back\nInput Working Mode:");
		scanf("%c", &choose);
		getchar();
		printf("------------------------------------------------\n");

		switch(choose)
		{
		case '1':
			RunLCD();
			break;
		case '2':
			RunKeypad();
			break;
		case '3':
			RunTMP();
			break;
		case '4':
			RunDevice();
			break;
		case '5':
			RunPrinter();
			break;
		default:
			break;
		}
	}while('q' != choose && 'Q' != choose);

	return 0;
}

void RunKeypad(void)
{
	PLKPCONTEXT ctx = NULL;
	unsigned char key_val = 0;
	struct timeval TimeStart, TimeEnd;
	float TimeUse = 0.0;
	char input[20] = {'\0'}, KeypadFlay = 0;
	int CharPosition = 0;

	ctx = lkp_create();
	if( !ctx )
	{
		printf( "error - it can not connect to the device.\n");
		goto Exit;
	}

	if( lcd_clean_scr( ctx ) < 0 )	// 清除 LCD 螢幕
		printf( "error - clear lcd's screen.\n" );
	if( lcd_clean( ctx ) < 0 )		// 清除顯示暫存器
		printf( "error - clear buffer.\n" );

	gettimeofday(&TimeStart, NULL);
	gettimeofday(&TimeEnd, NULL);
	while(1)
	{
		if( kpd_get_keypad( ctx, &key_val ) < 0 )
		{
			printf( "error - get key value.\n" );
		}
		else
		{
			if(key_val > 0x03)
			{
				printf( "Keypad value = %02X\n", key_val );
				sprintf(input, "%c", key_val);
				KeypadFlay = 1;
				gettimeofday(&TimeStart, NULL);
			}

			if(key_val == 0x15)
				break;

			if(KeypadFlay == 1)
			{
				//printf("TimeUse: %f\n", TimeUse);
				gettimeofday(&TimeEnd, NULL);
				TimeUse = 1000000 * (TimeEnd.tv_sec - TimeStart.tv_sec) + TimeEnd.tv_usec - TimeStart.tv_usec;
				TimeUse /= 1000000;
				if (TimeUse > 2.0 )
				{
					if( lcd_clean( ctx ) < 0 )		// 清除顯示暫存器
						printf( "error - clear buffer.\n" );

					if( lcd_draw_text_16f(ctx, CharPosition, 0, input, 0) < 0 )
						printf( "error - draw text 16f.\n" );

					if( lcd_print_out( ctx ) < 0 )
						printf( "error - print out.\n" );

					gettimeofday(&TimeStart, NULL);
					KeypadFlay = 0;
					CharPosition += 8;
				} 
			}
		}
		usleep(100000);
	};	//while('q' != choose && 'Q' != choose);

Exit:
	lkp_release( ctx );

	return;
}

void RunTMP(void)
{
	int ret = 0;
	char choose;
	wchar_t ws[100];
	PTMPCONTEXT ctx = NULL;
	unsigned char buf[ 20 ] = {0};

	ctx = tmp_create(500);
	if( !ctx )
	{
		printf( "error - it can not connect to the device.\n");
		goto Exit;
	}

	ret = tmp_set_color_degree(ctx, 10);
	if(ret == 1)
	{
		system("clear");
		printf("No Paper!\n");
		system("read -p \"Press a key to continue...\" -n 1 -s");

		return;
	}

	do
	{
		//system("clear");
		printf("\
1.Chinese Text Print(true type)\n\
2.English Text Print(true type)\n\
3.Icon Draw Print(16x16 bitmap)\n\
Q.Back\nInput select:");
		scanf("%c", &choose);
		getchar();
		printf("------------------------------------------------\n");
		switch(choose)
		{
		case '1':
			if( tmp_clean( ctx ) < 0 )		// 清除暫存區
				printf( "error - clear buffer.\n" );

			// 中文字測試（Truetype 格式）
			if( get_txt_bitmap( "測試", ws ) < 0 )
				printf( "error - Chinese text error.\n");

			if( (ret = tmp_draw_text( ctx, "/home/guest/wqy-microhei.ttc", 0, 15, 36, 36, ws )) < 0 )
				printf( "error %d - draw text.\n", ret );

			if( tmp_print_out( ctx ) < 0 )
				printf( "error - print out.\n" );
			break;

		case '2':
			if( tmp_clean( ctx ) < 0 )		// 清除暫存區
				printf( "error - clear buffer.\n" );

			// 英文字測試（Truetype 格式）
			if( get_txt_bitmap( "Test", ws ) < 0 )
				printf( "error - Englist text error.\n");

			if( (ret = tmp_draw_text( ctx, "/home/guest/wqy-microhei.ttc", 30, 20, 16, 16, ws )) < 0 )
				printf( "error %d - draw text.\n", ret );

			if( tmp_print_out( ctx ) < 0 )
				printf( "error - print out.\n" );
			break;

		case '3':
			buf[0] = 0x00;
			buf[1] = 0x70;	//  111 
			buf[2] = 0x70;	//  111
			buf[3] = 0x10;	//    1
			buf[4] = 0x10;	//    1
			buf[5] = 0xFF;	// 11111111
			buf[6] = 0x7E;	//  111111
			buf[7] = 0x00;

			if( tmp_clean( ctx ) < 0 )		// 清除暫存區
				printf( "error - clear buffer.\n" );

			if( tmp_draw_bmp( ctx, 30, 20, 8, 8, buf ) < 0 )
				printf( "error - draw bmp.\n" );

			if( tmp_print_out( ctx ) < 0 )
				printf( "error - print out.\n" );
			break;
		}
	}while('q' != choose && 'Q' != choose);

Exit:
	tmp_release( ctx );

	return;
}

void RunDevice(void)
{
	int ret = 0, led = 0;
	unsigned char pu8_status = 0xFF;

	ret = device_open();

	if(ret < 0)
		printf("device_open fail!\n");

	ret = device_power( DEVICE_CARD_HOLDER, 0x00 );
	ret = device_power( DEVICE_GPRS_KEY, 0x00 );
	ret = device_power( DEVICE_GPRS_POWER, 0x00 );
	ret = device_power( DEVICE_TMP_POWER, 0x00 );

	if(ret != 0)
		printf("device_power fail!\n");

	while(1)
	{
		ret = device_cardholder_get( &pu8_status );
		if(ret == 0)
			printf("cardholder: %02X\n", pu8_status);
		else
			printf("device cardholder get fail!\n");

		ret = device_detect(&pu8_status);
		printf("device detect: %02X\n", pu8_status);

		if(led == 1)
			device_power( DEVICE_CARD_LED, 0x01 );	// LED ON
		else
			device_power( DEVICE_CARD_LED, 0x00 );	// LED OFF

		led = !led;

		sleep(1);
	}

	return;
}

void RunPrinter(void)
{
	int ret = 0;
	char choose;
	PLKPCONTEXT ctx = NULL;

	ret = ptr_select( 3 , 0);
	if(ret != 0)
	{
		system("clear");
		printf("ptr_select error!\n");
		system("read -p \"Press a key to continue...\" -n 1 -s");

		return;
	}

	ctx = lkp_create();
	if( !ctx )
	{
		printf( "error - it can not connect to the device.\n");
		goto Exit;
	}

	do
	{
		//system("clear");
		printf("\
1.ptr_count_init\n\
2.ptr_count_get\n\
3.ptr_count_stop\n\
Q.Back\nInput select:");
		scanf("%c", &choose);
		getchar();
		printf("------------------------------------------------\n");
		switch(choose)
		{
		case '1':
			if( ptr_count_init( ctx ) < 0 )
			{
				printf( "error - set printer count init.\n" );
			}

			break;

		case '2':
			{
				PRINTERCOUNT_V2 pc;

				if( ptr_count_get( ctx, &pc ) < 0 )
				{
					printf( "error - the printer get count.\n" );
				}
				else
				{
					printf( "count - No color A3 = %d papers.\n", pc.photocopy.u16_gray_scale_a[3] );
					printf( "count - No color A4 = %d papers.\n", pc.photocopy.u16_gray_scale_a[4] );
					printf( "count - Color A3 = %d papers.\n", pc.photocopy.u16_color_a[3] );
					printf( "count - Color A4 = %d papers.\n", pc.photocopy.u16_color_a[4] );

					printf( "count - No color B4 = %d papers.\n", pc.photocopy.u16_gray_scale_b[4] );
					printf( "count - No color B5 = %d papers.\n", pc.photocopy.u16_gray_scale_b[5] );
					printf( "count - Color B4 = %d papers.\n", pc.photocopy.u16_color_b[4] );
					printf( "count - Color B5 = %d papers.\n", pc.photocopy.u16_color_b[5] );

					printf( "count - Print No color A3 = %d papers.\n", pc.print.u16_gray_scale_a[3] );
					printf( "count - Print No color A4 = %d papers.\n", pc.print.u16_gray_scale_a[4] );
					printf( "count - Print Color A3 = %d papers.\n", pc.print.u16_color_a[3] );
					printf( "count - Print Color A4 = %d papers.\n", pc.print.u16_color_a[4] );

					printf( "count - Print No color B4 = %d papers.\n", pc.print.u16_gray_scale_b[4] );
					printf( "count - Print No color B5 = %d papers.\n", pc.print.u16_gray_scale_b[5] );
					printf( "count - Print Color B4 = %d papers.\n", pc.print.u16_color_b[4] );
					printf( "count - Print Color B5 = %d papers.\n", pc.print.u16_color_b[5] );

					printf( "count - Scan No color A3 = %d papers.\n", pc.scan.u16_gray_scale_a[3] );
					printf( "count - Scan No color A4 = %d papers.\n", pc.scan.u16_gray_scale_a[4] );
					printf( "count - Scan Color A3 = %d papers.\n", pc.scan.u16_color_a[3] );
					printf( "count - Scan Color A4 = %d papers.\n", pc.scan.u16_color_a[4] );

					printf( "count - Scan No color B4 = %d papers.\n", pc.scan.u16_gray_scale_b[4] );
					printf( "count - Scan No color B5 = %d papers.\n", pc.scan.u16_gray_scale_b[5] );
					printf( "count - Scan Color B4 = %d papers.\n", pc.scan.u16_color_b[4] );
					printf( "count - Scan Color B5 = %d papers.\n", pc.scan.u16_color_b[5] );

					printf( "work status = %d.\n", pc.u8_work_status );
				}
			}
			break;

		case '3':
			if( ptr_count_stop( ctx ) < 0 )
			{
				printf( "error - the printer get count stop.\n" );
			}
			break;
		}
	}while('q' != choose && 'Q' != choose);

Exit:
	lkp_release( ctx );

	return;
}
// 使用 Truetype 文字時需使用此函式做轉換
int get_txt_bitmap( char *str, wchar_t *ws )
{
	int		i;
#if !defined(__CYGWIN__) && !defined( WIN32 )
	int		j;
	unsigned short us[ 100 ];
	FILE *f = NULL;
	char sz_exec[ 80 ];
#endif


#if defined(__CYGWIN__) || defined( WIN32 )
	if(setlocale(LC_ALL, "zh_TW.Big5") == NULL)
	{
		printf("error - setlocale\n");
		return -1;
	}

	i = mbstowcs( ws, str, strlen( str ) );
#else
	sprintf( sz_exec, "echo -n \"%s\" | ./iconv  -f BIG-5 -t UCS2", str );
	f = popen( sz_exec, "r" );
	i = fread( us, sizeof( short ), 100, f );
	for( j=0; j<i; j++ )
	{
		ws[ j ] = us[ j ];
	}
	ws[ i ] = (wchar_t)NULL;

	pclose( f );
#endif

	return 0;
}

void RunLCD(void)
{
	int ret = 0;
	char choose;
	wchar_t ws[100];
	PLKPCONTEXT ctx = NULL;
	unsigned char buf[ 20 ] = {0};

	ctx = lkp_create();
	if( !ctx )
	{
		printf( "error - it can not connect to the device.\n");
		goto Exit;
	}

	do{
		//system("clear");
		printf("\
			   1.Chinese Text Display(true type)\n\
			   2.Chinese Text Display(16x16 bitmap)\n\
			   3.English Text Display(true type)\n\
			   4.English Text Display(16x16 bitmap)\n\
			   5.Icon Draw Display(16x16 bitmap)\n\
			   Q.Back\nInput select:");
		scanf("%c", &choose);
		getchar();
		printf("------------------------------------------------\n");
		switch(choose)
		{
		case '1':
			if( lcd_clean_scr( ctx ) < 0 )	// 清除 LCD 螢幕
				printf( "error - clear lcd's screen.\n" );
			if( lcd_clean( ctx ) < 0 )		// 清除顯示暫存器
				printf( "error - clear buffer.\n" );

			// 中文字測試（Truetype 格式）
			if( get_txt_bitmap( "測試", ws ) < 0 )
				printf( "error - Chinese text error.\n");

			if( (ret = lcd_draw_text( ctx, "/home/guest/wqy-microhei.ttc", 20, 15, 32, 16, ws, 0 )) < 0 )
				printf( "error %d - draw text.\n", ret );

			if( lcd_print_out( ctx ) < 0 )
				printf( "error - print out.\n" );
			break;

		case '2':
			if( lcd_clean_scr( ctx ) < 0 )	// 清除 LCD 螢幕
				printf( "error - clear lcd's screen.\n" );
			if( lcd_clean( ctx ) < 0 )		// 清除顯示暫存器
				printf( "error - clear buffer.\n" );

			// 中文字測試（16x16 bitmap格式）
			if( lcd_draw_text_16f(ctx, 30, 20, "中文測試", 0) < 0 )
				printf( "error - draw text 16f.\n" );

			if( lcd_print_out( ctx ) < 0 )
				printf( "error - print out.\n" );
			break;

		case '3':
			if( lcd_clean_scr( ctx ) < 0 )	// 清除 LCD 螢幕
				printf( "error - clear lcd's screen.\n" );
			if( lcd_clean( ctx ) < 0 )		// 清除顯示暫存器
				printf( "error - clear buffer.\n" );

			// 英文字測試（Truetype 格式）
			if( get_txt_bitmap( "Test", ws ) < 0 )
				printf( "error - Englist text error.\n");

			if( (ret = lcd_draw_text( ctx, "/home/guest/wqy-microhei.ttc", 20, 10, 32, 16, ws, 0 )) < 0 )
				printf( "error %d - draw text.\n", ret );

			if( lcd_print_out( ctx ) < 0 )
				printf( "error - print out.\n" );
			break;

		case '4':
			if( lcd_clean_scr( ctx ) < 0 )	// 清除 LCD 螢幕
				printf( "error - clear lcd's screen.\n" );
			if( lcd_clean( ctx ) < 0 )		// 清除顯示暫存器
				printf( "error - clear buffer.\n" );

			// 英文字測試（16x16 bitmap格式）
			if( lcd_draw_text_16f(ctx, 13, 20, "English Test", 0) < 0 )
				printf( "error - draw text 16f.\n" );

			if( lcd_print_out( ctx ) < 0 )
				printf( "error - print out.\n" );
			break;

		case '5':
			buf[0] = 0x00;
			buf[1] = 0x70;	//  111 
			buf[2] = 0x70;	//  111
			buf[3] = 0x10;	//    1
			buf[4] = 0x10;	//    1
			buf[5] = 0xFF;	// 11111111
			buf[6] = 0x7E;	//  111111
			buf[7] = 0x00;

			if( lcd_clean_scr( ctx ) < 0 )	// 清除 LCD 螢幕
				printf( "error - clear lcd's screen.\n" );
			if( lcd_clean( ctx ) < 0 )		// 清除顯示暫存器
				printf( "error - clear buffer.\n" );

			if( lcd_draw_bmp( ctx, 30, 20, 8, 8, buf ) < 0 )
				printf( "error - draw bmp.\n" );

			if( lcd_print_out( ctx ) < 0 )
				printf( "error - print out.\n" );
			break;
		}
	}while('q' != choose && 'Q' != choose);

Exit:
	lkp_release( ctx );
}
