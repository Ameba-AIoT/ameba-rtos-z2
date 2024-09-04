#include "osdep_service.h"
#include "device_lock.h"
#include "flash_api.h"
#include "lfs.h"

#ifndef CONFIG_USE_FLASHCFG
#define LFS_FLASH_BASE_ADDR (0x200000 - 0xA9000) // begin address
#define LFS_DEVICE_SIZE 0xA9000 // 512KB
#endif

extern struct lfs_config g_lfs_cfg;
extern lfs_t g_lfs;

/**
 * Interface between littlefs and flash read
 * @param  c      littlefs configuration
 * @param  block  The block to read from
 * @param  off    Offset address
 * @param  buffer The buffer to store the readback data
 * @param  size   The size of the data to read
 * @return
 */
int lfs_diskio_read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size);

/**
 * Interface between littlefs and flash write
 * @param  c      littlefs configuration
 * @param  block  The block to write to
 * @param  off    Offset address
 * @param  buffer The buffer which store the data to be written
 * @param  size   The size of the data to write
 * @return
 */
int lfs_diskio_prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size);

/**
 * Interface between littlefs and flash erase
 * @param  c      littlefs configuration
 * @param  block  The block to erase
 * @return
 */
int lfs_diskio_erase(const struct lfs_config *c, lfs_block_t block);

int lfs_diskio_sync(const struct lfs_config *c);

#ifdef LFS_THREADSAFE
int lfs_diskio_lock(const struct lfs_config *c);
int lfs_diskio_unlock(const struct lfs_config *c);
#endif