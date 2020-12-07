#include "stm8s.h"
#include "main.h"

void init_tim1(char ccr1h)//PWM, time_counter, time_management
{
	TIM1->IER|=TIM1_IER_UIE;
	TIM1->IER|=TIM1_IER_CC1IE;
  TIM1->PSCRH=0;
	TIM1->PSCRL=14; 
	TIM1->ARRH=255; 
  TIM1->ARRL=0;
	TIM1->CCR1H=ccr1h;
	TIM1->CCR1L=100;
	TIM1->CNTRH=0;
	TIM1->CNTRL=0;
	rim();
	TIM1->CR1|=TIM1_CR1_CEN;
}

void init_tim2 (void) {//encoder_poller
	TIM2->IER|=TIM2_IER_UIE;
	//TIM2->IER|=TIM1_IER_CC1IE;
  TIM2->PSCR=0;
	TIM2->ARRH=2; 
  TIM2->ARRL=20;
	TIM2->CNTRH=0;
	TIM2->CNTRL=0;
	rim();
	TIM2->CR1|=TIM2_CR1_CEN;
  }
	
void out_init(out_data_t pin_out,out_data_t od_pp_out) {
  switch(pin_out){
	  case c3: {
	  GPIOC->DDR|=1<<3;
	  GPIOC->CR1|=1<<3;
	  GPIOC->CR2|=1<<3;
	  GPIOC->ODR&=~(1<<3);
	  break;}
		case b5: {
			if(od_pp_out==pp){      //push-pull
	        GPIOB->DDR|=1<<5; 
	        GPIOB->CR1|=1<<5;
	        GPIOB->CR2|=1<<5;
	        GPIOB->ODR&=~(1<<5);
      }
			else{                  //open drain
				  GPIOB->ODR|=1<<5;
				  GPIOB->CR1&=~(1<<5);
					GPIOB->DDR|=1<<5;
	        GPIOB->CR2|=1<<5;
		  }    
		  break;}
		}
}
	
void init_encoder(encoder_t* enc,signed char l,signed char h, char but_lim,char but_lim_long){
	PORT_enc->CR2&=~(bit_enc_l|bit_enc_r|bit_enc_b);
	PORT_enc->DDR&=~(bit_enc_l|bit_enc_r|bit_enc_b);
	PORT_enc->CR1|=(bit_enc_l|bit_enc_r|bit_enc_b);
	
	enc->enc_data_lim_h=h;
	enc->enc_data_lim_l=l;
	enc->enc_data=l;
	enc->but_data_lim=but_lim;
	enc->but_data_lim_long=but_lim_long;
}

//(if there are some issues - put eeprom setters in the begining of the program)
	//write_eeprom(&eeprom_data.power_0,66);
	//write_eeprom(&eeprom_data.power_control_mode,0);
	//write_eeprom(&eeprom_data.kp,30);
	//write_eeprom(&eeprom_data.ki,7);
	//write_eeprom(&eeprom_data.kd,1);
	//write_eeprom(&eeprom_data.kki,1);