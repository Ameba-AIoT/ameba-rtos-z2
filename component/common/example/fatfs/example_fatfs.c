#include "platform_opts.h"

#if defined(CONFIG_EXAMPLE_FATFS) && CONFIG_EXAMPLE_FATFS
#include "FreeRTOS.h"
#include "task.h"
#include <platform/platform_stdlib.h>
#include "basic_types.h"
#include "section_config.h"

#if CONFIG_FATFS_EN
#include "ff.h"
#include <fatfs_ext/inc/ff_driver.h>

#if FATFS_DISK_FLASH
#if (_MAX_SS != 4096)
#error set _MAX_SS to 4096 in ffconf.h to define maximum supported range of sector size for flash memory. See the description below the MACRO for details.
#endif
#if (_USE_MKFS != 1)
#error define _USE_MKFS MACRO to 1 in ffconf.h for on-board flash memory to enable f_mkfs() which creates FATFS volume on Flash.
#endif
#include "flash_api.h"
#include <disk_if/inc/flash_fatfs.h>
#define STACK_SIZE		4096
#endif

#define TEST_SIZE		512

u8 WRBuf[TEST_SIZE];
u8 RDBuf[TEST_SIZE];
FRESULT list_files(char *);
FRESULT del_dir(const TCHAR *path, int del_self);
FATFS fs_sd;
FIL m_file;

void example_fatfs_thread(void *param)
{
	int drv_num = 0;
	int Fatfs_ok = 0;

	FRESULT res;
	FATFS m_fs;
	FIL m_file;
	char logical_drv[4]; /* root diretor */
	char path[64], path_list[64];
	char filename[64] = "TEST.TXT";
	int br, bw;
	int ret = 0;
	int flash = 0;

	//1 register disk driver to fatfs
	printf("Register disk driver to Fatfs.\n");
#if FATFS_DISK_FLASH
	drv_num = FATFS_RegisterDiskDriver(&FLASH_disk_Driver);
	u8 test_info[] = "\"Ameba test fatfs flash~~~~\"";
	flash = 1;
#endif

	if (drv_num < 0) {
		printf("Register disk driver to FATFS fail.\n");
	} else {
		Fatfs_ok = 1;

		logical_drv[0] = drv_num + '0';
		logical_drv[1] = ':';
		logical_drv[2] = '/';
		logical_drv[3] = 0;
	}
	//1 Fatfs write and read test
	if (Fatfs_ok) {

		printf("FatFS Write/Read test begin......\n\n");

		res = f_mount(&m_fs, logical_drv, 1);
		if (res && flash) {
			if (f_mkfs(logical_drv, 1, 4096) != FR_OK) {
				printf("Create FAT volume on Flash fail.\n");
				goto fail;
			}
			if (f_mount(&m_fs, logical_drv, 0) != FR_OK) {
				printf("FATFS mount logical drive on Flash fail.\n");
				goto fail;
			}
		} else {
			if (res) {
				printf("FATFS mount logical drive fail.\n");
				goto fail;
			}
		}

		// write and read test
		strcpy(path, logical_drv);

		sprintf(&path[strlen(path)], "%s", filename);

		//Open source file
		res = f_open(&m_file, path, FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
		if (res) {
			printf("open file (%s) fail.\n", filename);
			goto fail;
		}

		printf("Test file name:%s\n\n", filename);

		// clean write and read buffer
		memset(&WRBuf[0], 0x00, TEST_SIZE);
		memset(&RDBuf[0], 0x00, TEST_SIZE);

		strcpy((char *)&WRBuf[0], (char *)&test_info[0]);

		do {
			res = f_write(&m_file, WRBuf, strlen((char *)WRBuf), (UINT *)&bw);
			if (res) {
				f_lseek(&m_file, 0);
				printf("Write error.\n");
			}
			printf("Write %d bytes.\n", bw);
		} while (bw < strlen((char *)WRBuf));

		printf("Write content:\n%s\n", WRBuf);
		printf("\n");

		/* move the file pointer to the file head*/
		res = f_lseek(&m_file, 0);

		do {
			res = f_read(&m_file, RDBuf, strlen((char *)WRBuf), (UINT *)&br);
			if (res) {
				f_lseek(&m_file, 0);
				printf("Read error.\n");
			}
			printf("Read %d bytes.\n", br);
		} while (br < strlen((char *)WRBuf));

		printf("Read content:\n%s\n", RDBuf);

		// close source file
		res = f_close(&m_file);
		if (res) {
			printf("close file (%s) fail.\n", filename);
		}

		// List all files
		memset(path_list, 0x00, 64);
		strcpy(path_list, logical_drv);
		printf("List all files\n");
		res = list_files(path_list);
		if (res) {
			printf("list all files fail");
		}

		//
		if (f_mount(NULL, logical_drv, 1) != FR_OK) {
			printf("FATFS unmount logical drive fail.\n");
		}

		if (FATFS_UnRegisterDiskDriver(drv_num)) {
			printf("Unregister disk driver from FATFS fail.\n");
		}
	}

fail:
	vTaskDelete(NULL);
}

char *print_file_info(FILINFO fileinfo, char *fn, char *path)
{
	char info[256];
	char fname[64];
	memset(fname, 0, sizeof(fname));
	snprintf(fname, sizeof(fname), "%s", fn);

	snprintf(info, sizeof(info),
			 "%c%c%c%c%c  %u/%02u/%02u  %02u:%02u  %9u  %30s  %30s",
			 (fileinfo.fattrib & AM_DIR) ? 'D' : '-',
			 (fileinfo.fattrib & AM_RDO) ? 'R' : '-',
			 (fileinfo.fattrib & AM_HID) ? 'H' : '-',
			 (fileinfo.fattrib & AM_SYS) ? 'S' : '-',
			 (fileinfo.fattrib & AM_ARC) ? 'A' : '-',
			 (fileinfo.fdate >> 9) + 1980,
			 (fileinfo.fdate >> 5) & 15,
			 fileinfo.fdate & 31,
			 (fileinfo.ftime >> 11),
			 (fileinfo.ftime >> 5) & 63,
			 fileinfo.fsize,
			 fn,
			 path);
	printf("%s\n\r", info);
	return info;
}

FRESULT list_files(char *list_path)
{
	DIR m_dir;
	FILINFO m_fileinfo;
	FRESULT res;
	char *filename;
#if _USE_LFN
	char fname_lfn[_MAX_LFN + 1];
	m_fileinfo.lfname = fname_lfn;
	m_fileinfo.lfsize = sizeof(fname_lfn);
#endif
	char cur_path[64];
	//strcpy(cur_path, list_path);

	// open directory
	res = f_opendir(&m_dir, list_path);

	if (res == FR_OK) {
		for (;;) {
			strcpy(cur_path, list_path);
			// read directory and store it in file info object
			res = f_readdir(&m_dir, &m_fileinfo);
			if (res != FR_OK || m_fileinfo.fname[0] == 0) {
				break;
			}

#if _USE_LFN
			filename = *m_fileinfo.lfname ? m_fileinfo.lfname : m_fileinfo.fname;
#else
			filename = m_fileinfo.fname;
#endif
			if (*filename == '.' || *filename == '..') {
				continue;
			}

			// check if the object is directory
			if (m_fileinfo.fattrib & AM_DIR) {
				print_file_info(m_fileinfo, filename, cur_path);
				sprintf(&cur_path[strlen(list_path)], "/%s", filename);
				res = list_files(cur_path);
				//strcpy(list_path, cur_path);
				if (res != FR_OK) {
					break;
				}
				//list_path[strlen(list_path)] = 0;
			} else {
				print_file_info(m_fileinfo, filename, cur_path);
			}

		}
	}

	// close directory
	res = f_closedir(&m_dir);
	if (res) {
		printf("close directory fail: %d\n", res);
	}
	return res;
}

FRESULT del_dir(const TCHAR *path, int del_self)
{
	FRESULT res;
	DIR   m_dir;
	FILINFO m_fileinfo;
	char *filename;
	char file[_MAX_LFN + 1];
#if _USE_LFN
	char fname_lfn[_MAX_LFN + 1];
	m_fileinfo.lfname = fname_lfn;
	m_fileinfo.lfsize = sizeof(fname_lfn);
#endif

	res = f_opendir(&m_dir, path);

	if (res == FR_OK) {
		for (;;) {
			// read directory and store it in file info object
			res = f_readdir(&m_dir, &m_fileinfo);
			if (res != FR_OK || m_fileinfo.fname[0] == 0) {
				break;
			}

#if _USE_LFN
			filename = *m_fileinfo.lfname ? m_fileinfo.lfname : m_fileinfo.fname;
#else
			filename = m_fileinfo.fname;
#endif
			if (*filename == '.' || *filename == '..') {
				continue;
			}

			printf("del: %s\n\r", filename);
			sprintf((char *)file, "%s/%s", path, filename);

			if (m_fileinfo.fattrib & AM_DIR) {
				res = del_dir(file, 1);
			} else {
				res = f_unlink(file);
			}

		}

	}

	// close directory
	res = f_closedir(&m_dir);

	// delete self?
	if (res == FR_OK) {
		if (del_self == 1) {
			res = f_unlink(path);
		}
	}

	return res;
}

void example_fatfs(void)
{
	if (xTaskCreate(example_fatfs_thread, ((const char *)"example_fatfs_thread"), STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
		printf("\n\r%s xTaskCreate(example_fatfs_thread) failed", __FUNCTION__);
	}
}
#endif //#if CONFIG_FATFS_EN
#endif //#if defined(CONFIG_EXAMPLE_FATFS) && CONFIG_EXAMPLE_FATFS