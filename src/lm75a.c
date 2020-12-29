
#include "stm8s.h"
#include "main.h"


int get_temp_11(char addr)
{
int t=0,tt=0;
start_iic (addr, READ);
t=receive_byte (1);
tt=receive_byte (0);
stop_iic ();
t<<=3;
tt>>=5;
t+=tt;
return (t);
}

char get_temp_8(char addr)//double temperature (0-125C, value=0-250) t=value/2
{
char t=0,tt=0,sensor_enable=0;
sensor_enable=start_iic (addr, READ);
t=receive_byte (1);
tt=receive_byte (0);
t<<=1;                //!without sign
tt>>=7;
t+=tt;
stop_iic ();
if (sensor_enable) return t;
else return 0;
}