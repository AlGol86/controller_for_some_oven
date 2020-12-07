#include "stm8s.h"
#include "main.h"

void encoder_handler(encoder_t* enc){ 
  char cnt_lim=7;
	int cnt_button_lim=10000;
//encoder
	if(((PORT_enc->IDR&bit_enc_l)==0) && (enc->cnt < cnt_lim)) 
	  enc->cnt++;
  else	if (enc->cnt>0) enc->cnt--;
//button
	if(((PORT_enc->IDR&bit_enc_b)==0) && (enc->cnt_button < cnt_button_lim)) 
	  enc->cnt_button++;
  else	if (enc->cnt_button > 0) 
	        if(enc->cnt_button > cnt_lim) 
					    enc->cnt_button = cnt_lim; 
					else
					 enc->cnt_button--;
//encoder
	if ((enc->cnt >(cnt_lim-2))&&(enc->f_push==0)) {
		  enc->f_push=1;
			if(((PORT_enc->IDR&bit_enc_r)==0)&&(enc->enc_data < enc->enc_data_lim_h)) enc->enc_data++; 
			else if(((PORT_enc->IDR&bit_enc_r)!=0)&&(enc->enc_data > enc->enc_data_lim_l)) enc->enc_data--;
	    }
   if ((enc->cnt==0)&&(enc->cnt_button==0))	
	       {enc->f_push=0;enc->f_long_push=0;}

//button long
  if ((enc->cnt_button >(cnt_button_lim-2))&&(enc->f_long_push==0)){
		enc->f_long_push=1;
		enc->but_data_lim += enc->but_data_lim_long;
		enc->but_data_lim_long =  enc->but_data_lim - enc->but_data_lim_long;
		enc->but_data_lim -= enc->but_data_lim_long;
		oled_blink1(0,127, 30,1);
		enc->but_data=0;
	}

//button short	
	if ((enc->cnt_button >(cnt_lim-2))&&(enc->f_push==0)) {
		enc->f_push=1;
		if (enc->but_data < enc->but_data_lim) enc->but_data++;
		else enc->but_data=0;
	}			
	
}
	
void encoder_setter(signed char lim_l,signed char lim_h,signed char first_value){
		extern encoder_t encoder;	
		encoder.enc_data_lim_l=lim_l;
		encoder.enc_data_lim_h=lim_h;
		encoder.enc_data=first_value;
}