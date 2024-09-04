#include "timer_api.h"

gtimer_t my_timer8;
int counter8 = 0;

static void timer8_timeout_handler(uint32_t id)
{
	printf("gtimer8 us_value %d\n", counter8++);
}

//GTimer8 is exclusively used in secure
void NS_ENTRY gtimer8_example_nsc(void)
{
	gtimer_init(&my_timer8, TIMER8);
	if (my_timer8.timer_adp.tid != 0xFF) {
		gtimer_start_periodical(&my_timer8, 1000000, (void *)timer8_timeout_handler, NULL);
	}
}
