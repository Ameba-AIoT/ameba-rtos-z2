Example Description

This example describes how to use the General Timer API.

Requirement Components:
    2 LED
    main.c
    main_s.c (Required in trustzone project)

Connect the two LED to port PA_19 and PA_20 respectively.

The two LED will blink at a different frequency.

Two timers are initialized in this example,
    1, Periodic Timer
    2, One Shut Timer

Note
    1, Supported Gtimer, TIMER0 to TIMER6.

Optional
If TrustZone is enabled, GTimer8 can only be used in secure zone.

GTimer8 init can be found in main_s.c
    
The nsc function call allows communication call from non-secure to secure environment.
Timer8 will start counter & prints periodic timer at 1 second interval.