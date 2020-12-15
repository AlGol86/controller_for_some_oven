/*	BASIC INTERRUPT VECTOR TABLE FOR STM8 devices
 *	Copyright (c) 2007 STMicroelectronics
 */
#include "main.h"

typedef void @far (*interrupt_handler_t)(void);

struct interrupt_vector {
	unsigned char interrupt_instruction;
	interrupt_handler_t interrupt_handler;
};

@far @interrupt void NonHandledInterrupt (void)
{
	/* in order to detect unexpected events during development, 
	   it is recommended to set a breakpoint on the following instruction
	*/
	return;
}

@far @interrupt void TIM1Interrupt (void)
{
	extern sys_time_t sys_time;
	extern encoder_t encoder;
	task_t i;
	TIM1->SR1&=~TIM1_SR1_UIF;//flag "0"
	out(b5,off);
	sys_time.tick++;
	if (sys_time.tick&1) {
	  sys_time.relative_seconds++; //count seconds
          if (sys_time.relative_seconds==3600){
		  sys_time.relative_seconds=0;
		  sys_time.relative_hours++;
	  }
/*******************************************************************
** check time-event matches and set corresponding flag to the task**
********************************************************************/
	  for(i=task_1;i<=task_10;i++){//time_management
	    if((sys_time.relative_seconds==(*(&sys_time.time_label_seconds[0]+i)))\ //if hours & seconds match to the event-->>set "task_event_flag"
	    &&(sys_time.relative_hours==(*(&sys_time.time_label_hours[0]+i)))) {
		*(&sys_time.task_event_f[0]+i)=1;
	    }	
	  }
        }
	return;
}

@far @interrupt void TIM1InterruptCC (void)
{
		
	TIM1->SR1&=~TIM1_SR1_CC1IF;//flag "0"
	out(b5,on);
	return;
}

@far @interrupt void TIM2Interrupt (void)
{
	extern encoder_t encoder;
	
	TIM2->SR1&=~TIM2_SR1_UIF;//flag "0"
	encoder_handler(&encoder);
	
	return;
}

extern void _stext();     /* startup routine */

struct interrupt_vector const _vectab[] = {
	{0x82, (interrupt_handler_t)_stext}, /* reset */
	{0x82, NonHandledInterrupt}, /* trap  */
	{0x82, NonHandledInterrupt}, /* irq0  */
	{0x82, NonHandledInterrupt}, /* irq1  */
	{0x82, NonHandledInterrupt}, /* irq2  */
	{0x82, NonHandledInterrupt}, /* irq3  */
	{0x82, NonHandledInterrupt}, /* irq4  */
	{0x82, NonHandledInterrupt}, /* irq5  */
	{0x82, NonHandledInterrupt}, /* irq6  */
	{0x82, NonHandledInterrupt}, /* irq7  */
	{0x82, NonHandledInterrupt}, /* irq8  */
	{0x82, NonHandledInterrupt}, /* irq9  */
	{0x82, NonHandledInterrupt}, /* irq10 */
	{0x82, TIM1Interrupt}, /* irq11 */
	{0x82, TIM1InterruptCC}, /* irq12 */
	{0x82, TIM2Interrupt}, /* irq13 */
	{0x82, NonHandledInterrupt}, /* irq14 */
	{0x82, NonHandledInterrupt}, /* irq15 */
	{0x82, NonHandledInterrupt}, /* irq16 */
	{0x82, NonHandledInterrupt}, /* irq17 */
	{0x82, NonHandledInterrupt}, /* irq18 */
	{0x82, NonHandledInterrupt}, /* irq19 */
	{0x82, NonHandledInterrupt}, /* irq20 */
	{0x82, NonHandledInterrupt}, /* irq21 */
	{0x82, NonHandledInterrupt}, /* irq22 */
	{0x82, NonHandledInterrupt}, /* irq23 */
	{0x82, NonHandledInterrupt}, /* irq24 */
	{0x82, NonHandledInterrupt}, /* irq25 */
	{0x82, NonHandledInterrupt}, /* irq26 */
	{0x82, NonHandledInterrupt}, /* irq27 */
	{0x82, NonHandledInterrupt}, /* irq28 */
	{0x82, NonHandledInterrupt}, /* irq29 */
};
