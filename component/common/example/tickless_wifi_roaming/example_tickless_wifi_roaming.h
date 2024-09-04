#ifndef __EXAMPLE_TICKLESS_WIFI_ROAMING_H__
#define __EXAMPLE_TICKLESS_WIFI_ROAMING_H__

void example_tickless_wifi_roaming_init(void);
void example_tickless_wifi_roaming(void);
int wifi_roaming_write_ap_to_flash(u8 *data, u32 len);
int wifi_roaming_init_done_callback(void);
#endif
