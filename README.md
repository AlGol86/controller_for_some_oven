# controller_for_some_oven
## This device consists of several details:
* MCU stm8s103f3p
* Encoder with embedded button
* OLED display 128*32 (I2C interface)
* ntc temperature sensors/lm75a temperature sensors (quantity of sensors can be adjusted)
* Solid relay, load (heater)
![appearance](https://github.com/AlGol86/controller_for_some_oven/blob/main/pic/Prototype.jpg)
### Some point:
 **To provide time-management for every task there was implemented some ideas:**
 - Counting seconds from reset by interrupt handler (embedded timer TIM1), checking matches set time-labels for every task
 - In the begining (during initialization) execute method "SET_TIME_WHILE(task_t task, time_while_seconds)" for every task
 - In the main thread execute checking "CHECK_TASK_FLAG(task_t task)" for every task. If flag==1 for the task, perform the task and execute "SET_TIME_WHILE(task_t task, time_while_seconds)" (else skip the task).
   _This allows to create a long list of tasks and execute they in the time rulls_**
