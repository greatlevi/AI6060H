#ifndef POWERSAVE_H_
#define POWERSAVE_H_

typedef struct t_PS_TIMER{
    U8           en;
    clock_time_t tclock;
}PS_TIMER;

void findTIM(U8 *buf, U8 *end, clock_time_t intime);
void timerintr_handler(void);
void powersave_start_run(void);
void powersave_stop_run(void);
void Switch_Power_mode(U8 enable);
U8 is_waketime_alive();

#endif /* POWERSAVE_H_ */

