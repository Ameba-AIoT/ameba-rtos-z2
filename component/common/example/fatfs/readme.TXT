FATFS EXAMPLE

Description:
	This example is to read/write on Flash memory using FAT Filesystem.

Configuration:
	1. To read and write on Flash, set the parameters as below
		--> Set CONFIG_EXAMPLE_FATFS to be 1 in platform_opts.h;
		--> Set FATFS_DISK_FLASH to be 1 in platform_opts.h under CONFIG_EXAMPLE_FATFS;
		--> Set _MAX_SS to be 4096 in ffconf.h to define the maximum sector size supported;
		--> Set _USE_MKFS to be 1 in ffconf.h to enable f_mkfs() function which creates FATFS volume on Flash;
		--> The stack size of the example needs to be at least 4096.

	2. Config the address of FATFS region on Flash
		--> Users can modify the starting address in platform_opts.h according to their requirements, by default 512 KB is allocated out of 2MB Flash memory, with starting address of "#define FLASH_APP_BASE (0x200000 - 0xA9000)";
		--> If FLASH_APP_BASE is not defined in platform_opts.h, it will take the value from flash_fatfs.c with starting address of "#define FLASH_APP_BASE 0x180000";
		--> Make sure the FATFS region on Flash doesn't conflict with reserved Flash region.

Execution:
	A FatFs thread will be started automatically when booting.

[Supported List]
	Supported :
		Ameba-z2
