#include "stm8s.h"
#include "main.h"
encoder_t 				encoder;
temp_data_t 			temp_data;
display_buffer_t 	db;	
sys_time_t 				sys_time;
output_power_t 		output_power;
@near char			  array_data_temp [256];//store temperanure every 5 sec
menu_t 						menu,previous_menu;
char cmf;                               //changed menu flag
eeprom_data_t eeprom_data @0x4010;


main()
{
	char t_p1,t_p2,t_p3;
	out_init(b5,od);                 //PWM_out_init
	init_ADC_3456();                  //ADC_init
	init_encoder(&encoder,10,20,2,9);//lim_l,lim_h,lim_data-for button,long_lim_data-for button
	init_tim2();                     //encoder
	init_tim1(50);                    //load on-off PWM %
	init_ssd1306_2(address_iic_oled);	//OLED display initialization
	previous_menu=1;
	output_power.previous_power=100;
  start_time_while(task_get_temp,1);
	start_time_while(task_set_power,4);
	start_time_while(task_store_t1_in_array,2);
	start_time_while(task_regulator,2);
	
	while (1)	{
		
///get_temp		
	  if(check_time_while(task_get_temp)) {           
		  get_temperature_data();
			start_time_while(task_get_temp,1);
		}
		
///store_t1_in_array		
	  if(check_time_while(task_store_t1_in_array)) {  
		  store_t1_in_array();
			start_time_while(task_store_t1_in_array,5);
		}
		
///adjust_power		
		if(check_time_while(task_regulator)) {        
		if(eeprom_data.power_control_mode==position){
			if(temp_data.t1>=(eeprom_data.preset_temp<<1)) output_power.power=0;
			else output_power.power=99;
		}
		else if (eeprom_data.power_control_mode==PWM){
		      if (temp_data.t1<((eeprom_data.preset_temp-20)<<1)) output_power.power=99;
		      else output_power.power=calc_PWM();	
		     }
		start_time_while(task_regulator,2);
	  }
		
///set_power		
    if(check_time_while(task_set_power)){           
      if(output_power.previous_power!=output_power.power){
			  init_tim1((257*(int)output_power.power)/100); 
				output_power.previous_power=output_power.power;
			}
	    start_time_while(task_set_power,4);
	  }
		menu=encoder.but_data;
		
 //set chenged_menu flag-cmf		
		if(previous_menu!=menu){               
		  cmf=1;
			previous_menu=menu;
		}
		else cmf=0;
		
//*****//MENU/*****//
//print current power
	  if     (menu==screen_1) 	{
			if(cmf){
			  encoder_setter(0,99,eeprom_data.power_0);
				oled_Clear_Screen();
				db.t1=0;
				db.t2=0;
				db.t3=0;
				db.t4=0;
				db.t5=0;
				db.t6=0;
				db.power=100;
			}
			
			if(eeprom_data.power_control_mode==manual)	{
			  output_power.power=encoder.enc_data;
			  if(encoder.enc_data!=eeprom_data.power_0) write_eeprom(&eeprom_data.power_0,encoder.enc_data);
		  }
			
			print_buf_number (output_power.power,&db.power,0,0,GIGA);	
			print_buf_number (temp_data.t1>>1,&db.t1,40,0,MEGA);
      print_buf_number (temp_data.t2>>1,&db.t2,70,0,MEGA);	
      print_buf_number (temp_data.t3>>1,&db.t3,100,0,MEGA);			
			print_buf_number (temp_data.t4>>1,&db.t4,40,2,MEGA);
      print_buf_number (temp_data.t5>>1,&db.t5,70,2,MEGA);	
      print_buf_number (temp_data.t6>>1,&db.t6,100,2,MEGA);	
			
		}
//print  temperature grafic		
		else if(menu==screen_2) 	{
			if(cmf){
			start_time_while(task_print_screen2,10);
			print_screen2();
	    }
			if(check_time_while(task_print_screen2)) {
				print_screen2();
				start_time_while(task_print_screen2,10);
			}
	  }

//set preset temp		
		else if(menu==screen_3) 	{
			if(cmf){
			  encoder_setter(40,99,eeprom_data.preset_temp);
				oled_Clear_Screen();
				db.preset_temp=0;
			}
			print_buf_number(eeprom_data.preset_temp,&db.preset_temp,1,1,GIGA);
			if (encoder.enc_data!=eeprom_data.preset_temp) write_eeprom(&eeprom_data.preset_temp,encoder.enc_data);
		}
		
//set power control mode		
		else if(menu==screen_4) 	{
			if(cmf){
			  encoder_setter(0,2,eeprom_data.power_control_mode);
				oled_Clear_Screen();
				start_time_while(task_cursor,1);
				db.power_control_mode=idle;
			}
			print_buf_number(eeprom_data.power_control_mode,&db.power_control_mode,1,0,MEGA);
			
			
			if(encoder.enc_data!=eeprom_data.power_control_mode) write_eeprom(&eeprom_data.power_control_mode,encoder.enc_data);
		}
		
//set  set PID-const 	-1	
		else if((menu==screen_5)||(menu==screen_6)||(menu==screen_7)||(menu==screen_8)) 	{
			if(cmf){
			  encoder_setter(1,99,eeprom_data.kp);
				oled_Clear_Screen();
				start_time_while(task_cursor,1);
			  if(menu==screen_5){db.kp=0;encoder_setter(1,99,eeprom_data.kp);}
			  if(menu==screen_6){db.ki=0;encoder_setter(1,99,eeprom_data.ki);}
			  if(menu==screen_7){db.kd=0;encoder_setter(1,99,eeprom_data.kd);}
			  if(menu==screen_8){db.kki=0;encoder_setter(1,99,eeprom_data.kki);}
				
			}
			
			print_buf_number(eeprom_data.kp,&db.kp,0,2,MEGA);
			print_buf_number(eeprom_data.ki,&db.ki,30,2,MEGA);
			print_buf_number(eeprom_data.kd,&db.kd,60,2,MEGA);
			print_buf_number(eeprom_data.kki,&db.kki,90,2,MEGA);
			
		if(check_time_while(task_cursor)) {oled_print_mega_char('_',50,0);start_time_while(task_gap,1);}
		if(check_time_while(task_gap)) {oled_print_mega_char(' ',50,0);start_time_while(task_cursor,1);}
			
			if(menu==screen_5)if(encoder.enc_data!=eeprom_data.kp) write_eeprom(&eeprom_data.kp,encoder.enc_data);
			if(menu==screen_6)if(encoder.enc_data!=eeprom_data.ki) write_eeprom(&eeprom_data.ki,encoder.enc_data);
			if(menu==screen_7)if(encoder.enc_data!=eeprom_data.kd) write_eeprom(&eeprom_data.kd,encoder.enc_data);
			if(menu==screen_8)if(encoder.enc_data!=eeprom_data.kki) write_eeprom(&eeprom_data.kki,encoder.enc_data);
		}
//set  indicate calc.power members	
		else if(menu==screen_9) 	{
			if(cmf){
			 				oled_Clear_Screen();
							db.t_p1=100;
							db.t_p2=100;
							db.t_p3=100;
			}
			
			if (temp_data.t_p1>=100) t_p1=9; 
			  else if(temp_data.t_p1<=-100)t_p1=19;
				else if(temp_data.t_p1<0) t_p1=0-temp_data.t_p1/10+10;
				else t_p1=temp_data.t_p1/10;
			if (temp_data.t_p2>=100) t_p2=9; 
			  else if(temp_data.t_p2<=-100)t_p2=19;
				else if(temp_data.t_p2<0) t_p2=0-temp_data.t_p2/10+10;
				else t_p2=temp_data.t_p2/10;			
			if (temp_data.t_p3>=100) t_p3=9; 
			  else if(temp_data.t_p3<=-100)t_p3=19;
				else if(temp_data.t_p3<0) t_p3=0-temp_data.t_p3/10+10;
				else t_p3=temp_data.t_p3/10;
				
			print_buf_number(t_p1,&db.t_p1,0,2,MEGA);
			print_buf_number(t_p2,&db.t_p2,40,2,MEGA);
			print_buf_number(t_p3,&db.t_p3,70,2,MEGA);
			

		}
//set  indicate resistance r1, r2
		else if(menu==screen_10) 	{
			if(cmf){
			 				oled_Clear_Screen();
							db.r1=0;
							db.r2=0;
							db.t2=0;
							
							
			}
				
			print_buf_number_int(temp_data.r1,&db.r1,0,0,MEGA);
			print_buf_number_int(temp_data.r2,&db.r2,0,2,MEGA);
			print_buf_number(temp_data.t2>>1,&db.t2,100,2,MEGA);

			

		}			
	}
}
	
//print temperature graphic
void print_screen2(void){ 
	 extern @near char array_data_temp [256];
	 char data_print; 
	 char y;
	 int i;

   oled_Clear_Screen();
   sendCmd(oled_32_128_adr,0x00); 
   sendCmd(oled_32_128_adr,0x100);
     
      for (i=0;i<256;i+=2) 
         {
				 if(array_data_temp[i]==0) array_data_temp[i]=40;
           data_print=(array_data_temp[i]-40)/5;
           y=3-(data_print/8);
           sendCmd(oled_32_128_adr,0xb0|y); 
           send_single_data(oled_32_128_adr,1<<(7-data_print%8));
         }
}
	
void print_buf_number(char value,char * db_value,char x,char y,char font ){
	extern power_control_mode_t power_control_mode;
if (value!=*db_value) {
*db_value=value;
oled_print_nomber(2,value,x,y,font );
}
}

void print_buf_number_int (int value,int * db_value,char x,char y,char font ){
	extern power_control_mode_t power_control_mode;
 
if (value!=*db_value) {
*db_value=value;
      if(value<0) {oled_print_mega_char('-',x,y);x+=12;value=0-value;}
			else {oled_print_mega_char(' ',x,y);x+=12;}
x=oled_print_nomber(2,value,x,y,font );
oled_print_mega_char(' ',x,y);
}
}
	
void out(out_data_t pin_out,out_data_t event ){ //set pinout
	switch(pin_out){
	case c3: {
	if(event==on) 
	GPIOC->ODR|=(1<<3); 
	else 
	GPIOC->ODR&=~(1<<3);
	break;}
	
	case b5: {
	if(event==on) 
	GPIOB->ODR|=(1<<5); 
	else 
	GPIOB->ODR&=~(1<<5);
	break;}
	}
}
	
void start_time_while(task_t task, int seconds){
		extern sys_time_t sys_time;
		unsigned long int current_seconds=seconds+sys_time.relative_seconds+(long int)sys_time.relative_hours*3600;
		
		*((&(sys_time.time_label_seconds[0]))+*(&task))=current_seconds%3600;
		*((&sys_time.time_label_hours[0])+*(&task))=current_seconds/3600;
		*(&sys_time.task_event_f[0]+task)=0;
}
	
char check_time_while(task_t task){
	  extern sys_time_t sys_time;
		if(*(&sys_time.task_event_f[0]+task)){
				 *(&sys_time.task_event_f[0]+task)=0;
				 return 1;
		}
		else 
		     return 0;
}
