
/* Philipp Hurni, University of Bern, December 2013 	 */
/* Modified for Zolertia Firefly - M. Cabilo, Apr 2019 */

#include "contiki.h"
#include "etimer.h"
#include <stdio.h>
#include <string.h>
#include "node-id.h" /* get the variable node_id that holds the own node id */
#include "dev/leds.h"
/*---------------------------------------------------------------------------*/
PROCESS(led_blink_process, "LED Blink Timer Process");
PROCESS(hello_world_process, "Hello World Timer Process");
/*---------------------------------------------------------------------------*/

PROCESS_THREAD(led_blink_process, ev, data)
{
  PROCESS_BEGIN();
  static struct etimer timer_blink; //timer_wait;
  // Blink pattern every 4 seconds

  while (1)
  {
    leds_on(LEDS_YELLOW);
    etimer_set(&timer_blink, CLOCK_SECOND * 1);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_blink));


    //leds_off(LEDS_BLUE);
    //etimer_set(&timer_wait, CLOCK_SECOND * 3);
    //PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_wait));
  }

  PROCESS_END();
}

PROCESS_THREAD(hello_world_process, ev, data)
{
  PROCESS_BEGIN();
  static struct etimer timer_startup, timer_wait, timer_led;
  static int print_count = 0; // Counter to track the number of prints
  etimer_set(&timer_startup, CLOCK_SECOND * 30);
  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_startup));

  while (print_count < 10) // Stop after 10 prints
  {
    printf("Hello, world\n");
    print_count++; // Increment the counter

    // Turn on the RED led
    leds_on(LEDS_RED);
    etimer_set(&timer_led, CLOCK_SECOND * 1);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_led));
    leds_off(LEDS_RED); // Turn off the red LED after 1 second

    etimer_set(&timer_wait, CLOCK_SECOND * 4);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_wait));
  }

  PROCESS_END();
}

// autostart processes (can also be more than one)
AUTOSTART_PROCESSES(&led_blink_process, &hello_world_process);

/* Exercise 1a: compile the Contiki OS and flash the node attached to the PC via USB with
 * the above process it usually works best with flashing when you are the superuser
 *
 * $ cd <item01-path>
 * $ make
 * $ make item01.upload
 *
 * then run the following command
 * $ make login
 *
 * you can now communicate with the sensor node over the USB interface
 * restart the sensor node with the reset button until you see
 * "Hello World" printed onto the screen
 */

/* Exercise 1b: uncomment the three lines with the timer, compile and flash it to the sensor.
 * Observe what happens
 */
  //ANSWER
  //There are three led and only the third one lights to Yellow

/* Exercise 1c: alter the program such that the node prints "Hello World" infinitely to the screen,
 * with the delay of 4 seconds in between each statement
 */
  //ANSWER
  //while (1)
  //{
  //  printf("Hello, world\n");
  //  etimer_set(&timer_wait, CLOCK_SECOND * 4);
  //  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_wait));
  //}

  //PROCESS_END();

/* Exercise 1d: alter the program such that the node stops printing "Hello World" after 10 times
 */
 //ANSWER:
 // PROCESS_BEGIN();
 // static struct etimer timer_startup, timer_wait;
 // static int print_count = 0; // Counter to track the number of prints

 // etimer_set(&timer_startup, CLOCK_SECOND * 30);
 // PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_startup));

 // while (print_count < 10) // Stop after 10 prints
 // {
 //   printf("Hello, world\n");
 //   print_count++; // Increment the counter
 //   etimer_set(&timer_wait, CLOCK_SECOND * 4);
 //   PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_wait));
 // }

 // PROCESS_END();
 //}

/* Exercise 1e: alter the program such that the node turns on the blue led in each loop,
 * keeps it turned on for 1 second and then continues.
 */
 //ANSWER: Since the blue led doesn't work when I use it, I chose red instead.
 //while (print_count < 10) // Stop after 10 prints
//{
//  printf("Hello, world\n");
//    print_count++; // Increment the counter
//
//    // Turn on the RED led
//    leds_on(LEDS_RED);
//    etimer_set(&timer_led, CLOCK_SECOND * 1);
//    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_led));
//   leds_off(LEDS_RED); // Turn off the red LED after 1 second
//
//    etimer_set(&timer_wait, CLOCK_SECOND * 4);
//    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_wait));
//  }
//
//  PROCESS_END();
//}


/* Exercise 1f: separate the led blinking logic from the "Hello World"
 * printing logic: use two different processes, one that a) infinitely turns on and off the blue
 * LED (blink time = 1s, repeat interval=4s) and one that b) infinitely prints "Hello World"
 * with an interval of 10s
 *
 * The two processes shall be started when the node boots. The first (a) shall start immediately,
 * the second (b) shall start 30s after bootup.
 */
 //ANSWERS:
 // Include Contiki and relevant libraries
//#include "contiki.h"
//#include "etimer.h"
//#include <stdio.h>
//#include <string.h>
//#include "node-id.h" /* get the variable node_id that holds the own node id */
//#include "dev/leds.h"

/*---------------------------------------------------------------------------*/
// Declare the LED blink and Hello World processes
//PROCESS(led_blink_process, "LED Blink Timer Process");
//PROCESS(hello_world_process, "Hello World Timer Process");
/*---------------------------------------------------------------------------*/

// Thread function for the LED blink process
//PROCESS_THREAD(led_blink_process, ev, data)
//{
//  PROCESS_BEGIN();
//  static struct etimer timer_blink; // Timer for LED blinking
//  // Blink pattern every 4 seconds
//
//  while (1)
//  {
//    leds_on(LEDS_YELLOW); // Turn on yellow LED
//    etimer_set(&timer_blink, CLOCK_SECOND * 1); // Set timer for 1 second
//    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_blink)); // Wait for timer expiration
//
//    // Uncomment below for a more complex blink pattern
//    // leds_off(LEDS_BLUE);
//    // etimer_set(&timer_wait, CLOCK_SECOND * 3);
//    // PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_wait));
//  }
//
//  PROCESS_END();
//}

// Thread function for the Hello World process
//PROCESS_THREAD(hello_world_process, ev, data)
//{
//  PROCESS_BEGIN();
//  static struct etimer timer_startup, timer_wait, timer_led;
//  static int print_count = 0; // Counter to track the number of prints
//  
//  etimer_set(&timer_startup, CLOCK_SECOND * 30); // Initial delay of 30 seconds
//  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_startup)); // Wait for startup timer
//
//  while (print_count < 10) // Stop after 10 prints
//  {
//    printf("Hello, world\n"); // Print message
//    print_count++; // Increment the counter
//
//    // Turn on the RED LED
//    leds_on(LEDS_RED);
//    etimer_set(&timer_led, CLOCK_SECOND * 1); // LED on for 1 second
//    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_led)); // Wait for timer expiration
//    leds_off(LEDS_RED); // Turn off the red LED
//
//    etimer_set(&timer_wait, CLOCK_SECOND * 4); // Wait for 4 seconds before next iteration
//    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_wait)); // Wait for timer expiration
//  }
//
//  PROCESS_END();
//}
//
// Auto-start the defined processes
//AUTOSTART_PROCESSES(&led_blink_process, &hello_world_process);
//  