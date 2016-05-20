/*
* Copyright (c) 2016, Wind River Systems, Inc.
*
* Redistribution and use in source and binary forms, with or without modification, are
* permitted provided that the following conditions are met:
*
* 1) Redistributions of source code must retain the above copyright notice,
* this list of conditions and the following disclaimer.
*
* 2) Redistributions in binary form must reproduce the above copyright notice,
* this list of conditions and the following disclaimer in the documentation and/or
* other materials provided with the distribution.
*
* 3) Neither the name of Wind River Systems nor the names of its contributors may be
* used to endorse or promote products derived from this software without specific
* prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


/*
 * DESCRIPTION
 * A sample application to read a Analog-to-Digital port and display it's value
 *
 * In order to run this sample you will need a Intel Galileo Gen2 board, a
 * Grove adapter board, and a Grove potentiometer.  Both Grove parts are
 * available as part of the Galileo IOT Starter Kit.
 *
 * The program will read the voltage on ADC port 0 (A0 on the Grove) once a second
 * and print the current value.  If the value is 0, the program will exit.
 */


#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <ioLib.h>

#include "gpioutils.h"

/*******************************************************************************
* setup_pins - Setup the pin routing on the Galileo board so that the GPIO
*              pin is mapped to the Ax port on the Grove Shield
*
* Arguments: gpionum - Pin number to map
*
* Returns:  0 - Success
*           1 - Failed
*
*/

int setup_pins(int gpionum)
{

    /*
     * Create the required GPIO pin and reserve it
     */

    if (gpio_alloc(gpionum))
        {
        fprintf(stderr, "Error allocating GPIO %d, errno = %d\n", gpionum, errno);
        return (1);
        }

    if (gpio_set_direction(gpionum, "out"))
        {
        fprintf(stderr, "Error setting GPIO %d direction, errno = %d\n", gpionum, errno);
        return (1);
        }

    if (gpio_write_pin(gpionum, "1"))
        {
        fprintf(stderr, "Error setting GPIO %d value, errno = %d\n", gpionum, errno);
        return (1);
        }

      return(0);
}



/*******************************************************************************
* aio0 - Read the voltage on Analog port 0 and print it
*
* Runs a loop where the voltage on A0 is read once per second and printed on
* the console until a value of 0 is read which terminates the loop and
* returns.
*
* Arguments: None
*
* Returns:  N/A
*
*/

void aio0 ()
{
    char buf[64];
    int fd;
    int value;


    /* Open the ADC device 0, pin 0 */
    fd = open("/sys/bus/iio/devices/iio:device0/in_voltage0_raw",
              O_RDONLY, 0664);

    if (fd < 0)
        {
        fprintf (stderr, "ERROR: iio open failed! errno = %d\n", errno);
        return;
        }

    while (1)
        {
        read(fd, buf, 5*sizeof(char));       /* Read the ADC port */

        if (sscanf(buf, "%d", &value) == 1)  /* Convert and print */
            {
            printf ("A0 = %d\n", value);
            }
        else
            {
            fprintf (stderr, "Error reading voltage %s\n", buf);
            }

        if (value == 0)	/* Check for 0 volts and break out */
            break;

        sleep(1);       /* Wait approx 1 second */

        }

    printf("Read zero, exiting read loop\n");

    close (fd);
}


/*******************************************************************************
* main - User application entry function
*
* This the the entry point for a VxWorks User Mode application
*/

int main (int argc, char *argv[])
{

    if (setup_pins(49) != 0)    /* Set up the pin routing on the Galileo board */
        exit (-1);

    aio0();                    /* Run the ADC program */

    (void)gpio_dealloc(49);    /* Unreserve the GPIO pin */

    exit (0);
}
