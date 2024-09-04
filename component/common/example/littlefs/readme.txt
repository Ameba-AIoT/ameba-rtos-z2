Description
~~~~~~~~~~~
This example is for littlefs test.


Setup Guide
~~~~~~~~~~~
    1. Include littlefs source code to project.
        - Add path:
          /component/common/file_system/littlefs
          /component/common/file_system/littlefs/r2.9.1
        - Add source:
          /component/common/file_system/littlefs/littlefs_adapter.c
          /component/common/file_system/littlefs/r2.9.1/lfs.c
          /component/common/file_system/littlefs/r2.9.1/lfs_util.c

    2. Include littlefs example code to project.
        - Add example_littlefs.c to .mk file
        - Add caller of example_littlefs() to example_entry.c

    3. Add/Enable CONFIG_EXAMPLE_LITTLEFS in platform_opts.h.
        [platform_opts.h]
        #define CONFIG_EXAMPLE_LITTLEFS    1

    4. In component/common/file_system/littlefs/littlefs_adapter.h, set LFS_FLASH_BASE_ADDR and LFS_DEVICE_SIZE, which means the address and size of littlefs respectively.


Result description
~~~~~~~~~~~~~~~~~~
    A littlefs example thread is started automatically when booting.
    It will read and write on Flash.