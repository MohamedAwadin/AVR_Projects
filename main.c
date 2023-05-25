
#define  F_CPU    8000000UL
#include <util/delay.h>


#include "MemMap.h"
#include "StdTypes.h"
#include "Utils.h"
#include "DIO_Interface.h"
#include "LCD.h"
#include "KeyPad.h"
#include "Stepeer_Interface.h"
#include "ADC.h"
#include "Sensor.h"

#define  Green PINB0
#define  YELLOW PINB1
#define  RED PINB2
#define  BUZZER   PINB3

void start_system()
{
	u8 password[11] = {0};
	u8 i = 0;		//Just a counter variable
	u8 length_1 = 0;
	lcd_clear();
	lcd_goto(0,3);
	lcd_WriteString("Wellcome");
	lcd_goto(1,0);
	lcd_WriteString("FireAlarmSystem");
	lcd_clear();
	lcd_goto(0,0);
	lcd_WriteString("SetyourPassword");
	lcd_goto(1,0);
	length_1 = Get_pass(password);
	cli();
	
	//Save the length of the password in EEPROM (Location = 0x0001)
	EEPROM_WriteByte(0x0001,length_1);
	
	for(i = 0; i < length_1; i++)
	{
		//Save the password in EEPROM (In location from 12 to 22)
		EEPROM_WriteByte(i+2,password[i]);
	}
	sei();
	
	//Successful operation
	lcd_clear();
	lcd_print("  Your password is");
	lcd_gotoxy(1,2);
	lcd_print("      is saved");
	_delay_ms(1000);
	
}
u8 Get_pass(u8* pass )
{
	u8 x,len=0;
	
	while(1)
	{
		x = KEYPAD_GetEntry();
		while (x!=DEFULT_KEY)
		{
			if(x== 'c')
			{
				return len;
			}
			else if ((x>='0')&&(x<='9'))
			{
				pass[len++]=x;
				//num = (num*10)+(x-'0');
				LCD_WriteChar(x);
			}
		}
	}
}

void sign_up (void)
{
	unsigned char desired_password_1[11] = {0};
	unsigned char length_1 = 0;

	lcd_clear();
	lcd_gotoxy(1,1);
	lcd_print("Your new Password: ");
	lcd_gotoxy(1,2);
	length_1 = get_password(desired_password_1);
	
	
	
	
	if(compare(desired_password_1,desired_password_2,length_1,length_2))
	{
		unsigned char i = 0;		//Just a counter variable
		cli();
		
		//Save the length of the password in EEPROM (Location = 0x0001)
		eeprom_write(0x0001,length_1);
		
		for(i = 0; i < length_1; i++)
		{
			//Save the password in EEPROM (In location from 12 to 22)
			eeprom_write(i+2,desired_password_1[i]);
		}
		sei();
		
		//Successful operation
		lcd_clear();
		lcd_print("  Your password is");
		lcd_gotoxy(1,2);
		lcd_print("      is saved");
		_delay_ms(1000);
	}
	
	else
	{
		//Failed operation, try again
		lcd_clear();
		lcd_gotoxy(1,1);
		lcd_print("Failed operation");
		lcd_gotoxy(1,2);
		lcd_print("Plz, Try again");
		_delay_ms(1000);
		sign_up();
	}
}

int main(void)
{
	DIO_Init();
	LCD_Init();
	ADC_Init(VREF_AVCC,ADC_SCALER_64);
	
	u16 temp;
	u32 per;
	
	
	while(1)
	{
		//x=POT_VoltRead();
		temp = TEMP_Read();
		per =POT_PERRead();
		if (temp < 380)
		{
			LCD_GoTo(0,7);
			LCD_WriteString("         ");
			LCD_GoTo(0,0);
			LCD_WriteString("Fine^-^");
			LCD_GoTo(1,9);
			LCD_WriteString("       ");
			LCD_GoTo(1,0);
			LCD_WriteString("TEMP:");
			LCD_GoTo(1,5);
			LCD_WriteNumber(temp/10);
			LCD_WriteChar('.');
			LCD_WriteNumber(temp%10);
			DIO_WritePin(Green,HIGH);
			DIO_WritePin(YELLOW,LOW);
			DIO_WritePin(RED,LOW);
			DIO_WritePin(BUZZER,LOW);
		}
		else if (temp >= 380)
		{
			DIO_WritePin(Green,LOW);
			if (per < 50 )
			{
				//LCD_Clear();
				DIO_WritePin(BUZZER,LOW);
				LCD_GoTo(0,7);
				LCD_WriteString("         ");
				LCD_GoTo(0,0);
				LCD_WriteString("HEAT^o^");
				LCD_GoTo(1,9);
				LCD_WriteString("       ");
				LCD_GoTo(1,0);
				LCD_WriteString("TEMP:");
				LCD_GoTo(1,5);
				LCD_WriteNumber(temp/10);
				LCD_WriteChar('.');
				LCD_WriteNumber(temp%10);
				DIO_WritePin(YELLOW,HIGH);
				DIO_WritePin(RED,LOW);
				DIO_WritePin(BUZZER,LOW);
			}
			else if (per >= 50)
			{
				//LCD_Clear();
				LCD_GoTo(0,0);
				LCD_WriteString("FIRE!!!FIRE!FIRE");
				LCD_GoTo(1,0);
				LCD_WriteString("TEMP:");
				LCD_GoTo(1,5);
				LCD_WriteNumber(temp/10);
				LCD_WriteChar('.');
				LCD_WriteNumber(temp%10);
				LCD_WriteString("GAS:");
				LCD_GoTo(1,13);
				LCD_WriteNumber(per);
				LCD_WriteChar('%');
				
				DIO_WritePin(BUZZER,HIGH);
				DIO_WritePin(RED,HIGH);
				
				
			}
		}
		
		
		_delay_ms(400);
	}
}
