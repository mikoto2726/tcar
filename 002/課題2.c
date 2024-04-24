#include <tk/tkernel.h>
#include <tm/tmonitor.h>

/* GPIO(Port-E) */
#define PEDATA      0x400C0400      // Data Register
#define PECR        0x400C0404      // Output Control register
#define PEIE        0x400C0438      // Input Control register

INT  usermain(void)
{
    *(_UW*)PEIE &= ~((1<<2) | (1<<3)); // Clear bit 2 and 3 to disable interrupts
    *(_UW*)PECR |= (1<<2) | (1<<3);   // Set bit 2 and 3 to configure as output
    tm_printf("Please open http://127.0.0.1:8888\n");

    for (int i = 0; i < 3; ++i) {
        // Left LED on, Right LED off
        *(_UW*)PEDATA |= (1<<2);   // Turn on left LED
        *(_UW*)PEDATA &= ~(1<<3);  // Turn off right LED
        tk_dly_tsk(500);

        // Left LED off, Right LED on
        *(_UW*)PEDATA &= ~(1<<2);  // Turn off left LED
        *(_UW*)PEDATA |= (1<<3);   // Turn on right LED
        tk_dly_tsk(500);
    }

    // Turn off both LEDs
    *(_UW*)PEDATA &= ~((1<<2) | (1<<3));

    return 0;
}
