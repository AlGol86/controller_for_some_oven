#include "stm8s.h"
#include "main.h"

///////////////////////////////////////////////////////////////////
////////////////CALCULATE POWER////////////////////////////////////
char calc_PWM(void){
	extern temp_data_t temp_data;
	extern eeprom_data_t eeprom_data @0x4010;
	int dt,dt_dt,power;
	

		
	dt=(eeprom_data.preset_temp<<1)-temp_data.t1; //temp error
	dt_dt=(temp_data.t1-temp_data.previous_temp); //speed of temp_change
	temp_data.err_temp_integral+=dt*10/eeprom_data.kki ;//integral of temp_error
	if(temp_data.err_temp_integral>300) 
	temp_data.err_temp_integral=300;
	else if(temp_data.err_temp_integral<-300)temp_data.err_temp_integral=-300;
	
	temp_data.t_p1=(dt*50)/eeprom_data.kp;
	temp_data.t_p2=(50*temp_data.err_temp_integral)/eeprom_data.ki/eeprom_data.kp;
	temp_data.t_p3=(eeprom_data.kd*50*dt_dt)/eeprom_data.kp;
	power=temp_data.t_p1  +  temp_data.t_p2  -  temp_data.t_p3;
	/////proportional//integral///////////////////////////////////////differencial////////
	if(power<0) power=0;
	else if(power>=100) power=99;
	return power;
}

void get_temperature_data(void){
		extern temp_data_t temp_data;
		temp_data.previous_temp=temp_data.t1;	
		temp_data.t2=get_temp_8(address_iic_t1);
		temp_data.t3=get_temp_8(address_iic_t2);
		temp_data.t5=get_temp_8(address_iic_t3);
		temp_data.t6=get_temp_8(address_iic_t4);
		temp_data.r1=resistance_calculation ( 7500, get_ADC(3));
		temp_data.r2=resistance_calculation ( 7500, get_ADC(4));
		temp_data.t1=get_temperature_ADC(temp_data.r1);	
		temp_data.t4=get_temperature_ADC(temp_data.r2);	
}

char get_temperature_ADC(int r_value){
	long int a,b,c,d,x;
	char i;
	char t;
	long int dr,dr_min;
	a=-28;
	b=15417;
	c=-2972800;
	d=212060000;
	dr_min=2000000000;
	
	
	for(i=40;i<240;i++)
	{
	x=(a*i*i*i+b*i*i+c*i+d);
	dr=(x/10000)-r_value;
	dr*=dr;
  if(dr<dr_min){dr_min=dr;t=i;}
  }
	return t-t/25;
}
	
void store_t1_in_array(void){
		extern @near char array_data_temp [256];
		extern temp_data_t temp_data;
		char i;
		for(i=0;i<255;i++){
			array_data_temp[i]=array_data_temp[i+1];
		}
		array_data_temp[255]=temp_data.t1;
		
	}
	////////////////////////////////////////////////
	void init_ADC_3456(void)
{
  GPIOD->DDR&=~BIT_MASK_ch3456 ;
  GPIOD->CR1&=~BIT_MASK_ch3456 ;
  GPIOD->CR2&=~BIT_MASK_ch3456 ;
  ADC1->CR1&=~ADC1_CR1_CONT;
}
 
unsigned int get_ADC(char ch)
{
char h=0;
char l=0;
  ADC1->CSR=ADC1_CSR_CH&ch;
  ADC1->CR1|=ADC1_CR1_ADON; 
  ADC1->CR1|=ADC1_CR1_ADON;  //dubble 'ADON" for switch on ADC
while((ADC1->CSR&ADC1_CSR_EOC)==0)
  {nop();}

ADC1->CSR&=~ADC1_CSR_EOC;
h=ADC1->DRH;
l=ADC1->DRL;
return ((h<<2)|l); 
}

unsigned int resistance_calculation ( unsigned int r_value, unsigned int value_ADC)
{
  return(((unsigned long int)r_value*value_ADC)/(1024-value_ADC));
}

unsigned int get_temp_ADC(char ch)
{
unsigned long int r;
unsigned long int t;
unsigned int adc;
adc=get_ADC(ch);
r = resistance_calculation(1500,adc);  //r=xxxx -> xxx.x Ohm R=150.0 Ohm
t=(unsigned long int)97*r*r+(unsigned long int)2325000*r-(unsigned long int)2422000000;
t=t/10000000;
return t;
}

///save to eeprom
void write_eeprom(char* addr_eeprom,char data)
{ sim();
  	if (!((FLASH->IAPSR) & (FLASH_IAPSR_DUL)))
    {  // unlock EEPROM
       FLASH->DUKR = 0xAE;
       FLASH->DUKR = 0x56;
    }
  rim();
       while (!((FLASH->IAPSR) & (FLASH_IAPSR_DUL))) {;}
       *addr_eeprom=data;
       FLASH->IAPSR &= ~(FLASH_IAPSR_DUL); // lock EEPROM
}

	
	