#include "stm8s.h"


#define PORT_iic       GPIOC
#define pin_sda        3
#define bit_sda                           (1<<pin_sda)
#define pin_clk        4
#define bit_clk                           (1<<pin_clk)

#define WRITE 0
#define READ  1

#define PORT_enc       GPIOC
#define pin_enc_l      6
#define bit_enc_l                       (1<<pin_enc_l)
#define pin_enc_r      5
#define bit_enc_r                       (1<<pin_enc_r)
#define pin_enc_b      7
#define bit_enc_b                       (1<<pin_enc_b)


#define address_iic_oled 0x3c // 3c 
#define oled_32_128_adr  0x3c // 3c 
#define address_iic_t    0x48 //
#define address_iic_t1   (address_iic_t+0)
#define address_iic_t2   (address_iic_t+1)
#define address_iic_t3   (address_iic_t+3)
#define address_iic_t4   (address_iic_t+7)

#define bit_read    1
#define bit_write   0
#define ACK 0 //acknowledge
#define NOT_ACK 1 //not acknowledge

#define SMALL 0
#define MIDDLE 1
#define MEGA 2
#define GIGA 3
#define TERRA 4





#define PIN_ADC_chanal_3     2  //port D(ch#3)
#define BIT_ADC_chanal_3    (1<<PIN_ADC_chanal_3 )
#define PIN_ADC_chanal_4     3  //port D(ch#4)
#define BIT_ADC_chanal_4    (1<<PIN_ADC_chanal_4 )
#define PIN_ADC_chanal_5     5  //port D(ch#5)
#define BIT_ADC_chanal_5    (1<<PIN_ADC_chanal_5 )
#define PIN_ADC_chanal_6     6  //port D(ch#6)
#define BIT_ADC_chanal_6    (1<<PIN_ADC_chanal_6 )
#define BIT_MASK_ch3456      (BIT_ADC_chanal_3|BIT_ADC_chanal_4|BIT_ADC_chanal_5|BIT_ADC_chanal_6)

typedef enum {
manual,position,PWM,idle} power_control_mode_t;

typedef struct {
	char kp;
	char ki;
	char kki;
	char kd;
	char preset_temp;
	power_control_mode_t power_control_mode;
	char power_0;
}
eeprom_data_t;

typedef struct {
	char cnt;
	int cnt_button;
	char f_push :1;
	char f_long_push :1;
	
	signed char first_enc_data;
	signed char enc_data;
	signed char enc_data_lim_h;
	signed char enc_data_lim_l;
	char but_data;
	char but_data_lim;
	char but_data_lim_long;
} encoder_t;


typedef struct {
	char  t1;
	char  t2;
	char  t3;
	char  t4;
	char  t5;
	char  t6;
	int   r1;
	int   r2;
	char previous_temp;
	int  err_temp_integral;
	int  t_p1;
	int  t_p2;
	int  t_p3;
} temp_data_t;




typedef struct {
	char t1;
	char t2;
	char t3;
	char t4;
	char t5;
	char t6;
	char kp;
	char ki;
	char kd;
	char kki;
	char  t_p1;
	char  t_p2;
	char  t_p3;
	int r1;
	int r2;
	char power;
	char preset_temp;
	power_control_mode_t power_control_mode;
	
}display_buffer_t;

typedef struct {
	char tick;
	int relative_seconds;        //0-3600
	unsigned int relative_hours; //0-65535
	int time_label_seconds[15];
	unsigned int time_label_hours[15];
	char task_event_f[15] ;
		
} sys_time_t;

typedef struct {
	char previous_power;
	char power;
				
} output_power_t;

typedef enum {
on,off,c3,c4,c5,c6,c7,b5,od,pp} out_data_t;

typedef enum {
task_1,
task_get_temp,
task_regulator,
task_set_power,
task_store_t1_in_array,
task_print_screen2,
task_cursor,
task_gap,
task_10,
task_bound} task_t;

typedef enum {
screen_1,screen_2,screen_3,screen_4,screen_5,screen_6,screen_7,screen_8,screen_9,screen_10,screen_bound} menu_t;



void init_encoder(encoder_t* enc,signed char l,signed char h, char but_lim,char but_data_lim_long);
void encoder_handler(encoder_t* enc);

//iic
void sys_del_us(char del_us);
char start_iic (char adr, char read_write_bit);
char send_byte (char data_byte);
unsigned char receive_byte (char acknowledge);
void stop_iic (void);

void init_tim1(char ccr1h);
void init_tim2 (void);
@far @interrupt void TIM1Interrupt (void);
@far @interrupt void TIM1InterruptCC (void);



void init_ssd1306_2(char adr);
void delay(unsigned int del);
void sendCmd(char adr,char cmd);
void sendData(char adr,char* data, char count); //send [count] data byts 
void blankScreen(char adr,char n);
void oled_Clear_Screen(void);
void set_cursor(char adr, char x,char y );
void ff_string(char adr,char n);
void send_repeat_data(char adr,char data_byte,int n);
void send_single_data(char adr,char data_byte);
void oled_blink1(char left, char right, char width, char count); //cursor right-left effect
void oled_print_small_digit(char d,char x,char y);  //3*5
void oled_print_middle_digit(char d,char x,char y); //6*10
void oled_print_mega_digit(char d,char x,char y);   //9*15
char oled_print_mega_char(char c,char x,char y);
void oled_print_giga_digit(char d,char x,char y);   //12*20
char oled_print_giga_char(char c,char x,char y); //12*20
char oled_print_char(char c,char x,char y,char font);   
void oled_print_terra_digit(char d,char x,char y);  //18*30
char oled_print_graphic(char* data,char n,char x,char width);
char oled_print_nomber(char f_zero,int n,char pos,char y, char font );

void out(out_data_t pin_out,out_data_t event );
void out_init(out_data_t pin_out,out_data_t od_pp_out);

char get_temp_8(char addr);
int  get_temp_11(char addr);

void print_screen1(char power);
void print_screen2(void);
void print_buf_number(char value,char* db_value,char x,char y,char font );
void print_buf_number_int (int value,int * db_value,char x,char y,char font );
void encoder_setter(signed char lim_l,signed char lim_h,signed char first_value);
void get_temperature_data(void);
void data_conditions_getting(void);
void power_adjusting(void);
void start_time_while(task_t task, int seconds);
char check_time_while(task_t task);
void store_t1_in_array(void);
char calc_PWM(void);
void init_ADC_3456(void);
unsigned int get_ADC(char ch);
unsigned int resistance_calculation ( unsigned int r_value, unsigned int value_ADC);
unsigned int get_temp_ADC(char ch);
void write_eeprom(char* addr_eeprom,char data);
char get_temperature_ADC(int r_value);