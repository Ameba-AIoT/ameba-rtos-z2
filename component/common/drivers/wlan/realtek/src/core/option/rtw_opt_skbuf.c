#include <section_config.h>
#include <osdep_service.h>
#include <skbuff.h>
#include "autoconf.h"

#ifdef CONFIG_PLATFORM_8710C
#if (SKB_PRE_ALLOCATE_RX == 1)
#define MAX_SKB_BUF_SIZE     2112
#else
#define MAX_SKB_BUF_SIZE     1658	// should >= the size in wlan driver
#endif //(SKB_PRE_ALLOCATE_RX == 1)
#else
#define MAX_SKB_BUF_SIZE     1650	// should >= the size in wlan driver
#endif
#define MAX_SKB_BUF_NUM      14		//14: High Performance test 8: Default
#define MAX_LOCAL_SKB_NUM    (MAX_SKB_BUF_NUM + 2)

/* DO NOT modify skb_buf and skb_data structure */
struct skb_buf {
	struct list_head list;
	struct sk_buff skb;
};

struct skb_data {
	/* starting address must be aligned by 32 bytes for km4 cache. */
	struct list_head list __attribute__((aligned(32)));
	unsigned char buf[MAX_SKB_BUF_SIZE];
	/* to protect ref when to invalid cache, its address must be
	 * aligned by 32 bytes. */
	atomic_t ref __attribute__((aligned(32)));
};

#if (SKB_PRE_ALLOCATE_RX == 1)
u8 g_skb_pre_allocate_rx = 1; 		// 1: Enable, 0: Disable
u8 g_exchange_lxbus_rx_skb = 1;		// 1: Enable, 0: Disable
u8 g_rx_q_desc_num = 6;			// 6: High Performance Test, 4: Default
#if (MAX_SKB_BUF_NUM == 8)
#error "Value of MAX_SKB_BUF_NUM should be changed to 14 for High Performance Test"
#endif
#endif
unsigned int nr_xmitframe = MAX_SKB_BUF_NUM;
unsigned int nr_xmitbuff = MAX_SKB_BUF_NUM;
int max_local_skb_num = MAX_LOCAL_SKB_NUM;
int max_skb_buf_num = MAX_SKB_BUF_NUM;

/* DO NOT access skb_pool and skb_data_pool out of wlan driver */
struct skb_buf skb_pool[MAX_LOCAL_SKB_NUM];

#define SKB_DATA_POOL_USING_GLOBAL_BUF	1
#if SKB_DATA_POOL_USING_GLOBAL_BUF
// SRAM_BD_DATA_SECTION default in SRAM. Can modify image2.icf to link to the end of SDRAM
SRAM_BD_DATA_SECTION
struct skb_data skb_data_pool[MAX_SKB_BUF_NUM];
void skb_data_size_check(int size)
{
	if (size != MAX_SKB_BUF_SIZE) {
		printf("\n\rAssert(%d == %d) failed on line %d in file %s", size, MAX_SKB_BUF_SIZE, __LINE__, __FILE__);
		HALT();
	}
}
#else
// Change to use heap (malloc) to save SRAM memory
SRAM_BD_DATA_SECTION
struct skb_data *skb_data_pool;

extern struct list_head skbdata_list;
extern int skbdata_used_num;
extern int max_skbdata_used_num;
void init_skb_data_pool(void)
{
	int i;

	skb_data_size_check(MAX_SKB_BUF_SIZE);
	//printf("\ninit_skb_data_pool\n");
	skb_data_pool = (struct skb_data *)rtw_zmalloc(max_skb_buf_num * sizeof(struct skb_data));
	if (!skb_data_pool) {
		printf("\nskb_data_pool alloc fail\n");
		return;
	}

	memset(skb_data_pool, '\0', max_skb_buf_num * sizeof(struct skb_data));
	INIT_LIST_HEAD(&skbdata_list);

	for (i = 0; i < max_skb_buf_num; i++) {
		INIT_LIST_HEAD(&skb_data_pool[i].list);
		list_add_tail(&skb_data_pool[i].list, &skbdata_list);
	}
	skbdata_used_num = 0;
	max_skbdata_used_num = 0;
}

void deinit_skb_data_pool(void)
{
	//printf("\ndeinit_skb_data_pool\n");
	rtw_mfree((uint8_t *)skb_data_pool, MAX_SKB_BUF_NUM * sizeof(struct skb_data));
}
#endif
