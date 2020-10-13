/*
 * IO driver for Real-time lab IO card communicating with the Butterfly Real-Time Tester
 * Channels are defined from 1 to 3, where 1 is test A on the Butterfly, 2 is B and 3 is C
 * 0V on an IO channel is defined as the logical value 0, while 5V is the logical value 1
 *
 * 2011, Øyvind Netland
 */

#ifndef IO_H
#define IO_H

#include <comedilib.h>

#define IO_DEV 3
#define DI(x) (24-x)
#define DO(x) (16-x)
#define DO_PWR 0

// initialize three input and three output used for the BRTT tests
int io_init();

// write to the output value of a channel (1-3) to to a value (0 or 1)
void io_write(int channel, int value);

// read the input value on a channel (1-3)
int io_read(int channel);

#endif /* IO_h */
