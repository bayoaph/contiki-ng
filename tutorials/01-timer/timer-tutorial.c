/**
 *
 * File: timer-tutorial.c
 * Author: Mark Anthony Cabilo
 * Date: December 2019
 *
 * Brief: Illustrates the use of different timers
 *        available in Contiki OS
**/

/* contiki.h is a header file that includes
  all fundamental contiki libraries into this project
*/
#include "contiki.h"
#include <stdio.h>

/* The following libraries are found in core/sys folder
   of the contiki
*/
#include "sys/timer.h"
#include "sys/ctimer.h"
#include "sys/etimer.h"
#include "sys/rtimer.h"
#include "sys/stimer.h"

static struct rtimer rt; //rtimer object and ctimer object will be used across functions
static struct ctimer ct;


void ctimer_function(void* data);
void rtimer_function(void* data);

PROCESS(process, "Process for Contiki Timers");
AUTOSTART_PROCESSES(&process);

PROCESS_THREAD(process, ev, data){
  /* Timer declarations */
  static struct timer nt;   //timer object to use the timer library
  static struct stimer st;  //stimer object
  static struct etimer et;  //etimer object

  static int etime_count;

  PROCESS_BEGIN();

  /* using timer objects require three things: (1) declaring it, (2) setting them up, and
(3) resetting it (optional) */
  timer_set(&nt, 5*CLOCK_SECOND); //setting up timer object
  printf("timer object set for 5 seconds\n");

  while(!timer_expired(&nt));
  printf("timer object expired\n");

  /* timer and stimer are the same in function and usage. Their main difference is that timer
receives input in terms of ticks (CLOCK_SECOND is a multiplier that converts
number of ticks per second) while stimer accepts values in terms of seconds */
  stimer_set(&st, 5);
  printf("stimer object set for 5 seconds\n");

  while(!stimer_expired(&st));
  printf("stimer obhect expired\n");

  /* etimers can generate events. This helps you use timeres across multiple processes as well
as putting a process to sleep while not needed. (More discussion on processes) */
  etimer_set(&et, 5*CLOCK_SECOND);
  printf("etimer object set for 5 seconds\n");

  etime_count = 0;
  while(etime_count < 3){
    /* leaves process at this point until etimer expires */
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    printf("etimer object expires\n");

    etime_count++;

    /* you can reset any timer with the following function (the ff resets etimer) */
    printf("Resetting etimer %i times\n", etime_count);
    etimer_reset(&et);
  }

  /* ctimers when expired, calls a callback function */
  ctimer_set(&ct, 5*CLOCK_SECOND, ctimer_function, NULL);
  printf("ctimer object set for 5 seconds\n");

  /* then the process terminates */
  PROCESS_END();
}

/* even if the process terminates, any pending timers would still be running. The following 
function will only run once ctimer expires */
void
ctimer_function(void* data){
  printf("ctimer callback function running and ctimer object expired\n");

  /* rtimers are similar to ctimers except they base their time to CPU run time */
  printf("rtimer set for 5 seconds\n");
  rtimer_set(&rt, RTIMER_NOW()+5*RTIMER_SECOND, 0, rtimer_function, NULL);
}

void
rtimer_function(void* data){
  printf("rtimer callback function running and rtimer object expired\n");
  printf("\ntimer-tutorial ends\n");
}
